#include "PagoRepository.h"
#include <drogon/drogon.h>

using namespace drogon;
using namespace drogon::orm;

void PagoRepository::insertar(const Pago& p,
    std::function<void(int)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "INSERT INTO pagos (id_talon, monto, medio_pago, observaciones) "
        "VALUES (?,?,?,?)",
        [callback](const Result& r) { callback(static_cast<int>(r.insertId())); },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        p.id_talon, p.monto, p.medio_pago, p.observaciones);
}

void PagoRepository::listar(int mes, int anio, int idConcepto,
    std::function<void(std::vector<Pago>)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    std::string sql =
        "SELECT pg.*, t.mes, t.anio, t.codigo_barra, t.id_concepto, "
        "f.numero_familia, "
        "tp.nombre_completo AS nombre_tutor_principal, "
        "c.nombre AS nombre_concepto "
        "FROM pagos pg "
        "JOIN talones t ON pg.id_talon=t.id_talon "
        "JOIN familias f ON t.id_familia=f.id_familia "
        "JOIN conceptos c ON t.id_concepto=c.id_concepto "
        "LEFT JOIN tutores tp ON tp.id_familia=f.id_familia AND tp.es_principal=1 "
        "WHERE 1=1 ";
    if (mes > 0)       sql += "AND t.mes="  + std::to_string(mes)  + " ";
    if (anio > 0)      sql += "AND t.anio=" + std::to_string(anio) + " ";
    if (idConcepto > 0) sql += "AND t.id_concepto=" + std::to_string(idConcepto) + " ";
    sql += "ORDER BY pg.fecha_pago DESC";

    db->execSqlAsync(sql,
        [callback](const Result& r) {
            std::vector<Pago> lista;
            for (const auto& row : r) {
                Pago p;
                p.id_pago      = row["id_pago"].as<int>();
                p.id_talon     = row["id_talon"].as<int>();
                p.fecha_pago   = row["fecha_pago"].as<std::string>();
                p.monto        = row["monto"].as<double>();
                p.medio_pago   = row["medio_pago"].as<std::string>();
                p.observaciones= row["observaciones"].isNull() ? "" : row["observaciones"].as<std::string>();
                p.mes          = row["mes"].isNull() ? 0 : row["mes"].as<int>();
                p.anio         = row["anio"].isNull() ? 0 : row["anio"].as<int>();
                p.numero_familia          = row["numero_familia"].as<int>();
                p.codigo_barra            = row["codigo_barra"].as<std::string>();
                p.nombre_concepto         = row["nombre_concepto"].as<std::string>();
                p.nombre_tutor_principal  = row["nombre_tutor_principal"].isNull() ? "" : row["nombre_tutor_principal"].as<std::string>();
                lista.push_back(std::move(p));
            }
            callback(std::move(lista));
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); });
}

void PagoRepository::resumenMes(int anio,
    std::function<void(Json::Value)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "SELECT t.mes, t.anio, COUNT(pg.id_pago) AS cantidad, SUM(pg.monto) AS total "
        "FROM pagos pg JOIN talones t ON pg.id_talon=t.id_talon "
        "WHERE t.anio=? "
        "GROUP BY t.mes, t.anio ORDER BY t.mes",
        [callback](const Result& r) {
            Json::Value arr(Json::arrayValue);
            for (const auto& row : r) {
                Json::Value o;
                o["mes"]      = row["mes"].isNull() ? 0 : row["mes"].as<int>();
                o["anio"]     = row["anio"].as<int>();
                o["cantidad"] = row["cantidad"].as<int>();
                o["total"]    = row["total"].as<double>();
                arr.append(o);
            }
            callback(arr);
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        anio);
}

void PagoRepository::resumenMedioPago(int mes, int anio,
    std::function<void(Json::Value)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    std::string sql =
        "SELECT pg.medio_pago, COUNT(pg.id_pago) AS cantidad, SUM(pg.monto) AS total "
        "FROM pagos pg JOIN talones t ON pg.id_talon=t.id_talon "
        "WHERE 1=1 ";
    if (mes > 0)  sql += "AND t.mes="  + std::to_string(mes)  + " ";
    if (anio > 0) sql += "AND t.anio=" + std::to_string(anio) + " ";
    sql += "GROUP BY pg.medio_pago";

    db->execSqlAsync(sql,
        [callback](const Result& r) {
            Json::Value arr(Json::arrayValue);
            for (const auto& row : r) {
                Json::Value o;
                o["medio_pago"] = row["medio_pago"].as<std::string>();
                o["cantidad"]   = row["cantidad"].as<int>();
                o["total"]      = row["total"].as<double>();
                arr.append(o);
            }
            callback(arr);
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); });
}
