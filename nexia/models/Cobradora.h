#pragma once
#include <string>

struct Cobradora {
    int    id_cobradora{0};
    std::string nombre;
    std::string telefono;
    double porcentaje_comision{0.0};
    bool   activa{true};
};
