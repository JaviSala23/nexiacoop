#pragma once
#include <string>

struct RifaCuota {
    int    id_cuota{0};
    int    id_numero{0};
    int    nro_cuota{0};
    double monto{0.0};
    std::string estado{"PENDIENTE"}; // PENDIENTE | PAGADA | ANULADA
    std::string fecha_pago;
    std::string medio_pago;
    int    id_movimiento{0};
};
