#include "password.h"
#include <argon2.h>
#include <vector>
#include <random>
#include <stdexcept>

static std::vector<uint8_t> randomBytes(size_t size)
{
    std::vector<uint8_t> buf(size);
    std::random_device rd;
    for (size_t i = 0; i < size; ++i)
        buf[i] = static_cast<uint8_t>(rd());
    return buf;
}

std::string hashPassword(const std::string& password)
{
    const uint32_t t_cost = 3;
    const uint32_t m_cost = 1 << 16;
    const uint32_t parallelism = 1;

    const size_t salt_len = 16;
    const size_t hash_len = 32;

    auto salt = randomBytes(salt_len);

    size_t encoded_len =
        argon2_encodedlen(t_cost, m_cost, parallelism, salt_len, hash_len, Argon2_id);

    std::vector<char> encoded(encoded_len);

    int result = argon2id_hash_encoded(
        t_cost,
        m_cost,
        parallelism,
        password.c_str(),
        password.size(),
        salt.data(),
        salt_len,
        hash_len,
        encoded.data(),
        encoded_len
    );

    if (result != ARGON2_OK)
        throw std::runtime_error(argon2_error_message(result));

    return std::string(encoded.data());
}

bool verifyPassword(const std::string& password, const std::string& hash)
{
    return argon2id_verify(hash.c_str(), password.c_str(), password.size()) == ARGON2_OK;
}