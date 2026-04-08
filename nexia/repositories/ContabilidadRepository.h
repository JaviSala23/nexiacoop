#pragma once
#include <functional>
#include <vector>
#include <string>
#include "../models/PlanCuenta.h"
#include "../models/Movimiento.h"
#include <json/json.h>

class ContabilidadRepository {
public:
    // Plan de cuentas
    static void listarCuentas(
        std::function<void(std::vector<PlanCuenta>)> callback,
        std::function<void(const std::string&)> errCallback);

    static void insertarCuenta(const PlanCuenta& c,
        std::function<void(int)> callback,
        std::function<void(const std::string&)> errCallback);

    static void actualizarCuenta(const PlanCuenta& c,
        std::function<void()> callback,
        std::function<void(const std::string&)> errCallback);

    static void toggleActivoCuenta(int id,
        std::function<void()> callback,
        std::function<void(const std::string&)> errCallback);

    // Movimientos
    static void listarMovimientos(const std::string& tipo,
        const std::string& desde, const std::string& hasta,
        std::function<void(std::vector<Movimiento>)> callback,
        std::function<void(const std::string&)> errCallback);

    static void insertarMovimiento(const Movimiento& m,
        std::function<void(int)> callback,
        std::function<void(const std::string&)> errCallback);

    static void eliminarMovimiento(int id,
        std::function<void()> callback,
        std::function<void(const std::string&)> errCallback);

    static void resumenPorCuenta(int anio,
        std::function<void(Json::Value)> callback,
        std::function<void(const std::string&)> errCallback);

    static void resumenMensual(int anio,
        std::function<void(Json::Value)> callback,
        std::function<void(const std::string&)> errCallback);
};
