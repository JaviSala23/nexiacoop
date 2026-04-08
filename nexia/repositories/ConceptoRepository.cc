#include "ConceptoRepository.h"
#include <drogon/drogon.h>

using namespace drogon;
using namespace drogon::orm;

static Concepto rowToConcepto(const Row& row) {
    Concepto c;
    c.id_concepto  = row["id_concepto"].as<int>();
    c.nombre       = row["nombre"].as<std::string>();
    c.descripcion  = row["descripcion"].isNull() ? "" : row["descripcion"].as<std::string>();
    c.tipo         = row["tipo"].as<std::string>();
    c.activo       = row["activo"].as<bool>();
    return c;
}

void ConceptoRepository::listar(
    std::function<void(std::vector<Concepto>)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync("SELECT * FROM conceptos ORDER BY tipo, nombre",
        [callback](const Result& r) {
            std::vector<Concepto> lista;
            for (const auto& row : r) lista.push_back(rowToConcepto(row));
            callback(std::move(lista));
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); });
}

void ConceptoRepository::buscarPorId(int id,
    std::function<void(Concepto)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync("SELECT * FROM conceptos WHERE id_concepto=?",
        [callback, errCallback](const Result& r) {
            if (r.empty()) { errCallback("Concepto no encontrado"); return; }
            callback(rowToConcepto(r[0]));
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        id);
}

void ConceptoRepository::buscarMensual(
    std::function<void(Concepto)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync("SELECT * FROM conceptos WHERE tipo='MENSUAL' AND activo=1 LIMIT 1",
        [callback, errCallback](const Result& r) {
            if (r.empty()) { errCallback("No hay concepto MENSUAL configurado"); return; }
            callback(rowToConcepto(r[0]));
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); });
}

void ConceptoRepository::insertar(const Concepto& c,
    std::function<void(int)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "INSERT INTO conceptos (nombre, descripcion, tipo, activo) VALUES (?,?,?,1)",
        [callback](const Result& r) { callback(static_cast<int>(r.insertId())); },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        c.nombre, c.descripcion, c.tipo);
}

void ConceptoRepository::actualizar(const Concepto& c,
    std::function<void()> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "UPDATE conceptos SET nombre=?, descripcion=?, tipo=? WHERE id_concepto=?",
        [callback](const Result&) { callback(); },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        c.nombre, c.descripcion, c.tipo, c.id_concepto);
}

void ConceptoRepository::toggleActivo(int id,
    std::function<void()> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "UPDATE conceptos SET activo = NOT activo WHERE id_concepto=?",
        [callback](const Result&) { callback(); },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        id);
}

// -------------------------------------------------------
// Cuotas Config
// -------------------------------------------------------
void ConceptoRepository::listarCuotasConfig(
    std::function<void(std::vector<CuotaConfig>)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync("SELECT * FROM cuotas_config ORDER BY cantidad_hijos",
        [callback](const Result& r) {
            std::vector<CuotaConfig> lista;
            for (const auto& row : r) {
                CuotaConfig cc;
                cc.id_config      = row["id_config"].as<int>();
                cc.cantidad_hijos = row["cantidad_hijos"].as<int>();
                cc.monto          = row["monto"].as<double>();
                cc.activo         = row["activo"].as<bool>();
                lista.push_back(std::move(cc));
            }
            callback(std::move(lista));
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); });
}

void ConceptoRepository::obtenerMontoPorHijos(int cantHijos,
    std::function<void(double)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    // Busca configuración exacta. Si no existe, usa la más alta disponible
    db->execSqlAsync(
        "SELECT monto FROM cuotas_config WHERE activo=1 AND cantidad_hijos <= ? "
        "ORDER BY cantidad_hijos DESC LIMIT 1",
        [callback, errCallback](const Result& r) {
            if (r.empty()) { errCallback("No hay configuración de cuotas definida"); return; }
            callback(r[0]["monto"].as<double>());
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        cantHijos);
}

void ConceptoRepository::upsertCuotaConfig(int cantHijos, double monto,
    std::function<void()> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "INSERT INTO cuotas_config (cantidad_hijos, monto, activo) VALUES (?,?,1) "
        "ON DUPLICATE KEY UPDATE monto=?, activo=1",
        [callback](const Result&) { callback(); },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        cantHijos, monto, monto);
}

void ConceptoRepository::eliminarCuotaConfig(int idConfig,
    std::function<void()> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "DELETE FROM cuotas_config WHERE id_config=?",
        [callback](const Result&) { callback(); },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        idConfig);
}
