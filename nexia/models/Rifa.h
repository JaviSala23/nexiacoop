#pragma once
#include <string>

struct Rifa {
    int    id_rifa{0};
    std::string nombre;
    std::string descripcion;
    std::string fecha_sorteo;
    int    cantidad_numeros{100};
    int    cant_cuotas{1};
    double monto_total{0.0};
    double monto_cuota{0.0};
    int    id_cuenta{0};
    std::string estado{"ACTIVA"}; // ACTIVA | CERRADA | SORTEADA
    std::string fecha_creacion;
    // Join data
    std::string nombre_cuenta;
    // Stats calculadas
    int    numeros_asignados{0};
    int    numeros_pagados{0};
    double total_recaudado{0.0};
};
