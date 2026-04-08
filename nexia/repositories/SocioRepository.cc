#include "SocioRepository.h"

using namespace drogon;
using namespace drogon::orm;

void SocioRepository::existeNumeroODni(int numero, const std::string& dni, int excluirId,
    std::function<void(bool, std::string)> callback) {
    auto db = app().getDbClient("main");
    std::string sql = "SELECT numero_socio, dni FROM socios WHERE (numero_socio=? OR dni=?)";
    if (excluirId > 0) sql += " AND id_socio<>" + std::to_string(excluirId);
    db->execSqlAsync(sql,
        [numero, dni, callback](const Result& r) {
            for (const auto& row : r) {
                if (row["numero_socio"].as<int>() == numero)
                    { callback(true, "El número de socio ya existe"); return; }
                if (row["dni"].as<std::string>() == dni)
                    { callback(true, "El DNI ya existe"); return; }
            }
            callback(false, "");
        },
        [callback](const DrogonDbException& e) {
            callback(true, "Error de validación: " + std::string(e.base().what()));
        },
        numero, dni);
}

void SocioRepository::listar(
    const std::string& estado,
    int idCobradora,
    std::function<void(std::vector<Socio>)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    std::string sql =
        "SELECT s.*, c.nombre as nombre_cobradora, "
        "l.nombre as nombre_localidad, p.nombre as nombre_provincia "
        "FROM socios s "
        "LEFT JOIN cobradoras c ON s.id_cobradora = c.id_cobradora "
        "LEFT JOIN localidades l ON s.id_localidad = l.id_localidad "
        "LEFT JOIN provincias p ON l.id_provincia = p.id_provincia "
        "WHERE 1=1 ";
    if (!estado.empty() && estado != "TODOS") sql += "AND s.estado = '" + estado + "' ";
    if (idCobradora > 0) sql += "AND s.id_cobradora = " + std::to_string(idCobradora) + " ";
    sql += "ORDER BY s.nombre_completo";

    db->execSqlAsync(sql,
        [callback](const Result& r) {
            std::vector<Socio> lista;
            for (const auto& row : r) {
                Socio s;
                s.id_socio           = row["id_socio"].as<int>();
                s.numero_socio       = row["numero_socio"].as<int>();
                s.nombre_completo    = row["nombre_completo"].as<std::string>();
                s.dni                = row["dni"].as<std::string>();
                s.direccion          = row["direccion"].as<std::string>();
                s.telefono           = row["telefono"].isNull() ? "" : row["telefono"].as<std::string>();
                s.id_localidad       = row["id_localidad"].isNull() ? 0 : row["id_localidad"].as<int>();
                s.id_cobradora       = row["id_cobradora"].isNull() ? 0 : row["id_cobradora"].as<int>();
                s.estado             = row["estado"].as<std::string>();
                s.fecha_alta         = row["fecha_alta"].as<std::string>();
                s.fecha_baja         = row["fecha_baja"].isNull() ? "" : row["fecha_baja"].as<std::string>();
                s.tiene_responsable  = row["tiene_responsable"].as<bool>();
                s.id_socio_principal  = row["id_socio_principal"].isNull() ? 0 : row["id_socio_principal"].as<int>();
                s.nombre_cobradora   = row["nombre_cobradora"].isNull() ? "" : row["nombre_cobradora"].as<std::string>();
                // No se usa más nombre_responsable
                s.nombre_localidad   = row["nombre_localidad"].isNull() ? "" : row["nombre_localidad"].as<std::string>();
                s.nombre_provincia   = row["nombre_provincia"].isNull() ? "" : row["nombre_provincia"].as<std::string>();
                lista.push_back(std::move(s));
            }
            callback(std::move(lista));
        },
        [errCallback](const DrogonDbException& e) {
            errCallback(e.base().what());
        });
}

