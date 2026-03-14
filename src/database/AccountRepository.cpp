#include "AccountRepository.hpp"
#include "database.hpp"

bool AccountRepository::isUsernameExists(const std::string& username)
{
    auto& conn = Database::instance().connection();

    pqxx::work txn(conn);

    pqxx::result r =
        txn.exec_params(
            "SELECT 1 FROM account WHERE username=$1",
            username);

    return !r.empty();
}

bool AccountRepository::createAccount(
    const std::string& username,
    const std::string& passwordHash,
    const std::string& email)
{
    auto& conn = Database::instance().connection();

    pqxx::work txn(conn);

    txn.exec_params(
        "INSERT INTO account(username,password_hash,email)"
        "VALUES($1,$2,$3)",
        username,
        passwordHash,
        email
    );

    txn.commit();

    return true;
}

bool AccountRepository::verifyLogin(
    const std::string& username,
    const std::string& outPasswordHash)
{
    auto& conn = Database::instance().connection();

    pqxx::work txn(conn);

    pqxx::result r =
        txn.exec_params(
            "SELECT password_hash FROM account WHERE username=$1",
            pqxx::params{username});

    if (r.empty())
        return false;

    return true;
}