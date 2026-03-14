#include "database.hpp"
#include <stdexcept>

Database& Database::instance()
{
    static Database db;
    return db;
}

void Database::connect(const std::string& connStr)
{
    conn_ = std::make_unique<pqxx::connection>(connStr);

    if (!conn_->is_open())
        throw std::runtime_error("DB connection failed");
}

pqxx::connection& Database::connection()
{
    if (!conn_)
        throw std::runtime_error("DB not connected");

    return *conn_;
}