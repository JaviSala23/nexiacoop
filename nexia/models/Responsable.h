#pragma once
#include <string>

struct Responsable {
    int    id_responsable{0};
    int    id_socio{0}; // socio al que pertenece (para inserción inline)
    std::string nombre_completo;
    std::string dni;
    std::string direccion;
    int    id_localidad{0};
    std::string telefono;
    std::string vinculo{"CONYUGE"};
};
