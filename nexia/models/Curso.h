#pragma once
#include <string>

struct Curso {
    int         id_curso  = 0;
    std::string nombre;
    int         orden     = 0;
    bool        activo    = true;
};
