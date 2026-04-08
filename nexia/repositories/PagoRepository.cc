#include "PagoRepository.h"

using namespace drogon;
using namespace drogon::orm;

void PagoRepository::insertar(
    const Pago& p,
    std::function<void(int)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    if (p.id_cobradora > 0) {
        db->execSqlAsync(
            "INSERT INTO pagos (id_talon, id_cobradora, monto, observaciones) VALUES (?,?,?,?)",
            [callback](const Result& r) { callback(static_cast<int>(r.insertId())); },
            [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
            p.id_talon, p.id_cobradora, p.monto, p.observaciones);
    } else {
        db->execSqlAsync(
            "INSERT INTO pagos (id_talon, monto, observaciones) VALUES (?,?,?)",
            [callback](const Result& r) { callback(static_cast<int>(r.insertId())); },
            [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
            p.id_talon, p.monto, p.observaciones);
    }
}

void PagoRepository::listar(
    int mes, int anio,
    std::function<void(std::vector<Pago>)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    std::string sql =
        "SELECT p.*, t.mes, t.anio, t.codigo_barra, "
        "s.nombre_completo as nombre_socio, s.numero_socio, c.nombre as nombre_cobradora "
        "FROM pagos p "
        "JOIN talones t ON p.id_talon = t.id_talon "
        "JOIN socios s ON t.id_socio = s.id_socio "
        "LEFT JOIN cobradoras c ON p.id_cobradora = c.id_cobradora "
        "WHERE 1=1 ";
    if (mes  > 0) sql += "AND t.mes = "  + std::to_string(mes)  + " ";
    if (anio > 0) sql += "AND t.anio = " + std::to_string(anio) + " ";
    sql += "ORDER BY p.fecha_pago DESC";
    db->execSqlAsync(sql,
        [callback](const Result& r) {
            std::vector<Pago> lista;
            for (const auto& row : r) {
                Pago p;
                p.id_pago        = row["id_pago"].as<int>();
                p.id_talon       = row["id_talon"].as<int>();
                p.id_cobradora   = row["id_cobradora"].as<int>();
                p.mes            = row["mes"].as<int>();
                p.anio           = row["anio"].as<int>();
                p.codigo_barra   = row["codigo_barra"].isNull() ? "" : row["codigo_barra"].as<std::string>();
                p.numero_socio   = row["numero_socio"].isNull() ? "" : std::to_string(row["numero_socio"].as<int>());
                p.fecha_pago     = row["fecha_pago"].as<std::string>();
                p.monto          = row["monto"].as<double>();
                p.observaciones  = row["observaciones"].isNull() ? "" : row["observaciones"].as<std::string>();
                p.nombre_socio   = row["nombre_socio"].as<std::string>();
                p.nombre_cobradora = row["nombre_cobradora"].isNull() ? "" : row["nombre_cobradora"].as<std::string>();
                lista.push_back(std::move(p));
            }
            callback(std::move(lista));
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); });
}

void PagoRepository::resumenMes(
    int anio,
    std::function<void(Json::Value)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    std::string sql =
        "SELECT t.mes, t.anio, COUNT(*) as total_pagos, SUM(p.monto) as total_monto "
        "FROM pagos p "
        "JOIN talones t ON p.id_talon = t.id_talon "
        "WHERE 1=1 ";
    if (anio > 0) sql += "AND t.anio = " + std::to_string(anio) + " ";
    sql += "GROUP BY t.anio, t.mes ORDER BY t.anio DESC, t.mes DESC";
    db->execSqlAsync(sql,
        [callback](const Result& r) {
            Json::Value arr(Json::arrayValue);
            for (const auto& row : r) {
                Json::Value o;
                o["mes"]         = row["mes"].as<int>();
                o["anio"]        = row["anio"].as<int>();
                o["total_pagos"] = row["total_pagos"].as<int>();
                o["total_monto"] = row["total_monto"].as<double>();
                arr.append(o);
            }
            callback(arr);
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); });
}

void PagoRepository::resumenCobradora(
    int mes, int anio,
    std::function<void(Json::Value)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    std::string sql =
        "SELECT c.id_cobradora, c.nombre as nombre_cobradora, "
        "COUNT(*) as total_pagos, SUM(p.monto) as total_monto "
        "FROM pagos p "
        "LEFT JOIN cobradoras c ON p.id_cobradora = c.id_cobradora "
        "JOIN talones t ON p.id_talon = t.id_talon "
        "WHERE 1=1 ";
    if (mes  > 0) sql += "AND t.mes = "  + std::to_string(mes)  + " ";
    if (anio > 0) sql += "AND t.anio = " + std::to_string(anio) + " ";
    sql += "GROUP BY c.id_cobradora, c.nombre ORDER BY total_monto DESC";
    db->execSqlAsync(sql,
        [callback](const Result& r) {
            Json::Value arr(Json::arrayValue);
            for (const auto& row : r) {
                Json::Value o;
                o["id_cobradora"]    = row["id_cobradora"].isNull() ? 0 : row["id_cobradora"].as<int>();
                o["nombre_cobradora"]= row["nombre_cobradora"].isNull() ? "Sin cobradora" : row["nombre_cobradora"].as<std::string>();
                o["total_pagos"]     = row["total_pagos"].as<int>();
                o["total_monto"]     = row["total_monto"].as<double>();
                arr.append(o);
            }
            callback(arr);
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); });
}
