#pragma once
#include <string>

struct RifaPremio {
    int    id_premio{0};
    int    id_rifa{0};
    int    orden{1};
    int    numero_ganador{0};
    std::string nombre_ganador;
    std::string descripcion;
    double valor{0.0};
    bool   fue_donado{false};
    int    id_movimiento{0};
};
