#pragma once
#include <string>

struct Familia {
    int    id_familia{0};
    int    numero_familia{0};
    std::string direccion;
    std::string telefono;
    std::string estado{"ACTIVA"};
    std::string fecha_alta;
    std::string fecha_baja;
    std::string observaciones;
    // Campos calculados (JOIN)
    int    cantidad_alumnos{0};
    std::string nombre_tutor_principal;
    std::string dni_tutor_principal;
    std::string telefono_tutor_principal;
};
