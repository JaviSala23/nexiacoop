#pragma once
#include <string>

struct Movimiento {
    int    id_movimiento{0};
    int    id_cuenta{0};
    std::string fecha;
    std::string descripcion;
    double monto{0.0};
    std::string tipo; // INGRESO | EGRESO
    std::string medio_pago{"EFECTIVO"};
    int    id_pago{0}; // 0 = ingreso/egreso manual
    // Join data
    std::string nombre_cuenta;
    std::string codigo_cuenta;
};
