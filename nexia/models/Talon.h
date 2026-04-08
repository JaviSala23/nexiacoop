#pragma once
#include <string>

struct Talon {
    int    id_talon{0};
    int    id_familia{0};
    int    id_concepto{0};
    int    mes{0};
    int    anio{0};
    double monto{0.0};
    std::string codigo_barra;
    std::string estado{"GENERADO"}; // GENERADO | PAGADO | ANULADO
    std::string fecha_generacion;
    std::string fecha_pago;
    std::string observaciones;
    // Join data
    int    numero_familia{0};
    std::string nombre_concepto;
    std::string tipo_concepto; // MENSUAL | EXTRA
    std::string nombre_tutor_principal;
    std::string dni_tutor_principal;
    std::string telefono_familia;
    std::string direccion_familia;
    int    cantidad_alumnos{0};
};
