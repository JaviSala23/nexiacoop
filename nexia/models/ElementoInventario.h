#pragma once
#include <string>

struct ElementoInventario {
    int    id_elemento{0};
    std::string nombre;
    std::string descripcion;
    int    stock_total{0};
    int    stock_disponible{0};
    bool   activo{true};
};
