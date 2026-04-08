#pragma once
#include <string>

struct Tutor {
    int    id_tutor{0};
    int    id_familia{0};
    std::string dni;
    std::string nombre_completo;
    std::string telefono;
    std::string email;
    bool   es_principal{true};
};
