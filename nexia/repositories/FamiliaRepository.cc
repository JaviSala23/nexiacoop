#include "FamiliaRepository.h"
#include <drogon/drogon.h>

using namespace drogon;
using namespace drogon::orm;

// -------------------------------------------------------
// Validación
// -------------------------------------------------------
void FamiliaRepository::existeNumeroODni(int numero, const std::string& dni, int excluirId,
    std::function<void(bool, std::string)> callback)
{
    auto db = app().getDbClient("main");
    // Verificar número de familia
    std::string sqlNum = "SELECT id_familia FROM familias WHERE numero_familia=?";
    if (excluirId > 0) sqlNum += " AND id_familia<>" + std::to_string(excluirId);
    db->execSqlAsync(sqlNum,
        [db, dni, excluirId, callback](const Result& r) {
            if (!r.empty()) { callback(true, "El número de familia ya existe"); return; }
            // Verificar DNI en tutores
            std::string sqlDni = "SELECT t.id_tutor FROM tutores t"
                " JOIN familias f ON t.id_familia = f.id_familia"
                " WHERE t.dni=?";
            if (excluirId > 0) sqlDni += " AND f.id_familia<>" + std::to_string(excluirId);
            db->execSqlAsync(sqlDni,
                [callback](const Result& r2) {
                    if (!r2.empty()) { callback(true, "El DNI ya está registrado"); return; }
                    callback(false, "");
                },
                [callback](const DrogonDbException& e) {
                    callback(true, "Error de validación: " + std::string(e.base().what()));
                },
                dni);
        },
        [callback](const DrogonDbException& e) {
            callback(true, "Error de validación: " + std::string(e.base().what()));
        },
        numero);
}

// -------------------------------------------------------
// Listar familias con tutor principal y conteo de alumnos
// -------------------------------------------------------
void FamiliaRepository::listar(const std::string& estado,
    std::function<void(std::vector<Familia>)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    std::string sql =
        "SELECT f.*, "
        "tp.nombre_completo AS nombre_tutor_principal, "
        "tp.dni AS dni_tutor_principal, "
        "tp.telefono AS telefono_tutor_principal, "
        "(SELECT COUNT(*) FROM alumnos a WHERE a.id_familia=f.id_familia AND a.estado='ACTIVO') AS cantidad_alumnos "
        "FROM familias f "
        "LEFT JOIN tutores tp ON tp.id_familia=f.id_familia AND tp.es_principal=1 "
        "WHERE 1=1 ";
    if (!estado.empty() && estado != "TODOS") sql += "AND f.estado='" + estado + "' ";
    sql += "ORDER BY f.numero_familia";

    db->execSqlAsync(sql,
        [callback](const Result& r) {
            std::vector<Familia> lista;
            for (const auto& row : r) {
                Familia f;
                f.id_familia     = row["id_familia"].as<int>();
                f.numero_familia = row["numero_familia"].as<int>();
                f.direccion      = row["direccion"].isNull() ? "" : row["direccion"].as<std::string>();
                f.telefono       = row["telefono"].isNull() ? "" : row["telefono"].as<std::string>();
                f.estado         = row["estado"].as<std::string>();
                f.fecha_alta     = row["fecha_alta"].as<std::string>();
                f.fecha_baja     = row["fecha_baja"].isNull() ? "" : row["fecha_baja"].as<std::string>();
                f.observaciones  = row["observaciones"].isNull() ? "" : row["observaciones"].as<std::string>();
                f.cantidad_alumnos = row["cantidad_alumnos"].as<int>();
                f.nombre_tutor_principal   = row["nombre_tutor_principal"].isNull() ? "" : row["nombre_tutor_principal"].as<std::string>();
                f.dni_tutor_principal      = row["dni_tutor_principal"].isNull() ? "" : row["dni_tutor_principal"].as<std::string>();
                f.telefono_tutor_principal = row["telefono_tutor_principal"].isNull() ? "" : row["telefono_tutor_principal"].as<std::string>();
                lista.push_back(std::move(f));
            }
            callback(std::move(lista));
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); });
}

