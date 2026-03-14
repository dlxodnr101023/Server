#include <iostream>
#include <mailio/message.hpp>
#include <mailio/smtp.hpp>
#include <mail.h>
#include <random>

std::map<std::string, VerificationCode> verificationMap;
std::mutex mapMutex;

void saveCode(const std::string& email, uint32_t code)
{
    std::lock_guard<std::mutex> lock(mapMutex);
    verificationMap[email] = { code, time(nullptr) + 300 };
}

uint32_t generateCode()
{
    thread_local std::mt19937 gen(std::random_device{}());
    thread_local std::uniform_int_distribution<uint32_t> dist(100000, 999999);
    return dist(gen);
}


using namespace mailio;
using std::cout;
using std::endl;

void sendMail(int num, std::string email)
{
    try
    {
        message msg;
        msg.from(mail_address("UnderLayerStdio", "underlayerstdio@gmail.com"));
        msg.add_recipient(mail_address("User", email));
        msg.subject("Root&Loot 인증 코드");

        std::string html =
            "<html>"
            "<body style='font-family:Arial;background-color:#f4f4f4;padding:20px;'>"
            "<div style='max-width:500px;margin:auto;background:white;"
            "padding:20px;border-radius:10px;box-shadow:0 0 10px rgba(0,0,0,0.1);'>"
            "<h2 style='color:#2c3e50;'>Root&Loot 이메일 인증</h2>"
            "<p>안녕하세요.<br>아래 인증 코드를 입력해주세요.</p>"
            "<div style='font-size:32px;font-weight:bold;letter-spacing:5px;"
            "text-align:center;margin:20px;color:#3498db;'>" +
            std::to_string(num) +
            "</div>"
            "<p style='color:gray;font-size:12px;'>이 코드는 5분 후 만료됩니다.</p>"
            "</div></body></html>";

        msg.content(html);
        msg.content_type(message::media_type_t::TEXT, "html");

        // 4. SMTP 연결 및 인증
        smtps conn("smtp.gmail.com", 465);

        conn.authenticate(
            "underlayerstdio@gmail.com",
            "mryilycpueavloam",
            smtps::auth_method_t::LOGIN);

        conn.submit(msg);

        cout << "메일 전송 성공!" << endl;
    }
    catch (std::exception &e)
    {
        cout << e.what() << endl;
    }
}