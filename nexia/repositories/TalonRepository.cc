#include "TalonRepository.h"
#include <sstream>
#include <iomanip>

using namespace drogon;
using namespace drogon::orm;

void TalonRepository::listar(
    int mes, int anio, int idCobradora,
    std::function<void(std::vector<Talon>)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    std::string sql =
        "SELECT t.*, "
        "s.nombre_completo as nombre_socio, s.direccion as direccion_socio, s.telefono as telefono_socio, s.dni as dni_socio, "
        "c.nombre as nombre_cobradora, "
        "l.nombre as localidad_socio, p.nombre as provincia_socio, "
        "anx.nombre_completo as nombre_responsable_socio, "
        "anx.nombre_completo as nombre_anexo, anx.dni as dni_anexo, "
        "anx.numero_socio as numero_socio_anexo, anx.telefono as telefono_anexo "
        "FROM talones t "
        "JOIN socios s ON t.id_socio = s.id_socio "
        "LEFT JOIN cobradoras c ON t.id_cobradora = c.id_cobradora "
        "LEFT JOIN localidades l ON s.id_localidad = l.id_localidad "
        "LEFT JOIN provincias p ON l.id_provincia = p.id_provincia "
        "LEFT JOIN socios anx ON anx.id_socio_principal = s.id_socio "
        "WHERE 1=1 ";
    if (mes > 0)       sql += "AND t.mes = "  + std::to_string(mes)  + " ";
    if (anio > 0)      sql += "AND t.anio = " + std::to_string(anio) + " ";
    if (idCobradora > 0) sql += "AND t.id_cobradora = " + std::to_string(idCobradora) + " ";
    sql += "ORDER BY s.nombre_completo";

    db->execSqlAsync(sql,
        [callback](const Result& r) {
            std::vector<Talon> lista;
            for (const auto& row : r) {
                Talon t;
                t.id_talon       = row["id_talon"].as<int>();
                t.id_socio       = row["id_socio"].as<int>();
                t.id_cobradora   = row["id_cobradora"].isNull() ? 0 : row["id_cobradora"].as<int>();
                t.mes            = row["mes"].as<int>();
                t.anio           = row["anio"].as<int>();
                t.monto          = row["monto"].as<double>();
                t.tipo           = row["tipo"].as<std::string>();
                t.codigo_barra   = row["codigo_barra"].as<std::string>();
                t.estado         = row["estado"].as<std::string>();
                t.fecha_generacion = row["fecha_generacion"].as<std::string>();
                t.fecha_pago     = row["fecha_pago"].isNull() ? "" : row["fecha_pago"].as<std::string>();
                t.nombre_socio            = row["nombre_socio"].as<std::string>();
                t.nombre_cobradora        = row["nombre_cobradora"].isNull() ? "Sin cobradora" : row["nombre_cobradora"].as<std::string>();
                t.direccion_socio         = row["direccion_socio"].isNull() ? "" : row["direccion_socio"].as<std::string>();
                t.telefono_socio          = row["telefono_socio"].isNull() ? "" : row["telefono_socio"].as<std::string>();
                t.dni_socio               = row["dni_socio"].isNull() ? "" : row["dni_socio"].as<std::string>();
                t.localidad_socio         = row["localidad_socio"].isNull() ? "" : row["localidad_socio"].as<std::string>();
                t.provincia_socio         = row["provincia_socio"].isNull() ? "" : row["provincia_socio"].as<std::string>();
                t.nombre_responsable_socio = row["nombre_responsable_socio"].isNull() ? "" : row["nombre_responsable_socio"].as<std::string>();
                t.nombre_anexo        = row["nombre_anexo"].isNull() ? "" : row["nombre_anexo"].as<std::string>();
                t.dni_anexo           = row["dni_anexo"].isNull() ? "" : row["dni_anexo"].as<std::string>();
                t.numero_socio_anexo  = row["numero_socio_anexo"].isNull() ? 0 : row["numero_socio_anexo"].as<int>();
                t.telefono_anexo      = row["telefono_anexo"].isNull() ? "" : row["telefono_anexo"].as<std::string>();
                lista.push_back(std::move(t));
            }
            callback(std::move(lista));
        },
        [errCallback](const DrogonDbException& e) {
            errCallback(e.base().what());
        });
}

void TalonRepository::buscarPorCodigo(
    const std::string& codigo,
    std::function<void(Talon)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "SELECT t.*, s.nombre_completo as nombre_socio, c.nombre as nombre_cobradora "
        "FROM talones t "
        "JOIN socios s ON t.id_socio = s.id_socio "
        "LEFT JOIN cobradoras c ON t.id_cobradora = c.id_cobradora "
        "WHERE t.codigo_barra = ?",
        [callback, errCallback](const Result& r) {
            if (r.empty()) { errCallback("Talón no encontrado"); return; }
            const auto& row = r[0];
            Talon t;
            t.id_talon       = row["id_talon"].as<int>();
            t.id_socio       = row["id_socio"].as<int>();
            t.id_cobradora   = row["id_cobradora"].isNull() ? 0 : row["id_cobradora"].as<int>();
            t.mes            = row["mes"].as<int>();
            t.anio           = row["anio"].as<int>();
            t.monto          = row["monto"].as<double>();
            t.tipo           = row["tipo"].as<std::string>();
            t.codigo_barra   = row["codigo_barra"].as<std::string>();
            t.estado         = row["estado"].as<std::string>();
            t.fecha_generacion = row["fecha_generacion"].as<std::string>();
            t.fecha_pago     = row["fecha_pago"].isNull() ? "" : row["fecha_pago"].as<std::string>();
            t.nombre_socio   = row["nombre_socio"].as<std::string>();
            t.nombre_cobradora = row["nombre_cobradora"].isNull() ? "Sin cobradora" : row["nombre_cobradora"].as<std::string>();
            callback(std::move(t));
        },
        [errCallback](const DrogonDbException& e) {
            errCallback(e.base().what());
        },
        codigo);
}