void FamiliaRepository::buscarPorId(int id,
    std::function<void(Familia)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "SELECT f.*, "
        "tp.nombre_completo AS nombre_tutor_principal, "
        "tp.dni AS dni_tutor_principal, "
        "tp.telefono AS telefono_tutor_principal, "
        "(SELECT COUNT(*) FROM alumnos a WHERE a.id_familia=f.id_familia AND a.estado='ACTIVO') AS cantidad_alumnos "
        "FROM familias f "
        "LEFT JOIN tutores tp ON tp.id_familia=f.id_familia AND tp.es_principal=1 "
        "WHERE f.id_familia=?",
        [callback, errCallback](const Result& r) {
            if (r.empty()) { errCallback("Familia no encontrada"); return; }
            const auto& row = r[0];
            Familia f;
            f.id_familia     = row["id_familia"].as<int>();
            f.numero_familia = row["numero_familia"].as<int>();
            f.direccion      = row["direccion"].isNull() ? "" : row["direccion"].as<std::string>();
            f.telefono       = row["telefono"].isNull() ? "" : row["telefono"].as<std::string>();
            f.estado         = row["estado"].as<std::string>();
            f.fecha_alta     = row["fecha_alta"].as<std::string>();
            f.fecha_baja     = row["fecha_baja"].isNull() ? "" : row["fecha_baja"].as<std::string>();
            f.observaciones  = row["observaciones"].isNull() ? "" : row["observaciones"].as<std::string>();
            f.cantidad_alumnos = row["cantidad_alumnos"].as<int>();
            f.nombre_tutor_principal   = row["nombre_tutor_principal"].isNull() ? "" : row["nombre_tutor_principal"].as<std::string>();
            f.dni_tutor_principal      = row["dni_tutor_principal"].isNull() ? "" : row["dni_tutor_principal"].as<std::string>();
            f.telefono_tutor_principal = row["telefono_tutor_principal"].isNull() ? "" : row["telefono_tutor_principal"].as<std::string>();
            callback(std::move(f));
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        id);
}

void FamiliaRepository::insertar(const Familia& f,
    std::function<void(int)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "INSERT INTO familias (numero_familia, direccion, telefono, estado, fecha_alta, observaciones) "
        "VALUES (?,?,?,?,?,?)",
        [callback](const Result& r) { callback(static_cast<int>(r.insertId())); },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        f.numero_familia, f.direccion, f.telefono, f.estado, f.fecha_alta, f.observaciones);
}

void FamiliaRepository::actualizar(const Familia& f,
    std::function<void()> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "UPDATE familias SET direccion=?, telefono=?, estado=?, observaciones=? "
        "WHERE id_familia=?",
        [callback](const Result&) { callback(); },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        f.direccion, f.telefono, f.estado, f.observaciones, f.id_familia);
}

void FamiliaRepository::cambiarEstado(int id, const std::string& estado,
    std::function<void()> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    const char* sql = (estado == "BAJA")
        ? "UPDATE familias SET estado=?, fecha_baja=CURDATE() WHERE id_familia=?"
        : "UPDATE familias SET estado=? WHERE id_familia=?";
    db->execSqlAsync(sql,
        [callback](const Result&) { callback(); },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        estado, id);
}

// -------------------------------------------------------
// Tutores
// -------------------------------------------------------
void FamiliaRepository::listarTutores(int idFamilia,
    std::function<void(std::vector<Tutor>)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "SELECT * FROM tutores WHERE id_familia=? ORDER BY es_principal DESC",
        [callback](const Result& r) {
            std::vector<Tutor> lista;
            for (const auto& row : r) {
                Tutor t;
                t.id_tutor        = row["id_tutor"].as<int>();
                t.id_familia      = row["id_familia"].as<int>();
                t.dni             = row["dni"].as<std::string>();
                t.nombre_completo = row["nombre_completo"].as<std::string>();
                t.telefono        = row["telefono"].isNull() ? "" : row["telefono"].as<std::string>();
                t.email           = row["email"].isNull() ? "" : row["email"].as<std::string>();
                t.es_principal    = row["es_principal"].as<bool>();
                lista.push_back(std::move(t));
            }
            callback(std::move(lista));
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        idFamilia);
}

