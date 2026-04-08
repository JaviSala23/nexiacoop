#pragma once
#include <string>

struct Talon {
    int    id_talon{0};
    int    id_socio{0};
    int    id_cobradora{0};
    int    mes{0};
    int    anio{0};
    double monto{0.0};
    std::string tipo;         // SIMPLE | CON_CARGO
    std::string codigo_barra;
    std::string estado{"GENERADO"}; // GENERADO | PAGADO | ANULADO
    std::string fecha_generacion;
    std::string fecha_pago;
    // Join data
    std::string nombre_socio;
    std::string nombre_cobradora;
    std::string direccion_socio;
    std::string localidad_socio;
    std::string provincia_socio;
    std::string telefono_socio;
    std::string dni_socio;
    std::string nombre_responsable_socio;
    // Datos del anexo (socio vinculado vía id_socio_principal)
    std::string nombre_anexo;
    std::string dni_anexo;
    int         numero_socio_anexo{0};
    std::string telefono_anexo;
};
