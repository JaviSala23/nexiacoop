#include "ProvinciaRepository.h"
using namespace drogon;
using namespace drogon::orm;

void ProvinciaRepository::listarProvincias(
    std::function<void(std::vector<Provincia>)> cb,
    std::function<void(const std::string&)> err)
{
    app().getDbClient("main")->execSqlAsync(
        "SELECT id_provincia, nombre FROM provincias ORDER BY nombre",
        [cb](const Result& r) {
            std::vector<Provincia> v;
            for (const auto& row : r) {
                Provincia p;
                p.id_provincia = row["id_provincia"].as<int>();
                p.nombre       = row["nombre"].as<std::string>();
                v.push_back(std::move(p));
            }
            cb(std::move(v));
        },
        [err](const DrogonDbException& e) { err(e.base().what()); });
}

void ProvinciaRepository::listarLocalidades(
    int idProvincia,
    std::function<void(std::vector<Localidad>)> cb,
    std::function<void(const std::string&)> err)
{
    auto db = app().getDbClient("main");
    std::string sql =
        "SELECT l.id_localidad, l.nombre, l.id_provincia, p.nombre as nombre_provincia "
        "FROM localidades l JOIN provincias p ON l.id_provincia = p.id_provincia ";
    if (idProvincia > 0)
        sql += "WHERE l.id_provincia = " + std::to_string(idProvincia) + " ";
    sql += "ORDER BY l.nombre";

    db->execSqlAsync(sql,
        [cb](const Result& r) {
            std::vector<Localidad> v;
            for (const auto& row : r) {
                Localidad l;
                l.id_localidad    = row["id_localidad"].as<int>();
                l.id_provincia    = row["id_provincia"].as<int>();
                l.nombre          = row["nombre"].as<std::string>();
                l.nombre_provincia= row["nombre_provincia"].as<std::string>();
                v.push_back(std::move(l));
            }
            cb(std::move(v));
        },
        [err](const DrogonDbException& e) { err(e.base().what()); });
}

void ProvinciaRepository::insertarLocalidad(
    const std::string& nombre, int idProvincia,
    std::function<void(int)> cb,
    std::function<void(const std::string&)> err)
{
    app().getDbClient("main")->execSqlAsync(
        "INSERT INTO localidades (nombre, id_provincia) VALUES (?,?)",
        [cb](const Result& r) { cb(static_cast<int>(r.insertId())); },
        [err](const DrogonDbException& e) { err(e.base().what()); },
        nombre, idProvincia);
}

void ProvinciaRepository::eliminarLocalidad(
    int idLocalidad,
    std::function<void()> cb,
    std::function<void(const std::string&)> err)
{
    app().getDbClient("main")->execSqlAsync(
        "DELETE FROM localidades WHERE id_localidad=?",
        [cb](const Result&) { cb(); },
        [err](const DrogonDbException& e) { err(e.base().what()); },
        idLocalidad);
}

void ProvinciaRepository::insertarProvincia(
    const std::string& nombre,
    std::function<void(int)> cb,
    std::function<void(const std::string&)> err)
{
    app().getDbClient("main")->execSqlAsync(
        "INSERT INTO provincias (nombre) VALUES (?)",
        [cb](const Result& r) { cb(static_cast<int>(r.insertId())); },
        [err](const DrogonDbException& e) { err(e.base().what()); },
        nombre);
}

void ProvinciaRepository::eliminarProvincia(
    int idProvincia,
    std::function<void()> cb,
    std::function<void(const std::string&)> err)
{
    app().getDbClient("main")->execSqlAsync(
        "DELETE FROM provincias WHERE id_provincia=?",
        [cb](const Result&) { cb(); },
        [err](const DrogonDbException& e) { err(e.base().what()); },
        idProvincia);
}