void FamiliaRepository::insertarTutor(const Tutor& t,
    std::function<void(int)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "INSERT INTO tutores (id_familia, dni, nombre_completo, telefono, email, es_principal) "
        "VALUES (?,?,?,?,?,?)",
        [callback](const Result& r) { callback(static_cast<int>(r.insertId())); },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        t.id_familia, t.dni, t.nombre_completo, t.telefono, t.email, t.es_principal ? 1 : 0);
}

void FamiliaRepository::actualizarTutor(const Tutor& t,
    std::function<void()> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "UPDATE tutores SET dni=?, nombre_completo=?, telefono=?, email=?, es_principal=? "
        "WHERE id_tutor=?",
        [callback](const Result&) { callback(); },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        t.dni, t.nombre_completo, t.telefono, t.email, t.es_principal ? 1 : 0, t.id_tutor);
}

void FamiliaRepository::eliminarTutor(int idTutor,
    std::function<void()> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "DELETE FROM tutores WHERE id_tutor=?",
        [callback](const Result&) { callback(); },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        idTutor);
}

// -------------------------------------------------------
// Alumnos
// -------------------------------------------------------
void FamiliaRepository::listarAlumnos(int idFamilia,
    std::function<void(std::vector<Alumno>)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "SELECT * FROM alumnos WHERE id_familia=? ORDER BY nombre_completo",
        [callback](const Result& r) {
            std::vector<Alumno> lista;
            for (const auto& row : r) {
                Alumno a;
                a.id_alumno        = row["id_alumno"].as<int>();
                a.id_familia       = row["id_familia"].as<int>();
                a.dni              = row["dni"].isNull() ? "" : row["dni"].as<std::string>();
                a.nombre_completo  = row["nombre_completo"].as<std::string>();
                a.fecha_nacimiento = row["fecha_nacimiento"].isNull() ? "" : row["fecha_nacimiento"].as<std::string>();
                a.anio_escolar     = row["anio_escolar"].isNull() ? "" : row["anio_escolar"].as<std::string>();
                a.estado           = row["estado"].as<std::string>();
                lista.push_back(std::move(a));
            }
            callback(std::move(lista));
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        idFamilia);
}

void FamiliaRepository::insertarAlumno(const Alumno& a,
    std::function<void(int)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "INSERT INTO alumnos (id_familia, dni, nombre_completo, fecha_nacimiento, anio_escolar, estado) "
        "VALUES (?,NULLIF(?,''),?,NULLIF(?,''),?,?)",
        [callback](const Result& r) { callback(static_cast<int>(r.insertId())); },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        a.id_familia, a.dni, a.nombre_completo, a.fecha_nacimiento, a.anio_escolar, a.estado);
}

void FamiliaRepository::actualizarAlumno(const Alumno& a,
    std::function<void()> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "UPDATE alumnos SET dni=NULLIF(?,''), nombre_completo=?, fecha_nacimiento=NULLIF(?,''), "
        "anio_escolar=?, estado=? WHERE id_alumno=?",
        [callback](const Result&) { callback(); },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        a.dni, a.nombre_completo, a.fecha_nacimiento, a.anio_escolar, a.estado, a.id_alumno);
}

void FamiliaRepository::eliminarAlumno(int idAlumno,
    std::function<void()> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "DELETE FROM alumnos WHERE id_alumno=?",
        [callback](const Result&) { callback(); },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        idAlumno);
}

void FamiliaRepository::contarAlumnosActivos(int idFamilia,
    std::function<void(int)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "SELECT COUNT(*) AS cnt FROM alumnos WHERE id_familia=? AND estado='ACTIVO'",
        [callback](const Result& r) {
            callback(r[0]["cnt"].as<int>());
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        idFamilia);
}
