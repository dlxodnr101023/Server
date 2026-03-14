#pragma once
#include <string>

class AccountRepository
{
public:

    static bool isUsernameExists(const std::string& username);

    static bool createAccount(
        const std::string& username,
        const std::string& passwordHash,
        const std::string& email);

    static bool verifyLogin(
        const std::string& username,
        const std::string& outPasswordHash);
};