#pragma once
#include <string>
#include <functional>
#include <json/json.h>

class PagoService {
public:
    // Registra el pago de un talón por código de barras
    static void registrarPorCodigo(
        const std::string& codigoBarra,
        int idCobradora,
        const std::string& observaciones,
        std::function<void(Json::Value resultado)> callback,
        std::function<void(const std::string&)> errCallback);
};
