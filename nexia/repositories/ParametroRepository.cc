#include "ParametroRepository.h"

using namespace drogon;
using namespace drogon::orm;

void ParametroRepository::obtener(
    std::function<void(Parametro)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "SELECT * FROM parametros ORDER BY id_parametro LIMIT 1",
        [callback, errCallback](const Result& r) {
            if (r.empty()) { errCallback("Sin parámetros configurados"); return; }
            const auto& row = r[0];
            Parametro p;
            p.id_parametro       = row["id_parametro"].as<int>();
            p.mes_cobranza       = row["mes_cobranza"].as<int>();
            p.anio_cobranza      = row["anio_cobranza"].as<int>();
            p.nombre_institucion = row["nombre_institucion"].as<std::string>();
            p.domicilio          = row["domicilio"].as<std::string>();
            p.telefono           = row["telefono"].as<std::string>();
            p.localidad          = row["localidad"].as<std::string>();
            p.provincia          = row["provincia"].as<std::string>();
            p.email              = row["email"].as<std::string>();
            p.logo_url           = row["logo_url"].as<std::string>();
            callback(std::move(p));
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); });
}

void ParametroRepository::actualizar(
    const Parametro& p,
    std::function<void()> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "UPDATE parametros SET mes_cobranza=?, anio_cobranza=?, "
        "nombre_institucion=?, domicilio=?, telefono=?, localidad=?, provincia=?, "
        "email=?, logo_url=? "
        "WHERE id_parametro=?",
        [callback](const Result&) { callback(); },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        p.mes_cobranza, p.anio_cobranza,
        p.nombre_institucion, p.domicilio, p.telefono, p.localidad, p.provincia,
        p.email, p.logo_url,
        p.id_parametro);
}

