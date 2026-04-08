#include "AuthService.h"
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>

std::string AuthService::hashPassword(const std::string& plain)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(plain.c_str()),
           plain.size(), hash);
    std::ostringstream oss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    return oss.str();
}

bool AuthService::verifyPassword(const std::string& plain, const std::string& hash)
{
    return hashPassword(plain) == hash;
}