void TalonRepository::insertarBatch(
    const std::vector<Talon>& talones,
    std::function<void()> callback,
    std::function<void(const std::string&)> errCallback)
{
    if (talones.empty()) { callback(); return; }
    auto db = app().getDbClient("main");

    // INSERT multi-row en una sola query — sin callbacks recursivos, sin UB
    // Los valores vienen de nuestro propio código (no de input de usuario)
    std::string sql =
        "INSERT INTO talones "
        "(id_socio, id_cobradora, mes, anio, monto, tipo, codigo_barra) VALUES ";

    bool first = true;
    for (const auto& t : talones) {
        if (!first) sql += ",";
        first = false;
        std::ostringstream monto;
        monto << std::fixed << std::setprecision(2) << t.monto;
        std::string idCob = (t.id_cobradora > 0)
                            ? std::to_string(t.id_cobradora) : "NULL";
        // tipo: siempre 'SIMPLE' o 'CON_CARGO' (constante de código)
        // codigo_barra: solo dígitos (generado internamente)
        sql += "(" + std::to_string(t.id_socio) + ","
                   + idCob + ","
                   + std::to_string(t.mes) + ","
                   + std::to_string(t.anio) + ","
                   + monto.str() + ","
                   + "'" + t.tipo + "',"
                   + "'" + t.codigo_barra + "')";
    }

    db->execSqlAsync(sql,
        [callback](const Result&) { callback(); },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); });
}

void TalonRepository::marcarPagado(
    int idTalon,
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

void TalonRepository::anular(
    int idTalon,
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

void TalonRepository::existeMesAnio(
    int mes, int anio,
    std::function<void(bool)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "SELECT COUNT(*) as total FROM talones WHERE mes=? AND anio=? AND estado != 'ANULADO'",
        [callback](const Result& r) {
            callback(r[0]["total"].as<int>() > 0);
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        mes, anio);
}

void TalonRepository::eliminarAnuladosMesAnio(
    int mes, int anio,
    std::function<void()> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "DELETE FROM talones WHERE mes=? AND anio=? AND estado='ANULADO'",
        [callback](const Result&) { callback(); },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        mes, anio);
}

void TalonRepository::sociosConTalonActivo(
    int mes, int anio,
    const std::vector<int>& socioIds,
    std::function<void(std::vector<int>)> callback,
    std::function<void(const std::string&)> errCallback)
{
    if (socioIds.empty()) { callback({}); return; }
    auto db = app().getDbClient("main");
    // IDs son enteros de nuestro propio código — seguro armar la cláusula IN
    std::string inClause;
    for (size_t i = 0; i < socioIds.size(); ++i) {
        if (i) inClause += ",";
        inClause += std::to_string(socioIds[i]);
    }
    db->execSqlAsync(
        "SELECT id_socio FROM talones WHERE mes=" + std::to_string(mes) +
        " AND anio=" + std::to_string(anio) +
        " AND estado!='ANULADO' AND id_socio IN (" + inClause + ")",
        [callback](const Result& r) {
            std::vector<int> ids;
            for (const auto& row : r) ids.push_back(row["id_socio"].as<int>());
            callback(std::move(ids));
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); });
}

void TalonRepository::eliminarAnuladosDeSocios(
    int mes, int anio,
    const std::vector<int>& socioIds,
    std::function<void()> callback,
    std::function<void(const std::string&)> errCallback)
{
    if (socioIds.empty()) { callback(); return; }
    auto db = app().getDbClient("main");
    std::string inClause;
    for (size_t i = 0; i < socioIds.size(); ++i) {
        if (i) inClause += ",";
        inClause += std::to_string(socioIds[i]);
    }
    db->execSqlAsync(
        "DELETE FROM talones WHERE mes=" + std::to_string(mes) +
        " AND anio=" + std::to_string(anio) +
        " AND estado='ANULADO' AND id_socio IN (" + inClause + ")",
        [callback](const Result&) { callback(); },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); });
}

void TalonRepository::listarMorosos(
    std::function<void(Json::Value)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "SELECT s.id_socio, s.numero_socio, s.nombre_completo, s.direccion, "
        "c.nombre as cobradora, "
        "COUNT(t.id_talon) as meses_adeudados, "
        "SUM(t.monto) as deuda_total "
        "FROM talones t "
        "JOIN socios s ON t.id_socio = s.id_socio "
        "LEFT JOIN cobradoras c ON t.id_cobradora = c.id_cobradora "
        "WHERE t.estado = 'GENERADO' "
        "GROUP BY s.id_socio "
        "ORDER BY s.nombre_completo",
        [callback](const Result& r) {
            Json::Value arr(Json::arrayValue);
            for (const auto& row : r) {
                Json::Value obj;
                obj["id_socio"]       = row["id_socio"].as<int>();
                obj["numero_socio"]   = row["numero_socio"].as<int>();
                obj["nombre"]         = row["nombre_completo"].as<std::string>();
                obj["direccion"]      = row["direccion"].as<std::string>();
                obj["cobradora"]      = row["cobradora"].isNull() ? "Sin cobradora" : row["cobradora"].as<std::string>();
                obj["meses_adeudados"]= row["meses_adeudados"].as<int>();
                obj["deuda_total"]    = row["deuda_total"].as<double>();
                arr.append(obj);
            }
            callback(arr);
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); });
}
