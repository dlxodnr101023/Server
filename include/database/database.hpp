#pragma once

#include <pqxx/pqxx>
#include <memory>
#include <string>

class Database
{
public:
    static Database& instance();

    void connect(const std::string& connStr);

    pqxx::connection& connection();

private:
    Database() = default;

    std::unique_ptr<pqxx::connection> conn_;
};