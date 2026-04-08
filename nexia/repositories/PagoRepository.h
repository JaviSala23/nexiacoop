#pragma once
#include <vector>
#include <string>
#include <functional>
#include <drogon/drogon.h>
#include "../models/Pago.h"

class PagoRepository {
public:
    static void insertar(
        const Pago& p,
        std::function<void(int)> callback,
        std::function<void(const std::string&)> errCallback);

    static void listar(
        int mes, int anio,
        std::function<void(std::vector<Pago>)> callback,
        std::function<void(const std::string&)> errCallback);

    static void resumenMes(
        int anio,
        std::function<void(Json::Value)> callback,
        std::function<void(const std::string&)> errCallback);

    static void resumenCobradora(
        int mes, int anio,
        std::function<void(Json::Value)> callback,
        std::function<void(const std::string&)> errCallback);
};
