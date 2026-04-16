#pragma once
#include <string>

struct RifaNumero {
    int    id_numero{0};
    int    id_rifa{0};
    int    numero{0};
    int    id_familia{0};        // 0 si es externo
    std::string nombre_externo;
    std::string telefono_externo;
    double saldo_pendiente{0.0};
    std::string estado{"ASIGNADO"}; // ASIGNADO | PAGADO_PARCIAL | PAGADO | ANULADO
    std::string fecha_asignacion;
    // Join data
    int    numero_familia{0};
    std::string nombre_tutor_principal;
    std::string telefono_familia;
    std::string nombre_rifa;
    int    cant_cuotas{1};
    double monto_total{0.0};
    double monto_cuota{0.0};
    // Computed
    int    cuotas_pagadas{0};
    int    cuotas_pendientes{0};
};
