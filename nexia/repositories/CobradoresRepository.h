#pragma once
#include <vector>
#include <string>
#include <functional>
#include <drogon/drogon.h>
#include "../models/Cobradora.h"

class CobradoresRepository {
public:
    static void listar(
        std::function<void(std::vector<Cobradora>)> callback,
        std::function<void(const std::string&)> errCallback);

    static void buscarPorId(
        int id,
        std::function<void(Cobradora)> callback,
        std::function<void(const std::string&)> errCallback);

    static void insertar(
        const Cobradora& c,
        std::function<void(int)> callback,
        std::function<void(const std::string&)> errCallback);

    static void actualizar(
        const Cobradora& c,
        std::function<void()> callback,
        std::function<void(const std::string&)> errCallback);

    static void eliminar(
        int id,
        std::function<void()> callback,
        std::function<void(const std::string&)> errCallback);
};
