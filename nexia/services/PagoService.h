#pragma once
#include <string>
#include <functional>
#include <json/json.h>

class PagoService {
public:
    // Registra el pago de un talón por código de barra.
    // Crea automáticamente el movimiento contable de INGRESO.
    static void registrarPorCodigo(
        const std::string& codigoBarra,
        const std::string& medioPago,
        const std::string& observaciones,
        std::function<void(Json::Value)> callback,
        std::function<void(const std::string&)> errCallback);
};
