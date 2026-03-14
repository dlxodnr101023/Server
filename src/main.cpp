#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include <packet.pb.h>
#include "database.hpp"
#include "env.h"
#include "AccountRepository.hpp"
#include <mail.h>
#include <mutex>
#include <random>
#include <password.h>
#include "AccountRepository.hpp"
#include <sw/redis++/redis++.h>

using boost::asio::ip::tcp;

sw::redis::Redis redis("tcp://127.0.0.1:6379");

AccountRepository Account;

class session
    : public std::enable_shared_from_this<session>
{
public:
    session(tcp::socket socket)
        : socket_(std::move(socket))
    {
    }

    void start()
    {
        do_read_header();
    }

private:
    void do_read_header()
    {
        auto self(shared_from_this());

        boost::asio::async_read(
            socket_,
            boost::asio::buffer(&header_data_, sizeof(header_data_)),
            [this, self](boost::system::error_code ec, std::size_t)
            {
                if (!ec)
                {
                    // 네트워크 바이트 순서 사용 시 변환 필요
                    // header_data_ = ntohl(header_data_);

                    // 길이 검증 (필수)
                    if (header_data_ == 0)
                    {
                        std::cout << "잘못된 패킷 크기\n";
                        return;
                    }

                    body_data_.resize(header_data_);
                    do_read_body();
                }
            });
    }

    void do_read_body()
    {
        auto self(shared_from_this());

        boost::asio::async_read(
            socket_,
            boost::asio::buffer(body_data_),
            [this, self](boost::system::error_code ec, std::size_t length)
            {
                if (!ec)
                {
                    Protocol::PacketWrapper packet;

                    if (packet.ParseFromArray(body_data_.data(), static_cast<int>(length)))
                    {
                        std::cout << "수신 성공\n";

                        // payload 타입별 처리
                        switch (packet.payload_case())
                        {
                        case Protocol::PacketWrapper::kCheckIdReq: // Res → Req
                        {
                            const auto &request = packet.check_id_req();
                            std::cout << "받은 값 " << request.username() << std::endl;

                            Protocol::PacketWrapper wrapper;
                            auto *msg = wrapper.mutable_check_id_res();

                            msg->set_username(request.username());
                            msg->set_is_duplicated(AccountRepository::isUsernameExists(request.username()));

                            std::cout << "보낸 값 " << AccountRepository::isUsernameExists(request.username()) << std::endl;

                            send_packet(wrapper);
                            break;
                        }
                        case Protocol::PacketWrapper::kEmailRes:
                        {
                            const auto &request = packet.email_res();
                            std::cout << "받은 값 " << request.email() << std::endl;

                            uint32_t newCode = generateCode();

                            redis.set("auth:" + request.email(), std::to_string(newCode), std::chrono::minutes(5));

                            sendMail(newCode, request.email());

                            break;
                        }
                        case Protocol::PacketWrapper::kEmailRes2: // 인증번호 확인 요청
                        {
                            const auto &req = packet.email_res2();
                            std::string email = req.email();
                            std::string userInputCode = std::to_string(req.verilynum());

                            try
                            {
                                // 1. Redis에서 해당 이메일의 번호 꺼내기
                                auto realCode = redis.get("auth:" + email);

                                Protocol::PacketWrapper res;
                                auto *msg = res.mutable_email_req();

                                if (realCode && *realCode == userInputCode)
                                {
                                    // [성공] 번호가 일치함
                                    msg->set_isverily(true);
                                    std::cout << "[Auth] 성공: " << email << std::endl;
                                }
                                else
                                {
                                    // [실패] 번호가 틀렸거나 만료됨
                                    msg->set_isverily(false);

                                    std::cout << "[Auth] 실패: " << email << std::endl;
                                }

                                // 3. 결과 전송
                                send_packet(res);
                            }
                            catch (const sw::redis::Error &e)
                            {
                                std::cerr << "Redis Error: " << e.what() << std::endl;
                            }
                            break;
                        }
                        case Protocol::PacketWrapper::kMakeaccount:
                        {
                            const auto &request = packet.makeaccount();
                            auto email = request.email();
                            std::cout << "받은 값 " << email << std::endl;
                            auto mails = redis.get("auth:" + email);

                            Protocol::PacketWrapper res;
                            auto *msg = res.mutable_makeaccount();

                            if (mails) {
                                Account.createAccount(request.username(), hashPassword(request.password()),request.email());
                                std::cout << "계정 생성완료" << std::endl;
                            }

                            break;
                        }
                        case Protocol::PacketWrapper::kLogin:
                        {
                            const auto &request = packet.login();

                            if(Account.verifyLogin(request.username(), request.password())) {
                                printf("로그인 성공!");
                            }else{
                                printf("프린트 실패 데스네...ㅜㅜ");
                            }
                        }

                        default:
                            std::cout << "알 수 없는 payload\n";
                            break;
                        }
                    }
                    else
                    {
                        std::cout << "protobuf 파싱 실패\n";
                    }

                    // 다음 패킷 대기
                    do_read_header();
                }
            });
    }
    void send_packet(const Protocol::PacketWrapper &packet)
    {
        auto self(shared_from_this());

        std::string body;
        packet.SerializeToString(&body);

        ushort length = static_cast<ushort>(body.size());

        auto buffer = std::make_shared<std::vector<char>>();
        buffer->resize(sizeof(length) + body.size());

        std::memcpy(buffer->data(), &length, sizeof(length));
        std::memcpy(buffer->data() + sizeof(length), body.data(), body.size());

        boost::asio::async_write(
            socket_,
            boost::asio::buffer(*buffer),
            [this, self, buffer](boost::system::error_code ec, std::size_t)
            {
                if (ec)
                    std::cout << "send failed\n";
            });
    }

    tcp::socket socket_;
    uint16_t header_data_;
    std::vector<char> body_data_;
    enum
    {
        max_length = 1024
    };
    char data_[max_length];
};

class server
{
public:
    server(boost::asio::io_context &io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
    {
        do_accept();
    }

private:
    void do_accept()
    {
        acceptor_.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket)
            {
                if (!ec)
                {
                    std::make_shared<session>(std::move(socket))->start();
                }

                do_accept();
            });
    }

    tcp::acceptor acceptor_;
};

int main(int argc, char *argv[])
{
    try
    {
        if (argc != 2)
        {
            std::cerr << "Usage: async_tcp_echo_server <port>\n";
            return 1;
        }

        std::string conn_info = getEnvString();

        Database::instance().connect(conn_info);

        boost::asio::io_context io_context;

        server s(io_context, std::atoi(argv[1]));

        io_context.run();
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}