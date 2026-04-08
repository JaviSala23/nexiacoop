#include "TalonRepository.h"
#include <sstream>
#include <iomanip>

using namespace drogon;
using namespace drogon::orm;

static Talon rowToTalon(const Row& row) {
    Talon t;
    t.id_talon       = row["id_talon"].as<int>();
    t.id_familia     = row["id_familia"].as<int>();
    t.id_concepto    = row["id_concepto"].as<int>();
    t.mes            = row["mes"].isNull() ? 0 : row["mes"].as<int>();
    t.anio           = row["anio"].isNull() ? 0 : row["anio"].as<int>();
    t.monto          = row["monto"].as<double>();
    t.codigo_barra   = row["codigo_barra"].as<std::string>();
    t.estado         = row["estado"].as<std::string>();
    t.fecha_generacion = row["fecha_generacion"].as<std::string>();
    t.fecha_pago     = row["fecha_pago"].isNull() ? "" : row["fecha_pago"].as<std::string>();
    t.observaciones  = row["observaciones"].isNull() ? "" : row["observaciones"].as<std::string>();
    t.numero_familia           = row["numero_familia"].as<int>();
    t.nombre_concepto          = row["nombre_concepto"].isNull() ? "" : row["nombre_concepto"].as<std::string>();
    t.tipo_concepto            = row["tipo_concepto"].isNull() ? "" : row["tipo_concepto"].as<std::string>();
    t.nombre_tutor_principal   = row["nombre_tutor_principal"].isNull() ? "" : row["nombre_tutor_principal"].as<std::string>();
    t.dni_tutor_principal      = row["dni_tutor_principal"].isNull() ? "" : row["dni_tutor_principal"].as<std::string>();
    t.telefono_familia         = row["telefono_familia"].isNull() ? "" : row["telefono_familia"].as<std::string>();
    t.direccion_familia        = row["direccion_familia"].isNull() ? "" : row["direccion_familia"].as<std::string>();
    t.cantidad_alumnos         = row["cantidad_alumnos"].as<int>();
    return t;
}

void TalonRepository::listar(int mes, int anio, int idConcepto, bool soloExtras,
    std::function<void(std::vector<Talon>)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    std::string sql =
        "SELECT t.*, "
        "f.numero_familia, f.telefono AS telefono_familia, f.direccion AS direccion_familia, "
        "tp.nombre_completo AS nombre_tutor_principal, tp.dni AS dni_tutor_principal, "
        "c.nombre AS nombre_concepto, c.tipo AS tipo_concepto, "
        "(SELECT COUNT(*) FROM alumnos a WHERE a.id_familia=f.id_familia AND a.estado='ACTIVO') AS cantidad_alumnos "
        "FROM talones t "
        "JOIN familias f ON t.id_familia=f.id_familia "
        "JOIN conceptos c ON t.id_concepto=c.id_concepto "
        "LEFT JOIN tutores tp ON tp.id_familia=f.id_familia AND tp.es_principal=1 "
        "WHERE 1=1 ";
    if (soloExtras) {
        sql += "AND (t.mes IS NULL OR t.mes=0) AND (t.anio IS NULL OR t.anio=0) ";
    } else {
        if (mes > 0)       sql += "AND t.mes="  + std::to_string(mes)  + " ";
        if (anio > 0)      sql += "AND t.anio=" + std::to_string(anio) + " ";
    }
    if (idConcepto > 0) sql += "AND t.id_concepto=" + std::to_string(idConcepto) + " ";
    sql += "ORDER BY f.numero_familia";

    db->execSqlAsync(sql,
        [callback](const Result& r) {
            std::vector<Talon> lista;
            for (const auto& row : r) lista.push_back(rowToTalon(row));
            callback(std::move(lista));
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); });
}

void TalonRepository::buscarPorCodigo(const std::string& codigo,
    std::function<void(Talon)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "SELECT t.*, "
        "f.numero_familia, f.telefono AS telefono_familia, f.direccion AS direccion_familia, "
        "tp.nombre_completo AS nombre_tutor_principal, tp.dni AS dni_tutor_principal, "
        "c.nombre AS nombre_concepto, c.tipo AS tipo_concepto, "
        "(SELECT COUNT(*) FROM alumnos a WHERE a.id_familia=f.id_familia AND a.estado='ACTIVO') AS cantidad_alumnos "
        "FROM talones t "
        "JOIN familias f ON t.id_familia=f.id_familia "
        "JOIN conceptos c ON t.id_concepto=c.id_concepto "
        "LEFT JOIN tutores tp ON tp.id_familia=f.id_familia AND tp.es_principal=1 "
        "WHERE t.codigo_barra=?",
        [callback, errCallback](const Result& r) {
            if (r.empty()) { errCallback("Talón no encontrado"); return; }
            callback(rowToTalon(r[0]));
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        codigo);
}

void TalonRepository::insertar(const Talon& t,
    std::function<void(int)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "INSERT INTO talones (id_familia, id_concepto, mes, anio, monto, codigo_barra, observaciones) "
        "VALUES (?,?,NULLIF(?,0),NULLIF(?,0),?,?,?)",
        [callback](const Result& r) { callback(static_cast<int>(r.insertId())); },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        t.id_familia, t.id_concepto, t.mes, t.anio, t.monto, t.codigo_barra, t.observaciones);
}

