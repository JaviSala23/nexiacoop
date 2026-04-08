#include "CursoRepository.h"
#include <drogon/drogon.h>

using namespace drogon;
using namespace drogon::orm;

static Curso rowToCurso(const Row& row) {
    Curso c;
    c.id_curso = row["id_curso"].as<int>();
    c.nombre   = row["nombre"].as<std::string>();
    c.orden    = row["orden"].as<int>();
    c.activo   = row["activo"].as<int>() != 0;
    return c;
}

void CursoRepository::listar(
    std::function<void(std::vector<Curso>)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "SELECT * FROM cursos ORDER BY orden, nombre",
        [callback](const Result& r) {
            std::vector<Curso> lista;
            for (const auto& row : r) lista.push_back(rowToCurso(row));
            callback(std::move(lista));
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); });
}

void CursoRepository::insertar(const Curso& c,
    std::function<void(int)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "INSERT INTO cursos (nombre, orden, activo) VALUES (?,?,?)",
        [callback](const Result& r) { callback(static_cast<int>(r.insertId())); },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        c.nombre, c.orden, c.activo ? 1 : 0);
}

void CursoRepository::actualizar(int id, const Curso& c,
    std::function<void()> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "UPDATE cursos SET nombre=?, orden=?, activo=? WHERE id_curso=?",
        [callback](const Result&) { callback(); },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        c.nombre, c.orden, c.activo ? 1 : 0, id);
}

void CursoRepository::eliminar(int id,
    std::function<void()> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "DELETE FROM cursos WHERE id_curso=?",
        [callback](const Result&) { callback(); },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        id);
}
