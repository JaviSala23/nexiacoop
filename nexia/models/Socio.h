#pragma once
#include <string>

struct Socio {
    int    id_socio{0};
    int    numero_socio{0};
    std::string nombre_completo;
    std::string dni;
    std::string direccion;
    std::string telefono;
    int    id_localidad{0};
    int    id_cobradora{0};
    int    id_socio_principal{0}; // 0 o null si es principal
    std::string estado{"ACTIVO"};
    std::string fecha_alta;
    std::string fecha_baja;
    bool   tiene_responsable{false};
    // Join data
    std::string nombre_cobradora;
    // std::string nombre_responsable; // Eliminado, ya no se usa en el padrón
    std::string nombre_localidad;
    std::string nombre_provincia;
};
