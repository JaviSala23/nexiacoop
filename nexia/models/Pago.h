#pragma once
#include <string>

struct Pago {
    int    id_pago{0};
    int    id_talon{0};
    int    id_cobradora{0};
    std::string fecha_pago;
    double monto{0.0};
    std::string observaciones;
    // Join data
    int    mes{0};
    int    anio{0};
    std::string codigo_barra;
    std::string numero_socio;
    std::string nombre_socio;
    std::string nombre_cobradora;
};
