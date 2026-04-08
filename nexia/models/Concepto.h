#pragma once
#include <string>

struct Concepto {
    int    id_concepto{0};
    std::string nombre;
    std::string descripcion;
    std::string tipo{"EXTRA"}; // MENSUAL | EXTRA
    bool   activo{true};
};
