#pragma once
#include <string>

struct Pago {
    int    id_pago{0};
    int    id_talon{0};
    std::string fecha_pago;
    double monto{0.0};
    std::string medio_pago{"EFECTIVO"}; // EFECTIVO | TRANSFERENCIA | MERCADOPAGO | OTRO
    std::string observaciones;
    // Join data
    int    mes{0};
    int    anio{0};
    int    numero_familia{0};
    std::string codigo_barra;
    std::string nombre_concepto;
    std::string nombre_tutor_principal;
};