void TalonRepository::insertarBatch(const std::vector<Talon>& talones,
    std::function<void()> callback,
    std::function<void(const std::string&)> errCallback)
{
    if (talones.empty()) { callback(); return; }
    auto db = app().getDbClient("main");

    // Todos los valores son enteros y doubles generados internamente - seguro armar SQL
    std::string sql =
        "INSERT INTO talones (id_familia, id_concepto, mes, anio, monto, codigo_barra) VALUES ";
    bool first = true;
    for (const auto& t : talones) {
        if (!first) sql += ",";
        first = false;
        std::ostringstream monto;
        monto << std::fixed << std::setprecision(2) << t.monto;
        sql += "(" + std::to_string(t.id_familia) + ","
                   + std::to_string(t.id_concepto) + ","
                   + std::to_string(t.mes) + ","
                   + std::to_string(t.anio) + ","
                   + monto.str() + ","
                   + "'" + t.codigo_barra + "')";
    }
    db->execSqlAsync(sql,
        [callback](const Result&) { callback(); },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); });
}

void TalonRepository::marcarPagado(int idTalon,
    std::function<void()> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "UPDATE talones SET estado='PAGADO', fecha_pago=NOW() WHERE id_talon=?",
        [callback](const Result&) { callback(); },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        idTalon);
}

void TalonRepository::anular(int idTalon,
    std::function<void()> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "UPDATE talones SET estado='ANULADO' WHERE id_talon=?",
        [callback](const Result&) { callback(); },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        idTalon);
}

void TalonRepository::existeMesAnioConcepto(int mes, int anio, int idConcepto,
    std::function<void(bool)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "SELECT COUNT(*) AS total FROM talones "
        "WHERE mes=? AND anio=? AND id_concepto=? AND estado!='ANULADO'",
        [callback](const Result& r) { callback(r[0]["total"].as<int>() > 0); },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        mes, anio, idConcepto);
}

void TalonRepository::eliminarAnuladosMesAnioConcepto(int mes, int anio, int idConcepto,
    std::function<void()> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "DELETE FROM talones WHERE mes=? AND anio=? AND id_concepto=? AND estado='ANULADO'",
        [callback](const Result&) { callback(); },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        mes, anio, idConcepto);
}

void TalonRepository::familiasConTalonActivo(int mes, int anio, int idConcepto,
    const std::vector<int>& familiaIds,
    std::function<void(std::vector<int>)> callback,
    std::function<void(const std::string&)> errCallback)
{
    if (familiaIds.empty()) { callback({}); return; }
    auto db = app().getDbClient("main");
    std::string inClause;
    for (size_t i = 0; i < familiaIds.size(); ++i) {
        if (i) inClause += ",";
        inClause += std::to_string(familiaIds[i]);
    }
    db->execSqlAsync(
        "SELECT id_familia FROM talones WHERE mes=" + std::to_string(mes) +
        " AND anio=" + std::to_string(anio) +
        " AND id_concepto=" + std::to_string(idConcepto) +
        " AND estado!='ANULADO' AND id_familia IN (" + inClause + ")",
        [callback](const Result& r) {
            std::vector<int> ids;
            for (const auto& row : r) ids.push_back(row["id_familia"].as<int>());
            callback(std::move(ids));
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); });
}

void TalonRepository::listarMorosos(
    std::function<void(Json::Value)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "SELECT f.id_familia, f.numero_familia, "
        "tp.nombre_completo AS nombre_familia, "
        "f.direccion, f.telefono, "
        "COUNT(t.id_talon) AS talones_adeudados, "
        "SUM(t.monto) AS deuda_total, "
        "MIN(CONCAT(LPAD(t.anio,4,'0'), LPAD(t.mes,2,'0'))) AS periodo_mas_antiguo "
        "FROM talones t "
        "JOIN familias f ON t.id_familia=f.id_familia "
        "LEFT JOIN tutores tp ON tp.id_familia=f.id_familia AND tp.es_principal=1 "
        "WHERE t.estado='GENERADO' "
        "AND t.mes > 0 "
        "AND (t.anio < YEAR(CURDATE()) OR (t.anio = YEAR(CURDATE()) AND t.mes < MONTH(CURDATE()))) "
        "GROUP BY f.id_familia "
        "ORDER BY deuda_total DESC",
        [callback](const Result& r) {
            Json::Value arr(Json::arrayValue);
            for (const auto& row : r) {
                Json::Value obj;
                obj["id_familia"]       = row["id_familia"].as<int>();
                obj["numero_familia"]   = row["numero_familia"].as<int>();
                obj["nombre"]           = row["nombre_familia"].isNull() ? "" : row["nombre_familia"].as<std::string>();
                obj["direccion"]        = row["direccion"].isNull() ? "" : row["direccion"].as<std::string>();
                obj["telefono"]         = row["telefono"].isNull() ? "" : row["telefono"].as<std::string>();
                obj["talones_adeudados"]= row["talones_adeudados"].as<int>();
                obj["deuda_total"]      = row["deuda_total"].as<double>();
                obj["periodo_mas_antiguo"] = row["periodo_mas_antiguo"].isNull() ? "" : row["periodo_mas_antiguo"].as<std::string>();
                arr.append(obj);
            }
            callback(arr);
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); });
}

void TalonRepository::actualizarMonto(int idTalon, double monto,
    std::function<void()> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = drogon::app().getDbClient("main");
    db->execSqlAsync(
        "UPDATE talones SET monto=? WHERE id_talon=? AND estado='GENERADO'",
        [callback](const drogon::orm::Result&) { callback(); },
        [errCallback](const drogon::orm::DrogonDbException& e) { errCallback(e.base().what()); },
        monto, idTalon);
}
