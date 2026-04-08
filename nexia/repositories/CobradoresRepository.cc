#include "CobradoresRepository.h"

using namespace drogon;
using namespace drogon::orm;

void CobradoresRepository::listar(
    std::function<void(std::vector<Cobradora>)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "SELECT * FROM cobradoras ORDER BY nombre",
        [callback](const Result& r) {
            std::vector<Cobradora> lista;
            for (const auto& row : r) {
                Cobradora c;
                c.id_cobradora        = row["id_cobradora"].as<int>();
                c.nombre              = row["nombre"].as<std::string>();
                c.telefono            = row["telefono"].isNull() ? "" : row["telefono"].as<std::string>();
                c.porcentaje_comision = row["porcentaje_comision"].as<double>();
                c.activa              = row["activa"].as<bool>();
                lista.push_back(std::move(c));
            }
            callback(std::move(lista));
        },
        [errCallback](const DrogonDbException& e) {
            errCallback(e.base().what());
        });
}

void CobradoresRepository::buscarPorId(
    int id,
    std::function<void(Cobradora)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "SELECT * FROM cobradoras WHERE id_cobradora = ?",
        [callback, errCallback](const Result& r) {
            if (r.empty()) { errCallback("Cobradora no encontrada"); return; }
            const auto& row = r[0];
            Cobradora c;
            c.id_cobradora        = row["id_cobradora"].as<int>();
            c.nombre              = row["nombre"].as<std::string>();
            c.telefono            = row["telefono"].isNull() ? "" : row["telefono"].as<std::string>();
            c.porcentaje_comision = row["porcentaje_comision"].as<double>();
            c.activa              = row["activa"].as<bool>();
            callback(std::move(c));
        },
        [errCallback](const DrogonDbException& e) {
            errCallback(e.base().what());
        },
        id);
}

void CobradoresRepository::insertar(
    const Cobradora& c,
    std::function<void(int)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "INSERT INTO cobradoras (nombre, telefono, porcentaje_comision, activa) VALUES (?,?,?,?)",
        [callback](const Result& r) { callback(static_cast<int>(r.insertId())); },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        c.nombre, c.telefono, c.porcentaje_comision, c.activa ? 1 : 0);
}

void CobradoresRepository::actualizar(
    const Cobradora& c,
    std::function<void()> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "UPDATE cobradoras SET nombre=?, telefono=?, porcentaje_comision=?, activa=? WHERE id_cobradora=?",
        [callback](const Result&) { callback(); },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        c.nombre, c.telefono, c.porcentaje_comision, c.activa ? 1 : 0, c.id_cobradora);
}

void CobradoresRepository::eliminar(
    int id,
    std::function<void()> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "UPDATE cobradoras SET activa=0 WHERE id_cobradora=?",
        [callback](const Result&) { callback(); },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        id);
}
