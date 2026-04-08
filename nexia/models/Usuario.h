#pragma once
#include <string>

struct Usuario {
    int    id_usuario{0};
    std::string username;
    std::string password_hash;
    std::string nombre;
    std::string rol{"OPERADOR"}; // ADMIN | OPERADOR
    bool   activo{true};
    std::string created_at;
};
