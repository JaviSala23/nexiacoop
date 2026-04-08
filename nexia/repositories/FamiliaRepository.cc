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

void FamiliaRepository::listarTodosAlumnos(const std::string& estado, const std::string& anioEscolar,
    std::function<void(std::vector<Alumno>)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    std::string sql =
        "SELECT a.id_alumno, a.id_familia, a.dni, a.nombre_completo, "
        "a.fecha_nacimiento, a.anio_escolar, a.estado, "
        "f.numero_familia, "
        "tp.nombre_completo AS nombre_tutor_principal "
        "FROM alumnos a "
        "LEFT JOIN familias f ON a.id_familia = f.id_familia "
        "LEFT JOIN tutores tp ON tp.id_familia = f.id_familia AND tp.es_principal = 1 "
        "WHERE 1=1 ";
    if (!estado.empty() && estado != "TODOS") sql += "AND a.estado = '" + estado + "' ";
    if (!anioEscolar.empty()) sql += "AND a.anio_escolar = '" + anioEscolar + "' ";
    sql += "ORDER BY a.nombre_completo";

    db->execSqlAsync(sql,
        [callback](const Result& r) {
            std::vector<Alumno> lista;
            for (const auto& row : r) {
                Alumno a;
                a.id_alumno              = row["id_alumno"].as<int>();
                a.id_familia             = row["id_familia"].as<int>();
                a.dni                    = row["dni"].isNull() ? "" : row["dni"].as<std::string>();
                a.nombre_completo        = row["nombre_completo"].as<std::string>();
                a.fecha_nacimiento       = row["fecha_nacimiento"].isNull() ? "" : row["fecha_nacimiento"].as<std::string>();
                a.anio_escolar           = row["anio_escolar"].isNull() ? "" : row["anio_escolar"].as<std::string>();
                a.estado                 = row["estado"].as<std::string>();
                a.numero_familia         = row["numero_familia"].isNull() ? 0 : row["numero_familia"].as<int>();
                a.nombre_tutor_principal = row["nombre_tutor_principal"].isNull() ? "" : row["nombre_tutor_principal"].as<std::string>();
                lista.push_back(std::move(a));
            }
            callback(std::move(lista));
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); });
}

// -------------------------------------------------------
// Stats de Alumnos
// -------------------------------------------------------
void FamiliaRepository::statsAlumnos(
    std::function<void(Json::Value)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "SELECT "
        "(SELECT COUNT(*) FROM alumnos WHERE estado='ACTIVO') AS total_activos, "
        "(SELECT COUNT(*) FROM alumnos WHERE estado='EGRESADO') AS total_egresados, "
        "(SELECT COUNT(*) FROM alumnos WHERE estado='INACTIVO') AS total_inactivos, "
        "(SELECT COUNT(*) FROM familias WHERE estado='ACTIVA') AS familias_activas, "
        "(SELECT ROUND(AVG(c),1) FROM (SELECT COUNT(*) AS c FROM alumnos WHERE estado='ACTIVO' GROUP BY id_familia) x) AS promedio_por_familia",
        [db, callback, errCallback](const Result& r1) {
            Json::Value stats;
            if (!r1.empty()) {
                const auto& row = r1[0];
                stats["total_activos"]        = row["total_activos"].as<int>();
                stats["total_egresados"]      = row["total_egresados"].as<int>();
                stats["total_inactivos"]      = row["total_inactivos"].as<int>();
                stats["familias_activas"]     = row["familias_activas"].as<int>();
                stats["promedio_por_familia"] = row["promedio_por_familia"].isNull() ? 0.0 : row["promedio_por_familia"].as<double>();
            }
            db->execSqlAsync(
                "SELECT c.nombre AS grado, c.orden, "
                "COALESCE(COUNT(a.id_alumno),0) AS cantidad "
                "FROM cursos c "
                "LEFT JOIN alumnos a ON a.anio_escolar = c.nombre AND a.estado = 'ACTIVO' "
                "WHERE c.activo = 1 "
                "GROUP BY c.id_curso, c.nombre, c.orden "
                "ORDER BY c.orden",
                [db, stats, callback, errCallback](const Result& r2) mutable {
                    Json::Value porGrado(Json::arrayValue);
                    for (const auto& row : r2) {
                        Json::Value g;
                        g["grado"]    = row["grado"].as<std::string>();
                        g["orden"]    = row["orden"].as<int>();
                        g["cantidad"] = row["cantidad"].as<int>();
                        porGrado.append(g);
                    }
                    stats["por_grado"] = porGrado;
                    db->execSqlAsync(
                        "SELECT cantidad_hijos, COUNT(*) AS familias "
                        "FROM (SELECT id_familia, COUNT(*) AS cantidad_hijos "
                        "      FROM alumnos WHERE estado='ACTIVO' GROUP BY id_familia) sub "
                        "GROUP BY cantidad_hijos ORDER BY cantidad_hijos",
                        [stats, callback](const Result& r3) mutable {
                            Json::Value dist(Json::arrayValue);
                            for (const auto& row : r3) {
                                Json::Value d;
                                d["cantidad"] = row["cantidad_hijos"].as<int>();
                                d["familias"] = row["familias"].as<int>();
                                dist.append(d);
                            }
                            stats["dist_hijos_por_familia"] = dist;
                            callback(std::move(stats));
                        },
                        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); });
                },
                [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); });
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); });
}

