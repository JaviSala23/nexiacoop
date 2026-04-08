#pragma once
#include <vector>
#include <string>
#include <functional>
#include <drogon/drogon.h>
#include "../models/Usuario.h"

class UsuarioRepository {
public:
    static void listar(
        std::function<void(std::vector<Usuario>)> callback,
        std::function<void(const std::string&)> errCallback);

    static void buscarPorUsername(
        const std::string& username,
        std::function<void(Usuario, bool)> callback,
        std::function<void(const std::string&)> errCallback);

    static void insertar(
        const Usuario& u,
        std::function<void(int)> callback,
        std::function<void(const std::string&)> errCallback);

    static void actualizarPassword(
        int id,
        const std::string& passwordHash,
        std::function<void()> callback,
        std::function<void(const std::string&)> errCallback);

    static void cambiarEstado(
        int id,
        bool activo,
        std::function<void()> callback,
        std::function<void(const std::string&)> errCallback);
};