void SocioRepository::buscarPorId(
    int id,
    std::function<void(Socio)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "SELECT s.*, c.nombre as nombre_cobradora, "
        "l.nombre as nombre_localidad, p.nombre as nombre_provincia "
        "FROM socios s "
        "LEFT JOIN cobradoras c ON s.id_cobradora = c.id_cobradora "
        "LEFT JOIN localidades l ON s.id_localidad = l.id_localidad "
        "LEFT JOIN provincias p ON l.id_provincia = p.id_provincia "
        "WHERE s.id_socio = ?",
        [callback, errCallback](const Result& r) {
            if (r.empty()) { errCallback("Socio no encontrado"); return; }
            const auto& row = r[0];
            Socio s;
            s.id_socio           = row["id_socio"].as<int>();
            s.numero_socio       = row["numero_socio"].as<int>();
            s.nombre_completo    = row["nombre_completo"].as<std::string>();
            s.dni                = row["dni"].as<std::string>();
            s.direccion          = row["direccion"].as<std::string>();
            s.telefono           = row["telefono"].isNull() ? "" : row["telefono"].as<std::string>();
            s.id_localidad       = row["id_localidad"].isNull() ? 0 : row["id_localidad"].as<int>();
            s.id_cobradora       = row["id_cobradora"].isNull() ? 0 : row["id_cobradora"].as<int>();
            s.estado             = row["estado"].as<std::string>();
            s.fecha_alta         = row["fecha_alta"].as<std::string>();
            s.fecha_baja         = row["fecha_baja"].isNull() ? "" : row["fecha_baja"].as<std::string>();
            s.tiene_responsable  = row["tiene_responsable"].as<bool>();
            s.id_socio_principal  = row["id_socio_principal"].isNull() ? 0 : row["id_socio_principal"].as<int>();
            s.nombre_cobradora   = row["nombre_cobradora"].isNull() ? "" : row["nombre_cobradora"].as<std::string>();
            s.nombre_localidad   = row["nombre_localidad"].isNull() ? "" : row["nombre_localidad"].as<std::string>();
            s.nombre_provincia   = row["nombre_provincia"].isNull() ? "" : row["nombre_provincia"].as<std::string>();
            callback(std::move(s));
        },
        [errCallback](const DrogonDbException& e) {
            errCallback(e.base().what());
        },
        id);
}

void SocioRepository::insertar(
    const Socio& s,
    std::function<void(int)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "INSERT INTO socios (numero_socio, nombre_completo, dni, direccion, telefono, "
        "id_localidad, id_cobradora, estado, fecha_alta, tiene_responsable, id_socio_principal) "
        "VALUES (?,?,?,?,?,NULLIF(?,0),NULLIF(?,0),?,?,?,NULLIF(?,0))",
        [callback](const Result& r) {
            callback(static_cast<int>(r.insertId()));
        },
        [errCallback](const DrogonDbException& e) {
            errCallback(e.base().what());
        },
        s.numero_socio, s.nombre_completo, s.dni, s.direccion, s.telefono,
        s.id_localidad,
        s.id_cobradora,
        s.estado, s.fecha_alta, s.tiene_responsable ? 1 : 0,
        s.id_socio_principal);
}

void SocioRepository::actualizar(
    const Socio& s,
    std::function<void()> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "UPDATE socios SET nombre_completo=?, dni=?, direccion=?, telefono=?, "
        "id_localidad=NULLIF(?,0), id_cobradora=NULLIF(?,0), estado=?, tiene_responsable=? "
        "WHERE id_socio=?",
        [callback](const Result&) { callback(); },
        [errCallback](const DrogonDbException& e) {
            errCallback(e.base().what());
        },
        s.nombre_completo, s.dni, s.direccion, s.telefono,
        s.id_localidad, s.id_cobradora,
        s.estado,
        s.tiene_responsable ? 1 : 0,
        s.id_socio);
}

void SocioRepository::cambiarEstado(
    int id,
    const std::string& estado,
    std::function<void()> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    auto sql = estado == "BAJA"
        ? "UPDATE socios SET estado=?, fecha_baja=CURDATE() WHERE id_socio=?"
        : "UPDATE socios SET estado=? WHERE id_socio=?";
    db->execSqlAsync(
        sql,
        [callback](const Result&) { callback(); },
        [errCallback](const DrogonDbException& e) {
            errCallback(e.base().what());
        },
        estado, id);
}

