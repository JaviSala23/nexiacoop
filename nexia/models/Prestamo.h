#pragma once
#include <string>

struct Prestamo {
    int    id_prestamo{0};
    int    id_elemento{0};
    int    id_socio{0};
    std::string fecha_prestamo;
    std::string fecha_devolucion;
    std::string observaciones;
    std::string estado{"PRESTADO"}; // PRESTADO | DEVUELTO
    // Join data
    std::string nombre_elemento;
    std::string nombre_socio;
    int    numero_socio{0};
    std::string dni_socio;
};