// -------------------------------------------------------
// Promover grados (pase de año) — con snapshot para revertir
// -------------------------------------------------------
void FamiliaRepository::promoverGrados(
    std::function<void(int, int, int)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    // Paso 0: guardar snapshot de alumnos activos y familias activas
    db->execSqlAsync(
        "INSERT INTO pase_anio_historial (egresados, promovidos, familias_inactiv) VALUES (0, 0, 0)",
        [db, callback, errCallback](const Result& rH) {
            int idHistorial = static_cast<int>(rH.insertId());
            // Snapshot: alumnos activos → guardar anio_escolar y estado
            db->execSqlAsync(
                "INSERT INTO pase_anio_detalle (id_historial, tipo, ref_id, campo_anterior, valor_anterior) "
                "SELECT ?, 'ALUMNO', a.id_alumno, 'anio_escolar', COALESCE(a.anio_escolar,'') "
                "FROM alumnos a WHERE a.estado = 'ACTIVO'",
                [db, idHistorial, callback, errCallback](const Result&) {
                    db->execSqlAsync(
                        "INSERT INTO pase_anio_detalle (id_historial, tipo, ref_id, campo_anterior, valor_anterior) "
                        "SELECT ?, 'ALUMNO', a.id_alumno, 'estado', a.estado "
                        "FROM alumnos a WHERE a.estado = 'ACTIVO'",
                        [db, idHistorial, callback, errCallback](const Result&) {
                            // Snapshot: familias activas → guardar estado
                            db->execSqlAsync(
                                "INSERT INTO pase_anio_detalle (id_historial, tipo, ref_id, campo_anterior, valor_anterior) "
                                "SELECT ?, 'FAMILIA', f.id_familia, 'estado', f.estado "
                                "FROM familias f WHERE f.estado = 'ACTIVA'",
                                [db, idHistorial, callback, errCallback](const Result&) {
                                    // Paso 1: egresar alumnos del último grado activo
                                    db->execSqlAsync(
                                        "UPDATE alumnos a "
                                        "JOIN cursos c ON a.anio_escolar = c.nombre "
                                        "SET a.estado = 'EGRESADO' "
                                        "WHERE a.estado = 'ACTIVO' AND c.activo = 1 "
                                        "AND c.orden = (SELECT MAX(orden) FROM cursos WHERE activo = 1)",
                                        [db, idHistorial, callback, errCallback](const Result& r1) {
                                            int egresados = static_cast<int>(r1.affectedRows());
                                            // Paso 2: avanzar el resto al siguiente grado
                                            db->execSqlAsync(
                                                "UPDATE alumnos a "
                                                "JOIN ("
                                                "  SELECT c1.nombre AS nombre_actual, c2.nombre AS nombre_siguiente "
                                                "  FROM cursos c1 "
                                                "  JOIN cursos c2 ON c2.activo = 1 "
                                                "  WHERE c1.activo = 1 "
                                                "    AND c2.orden = (SELECT MIN(c3.orden) FROM cursos c3 WHERE c3.activo = 1 AND c3.orden > c1.orden) "
                                                ") sig ON a.anio_escolar = sig.nombre_actual "
                                                "SET a.anio_escolar = sig.nombre_siguiente "
                                                "WHERE a.estado = 'ACTIVO'",
                                                [db, idHistorial, egresados, callback, errCallback](const Result& r2) {
                                                    int promovidos = static_cast<int>(r2.affectedRows());
                                                    // Paso 3: inactivar familias sin alumnos activos
                                                    db->execSqlAsync(
                                                        "UPDATE familias f SET f.estado = 'INACTIVA' "
                                                        "WHERE f.estado = 'ACTIVA' "
                                                        "AND (SELECT COUNT(*) FROM alumnos a WHERE a.id_familia = f.id_familia AND a.estado = 'ACTIVO') = 0",
                                                        [db, idHistorial, egresados, promovidos, callback, errCallback](const Result& r3) {
                                                            int familias_inact = static_cast<int>(r3.affectedRows());
                                                            // Actualizar totales en historial
                                                            db->execSqlAsync(
                                                                "UPDATE pase_anio_historial SET egresados=?, promovidos=?, familias_inactiv=? WHERE id_historial=?",
                                                                [egresados, promovidos, familias_inact, callback](const Result&) {
                                                                    callback(egresados, promovidos, familias_inact);
                                                                },
                                                                [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
                                                                egresados, promovidos, familias_inact, idHistorial);
                                                        },
                                                        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); });
                                                },
                                                [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); });
                                        },
                                        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); });
                                },
                                [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
                                idHistorial);
                        },
                        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
                        idHistorial);
                },
                [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
                idHistorial);
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); });
}

