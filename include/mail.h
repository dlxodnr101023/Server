#pragma once

#include <string>
#include <map>
#include <mutex>
#include <ctime>

void sendMail(int num, std::string email);

struct VerificationCode {
    uint32_t code;
    time_t   expireAt;

    VerificationCode(uint32_t c, time_t t) : code(c), expireAt(t) {}
    VerificationCode() : code(0), expireAt(0) {}
};

extern std::map<std::string, VerificationCode> verificationMap;
extern std::mutex mapMutex;

void     saveCode(const std::string& email, uint32_t code);
uint32_t generateCode();