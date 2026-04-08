#pragma once
#include <string>
#include <functional>

class AuthService {
public:
    static std::string hashPassword(const std::string& plain);
    static bool verifyPassword(const std::string& plain, const std::string& hash);
};