// -------------------------------------------------------
// Estado de la última promoción (para mostrar botón revertir)
// -------------------------------------------------------
void FamiliaRepository::estadoUltimaPromocion(
    std::function<void(bool, std::string)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "SELECT id_historial, fecha_ejecucion, egresados, promovidos, familias_inactiv, revertido "
        "FROM pase_anio_historial ORDER BY id_historial DESC LIMIT 1",
        [callback](const Result& r) {
            if (r.empty() || r[0]["revertido"].as<int>() != 0) {
                callback(false, "");
                return;
            }
            const auto& row = r[0];
            Json::Value v;
            v["id_historial"]     = row["id_historial"].as<int>();
            v["fecha_ejecucion"]  = row["fecha_ejecucion"].as<std::string>();
            v["egresados"]        = row["egresados"].as<int>();
            v["promovidos"]       = row["promovidos"].as<int>();
            v["familias_inactiv"] = row["familias_inactiv"].as<int>();
            Json::FastWriter fw;
            callback(true, fw.write(v));
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); });
}

// -------------------------------------------------------
// Revertir última promoción
// -------------------------------------------------------
void FamiliaRepository::revertirPromocion(
    std::function<void(bool, std::string)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    // Obtener el último pase no revertido
    db->execSqlAsync(
        "SELECT id_historial FROM pase_anio_historial WHERE revertido=0 ORDER BY id_historial DESC LIMIT 1",
        [db, callback, errCallback](const Result& rH) {
            if (rH.empty()) { callback(false, "No hay pase de año para revertir."); return; }
            int idHistorial = rH[0]["id_historial"].as<int>();
            // Restaurar anio_escolar de alumnos
            db->execSqlAsync(
                "UPDATE alumnos a "
                "JOIN pase_anio_detalle d ON d.ref_id = a.id_alumno AND d.id_historial = ? AND d.tipo = 'ALUMNO' AND d.campo_anterior = 'anio_escolar' "
                "SET a.anio_escolar = d.valor_anterior",
                [db, idHistorial, callback, errCallback](const Result&) {
                    // Restaurar estado de alumnos
                    db->execSqlAsync(
                        "UPDATE alumnos a "
                        "JOIN pase_anio_detalle d ON d.ref_id = a.id_alumno AND d.id_historial = ? AND d.tipo = 'ALUMNO' AND d.campo_anterior = 'estado' "
                        "SET a.estado = d.valor_anterior",
                        [db, idHistorial, callback, errCallback](const Result&) {
                            // Restaurar estado de familias
                            db->execSqlAsync(
                                "UPDATE familias f "
                                "JOIN pase_anio_detalle d ON d.ref_id = f.id_familia AND d.id_historial = ? AND d.tipo = 'FAMILIA' AND d.campo_anterior = 'estado' "
                                "SET f.estado = d.valor_anterior",
                                [db, idHistorial, callback, errCallback](const Result&) {
                                    // Marcar como revertido
                                    db->execSqlAsync(
                                        "UPDATE pase_anio_historial SET revertido=1, fecha_reversion=NOW() WHERE id_historial=?",
                                        [callback](const Result&) {
                                            callback(true, "Pase de año revertido correctamente.");
                                        },
                                        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
                                        idHistorial);
                                },
                                [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
                                idHistorial);
                        },
                        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
                        idHistorial);
                },
                [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
                idHistorial);
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); });
}
