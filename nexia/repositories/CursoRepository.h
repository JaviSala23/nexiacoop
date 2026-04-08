#pragma once
#include "../models/Curso.h"
#include <vector>
#include <functional>
#include <string>

struct CursoRepository {
    static void listar(
        std::function<void(std::vector<Curso>)> callback,
        std::function<void(const std::string&)> errCallback);

    static void insertar(const Curso& c,
        std::function<void(int)> callback,
        std::function<void(const std::string&)> errCallback);

    static void actualizar(int id, const Curso& c,
        std::function<void()> callback,
        std::function<void(const std::string&)> errCallback);

    static void eliminar(int id,
        std::function<void()> callback,
        std::function<void(const std::string&)> errCallback);
};
