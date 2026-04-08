#pragma once
#include <string>

struct PlanCuenta {
    int    id_cuenta{0};
    std::string codigo;
    std::string nombre;
    std::string tipo; // INGRESO | EGRESO
    bool   activo{true};
};
