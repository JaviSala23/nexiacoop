#pragma once
#include <string>

struct Alumno {
    int    id_alumno{0};
    int    id_familia{0};
    std::string dni;
    std::string nombre_completo;
    std::string fecha_nacimiento;
    std::string anio_escolar;
    std::string estado{"ACTIVO"};
};
