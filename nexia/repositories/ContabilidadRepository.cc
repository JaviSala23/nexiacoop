#include "ContabilidadRepository.h"
#include <drogon/drogon.h>

using namespace drogon;
using namespace drogon::orm;

// -------------------------------------------------------
// Plan de cuentas
// -------------------------------------------------------
void ContabilidadRepository::listarCuentas(
    std::function<void(std::vector<PlanCuenta>)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync("SELECT * FROM plan_cuentas ORDER BY tipo, codigo",
        [callback](const Result& r) {
            std::vector<PlanCuenta> lista;
            for (const auto& row : r) {
                PlanCuenta c;
                c.id_cuenta = row["id_cuenta"].as<int>();
                c.codigo    = row["codigo"].as<std::string>();
                c.nombre    = row["nombre"].as<std::string>();
                c.tipo      = row["tipo"].as<std::string>();
                c.activo    = row["activo"].as<bool>();
                lista.push_back(std::move(c));
            }
            callback(std::move(lista));
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); });
}

void ContabilidadRepository::insertarCuenta(const PlanCuenta& c,
    std::function<void(int)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "INSERT INTO plan_cuentas (codigo, nombre, tipo, activo) VALUES (?,?,?,1)",
        [callback](const Result& r) { callback(static_cast<int>(r.insertId())); },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        c.codigo, c.nombre, c.tipo);
}

void ContabilidadRepository::actualizarCuenta(const PlanCuenta& c,
    std::function<void()> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "UPDATE plan_cuentas SET codigo=?, nombre=?, tipo=? WHERE id_cuenta=?",
        [callback](const Result&) { callback(); },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        c.codigo, c.nombre, c.tipo, c.id_cuenta);
}

void ContabilidadRepository::toggleActivoCuenta(int id,
    std::function<void()> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync("UPDATE plan_cuentas SET activo = NOT activo WHERE id_cuenta=?",
        [callback](const Result&) { callback(); },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        id);
}

// -------------------------------------------------------
// Movimientos
// -------------------------------------------------------
void ContabilidadRepository::listarMovimientos(const std::string& tipo,
    const std::string& desde, const std::string& hasta,
    std::function<void(std::vector<Movimiento>)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    std::string sql =
        "SELECT m.*, pc.nombre AS nombre_cuenta, pc.codigo AS codigo_cuenta "
        "FROM movimientos m "
        "JOIN plan_cuentas pc ON m.id_cuenta = pc.id_cuenta "
        "WHERE 1=1 ";
    if (!tipo.empty())  sql += "AND m.tipo='" + tipo + "' ";
    if (!desde.empty()) sql += "AND m.fecha >= '" + desde + "' ";
    if (!hasta.empty()) sql += "AND m.fecha <= '" + hasta + "' ";
    sql += "ORDER BY m.fecha DESC, m.id_movimiento DESC";

    db->execSqlAsync(sql,
        [callback](const Result& r) {
            std::vector<Movimiento> lista;
            for (const auto& row : r) {
                Movimiento m;
                m.id_movimiento = row["id_movimiento"].as<int>();
                m.id_cuenta     = row["id_cuenta"].as<int>();
                m.fecha         = row["fecha"].as<std::string>();
                m.descripcion   = row["descripcion"].isNull() ? "" : row["descripcion"].as<std::string>();
                m.monto         = row["monto"].as<double>();
                m.tipo          = row["tipo"].as<std::string>();
                m.medio_pago    = row["medio_pago"].as<std::string>();
                m.id_pago       = row["id_pago"].isNull() ? 0 : row["id_pago"].as<int>();
                m.nombre_cuenta = row["nombre_cuenta"].as<std::string>();
                m.codigo_cuenta = row["codigo_cuenta"].as<std::string>();
                lista.push_back(std::move(m));
            }
            callback(std::move(lista));
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); });
}

void ContabilidadRepository::insertarMovimiento(const Movimiento& m,
    std::function<void(int)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "INSERT INTO movimientos (id_cuenta, fecha, descripcion, monto, tipo, medio_pago, id_pago) "
        "VALUES (?,?,?,?,?,?,NULLIF(?,0))",
        [callback](const Result& r) { callback(static_cast<int>(r.insertId())); },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        m.id_cuenta, m.fecha, m.descripcion, m.monto, m.tipo, m.medio_pago, m.id_pago);
}

void ContabilidadRepository::eliminarMovimiento(int id,
    std::function<void()> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    // Solo se pueden eliminar movimientos manuales (sin id_pago vinculado)
    db->execSqlAsync(
        "DELETE FROM movimientos WHERE id_movimiento=? AND id_pago IS NULL",
        [callback](const Result&) { callback(); },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        id);
}

void ContabilidadRepository::resumenPorCuenta(int anio,
    std::function<void(Json::Value)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    std::string sql =
        "SELECT pc.codigo, pc.nombre, pc.tipo, SUM(m.monto) AS total "
        "FROM movimientos m "
        "JOIN plan_cuentas pc ON m.id_cuenta = pc.id_cuenta "
        "WHERE 1=1 ";
    if (anio > 0) sql += "AND YEAR(m.fecha)=" + std::to_string(anio) + " ";
    sql += "GROUP BY pc.id_cuenta ORDER BY pc.tipo, pc.codigo";

    db->execSqlAsync(sql,
        [callback](const Result& r) {
            Json::Value arr(Json::arrayValue);
            for (const auto& row : r) {
                Json::Value o;
                o["codigo"] = row["codigo"].as<std::string>();
                o["nombre"] = row["nombre"].as<std::string>();
                o["tipo"]   = row["tipo"].as<std::string>();
                o["total"]  = row["total"].as<double>();
                arr.append(o);
            }
            callback(arr);
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); });
}

void ContabilidadRepository::resumenMensual(int anio,
    std::function<void(Json::Value)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    std::string sql =
        "SELECT MONTH(m.fecha) AS mes, m.tipo, SUM(m.monto) AS total "
        "FROM movimientos m WHERE YEAR(m.fecha)=? "
        "GROUP BY MONTH(m.fecha), m.tipo "
        "ORDER BY mes";

    db->execSqlAsync(sql,
        [callback](const Result& r) {
            Json::Value arr(Json::arrayValue);
            for (const auto& row : r) {
                Json::Value o;
                o["mes"]   = row["mes"].as<int>();
                o["tipo"]  = row["tipo"].as<std::string>();
                o["total"] = row["total"].as<double>();
                arr.append(o);
            }
            callback(arr);
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        anio);
}
