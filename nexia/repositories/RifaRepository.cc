#include "RifaRepository.h"
#include <drogon/drogon.h>
#include <sstream>

using namespace drogon;
using namespace drogon::orm;

// -------------------------------------------------------
// RIFAS
// -------------------------------------------------------
void RifaRepository::listar(
    std::function<void(std::vector<Rifa>)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "SELECT r.*, "
        "  COALESCE(pc.nombre,'') AS nombre_cuenta, "
        "  COUNT(DISTINCT rn.id_numero) AS numeros_asignados, "
        "  COUNT(DISTINCT CASE WHEN rn.estado='PAGADO' THEN rn.id_numero END) AS numeros_pagados, "
        "  COALESCE(SUM(CASE WHEN rc.estado='PAGADA' THEN rc.monto ELSE 0 END),0) AS total_recaudado "
        "FROM rifas r "
        "LEFT JOIN plan_cuentas pc ON r.id_cuenta=pc.id_cuenta "
        "LEFT JOIN rifa_numeros rn ON rn.id_rifa=r.id_rifa AND rn.estado!='ANULADO' "
        "LEFT JOIN rifa_cuotas rc ON rc.id_numero=rn.id_numero "
        "GROUP BY r.id_rifa "
        "ORDER BY r.id_rifa DESC",
        [callback](const Result& rs) {
            std::vector<Rifa> lista;
            for (const auto& row : rs) {
                Rifa r;
                r.id_rifa          = row["id_rifa"].as<int>();
                r.nombre           = row["nombre"].as<std::string>();
                r.descripcion      = row["descripcion"].isNull() ? "" : row["descripcion"].as<std::string>();
                r.fecha_sorteo     = row["fecha_sorteo"].isNull() ? "" : row["fecha_sorteo"].as<std::string>();
                r.cantidad_numeros = row["cantidad_numeros"].as<int>();
                r.cant_cuotas      = row["cant_cuotas"].as<int>();
                r.monto_total      = row["monto_total"].as<double>();
                r.monto_cuota      = row["monto_cuota"].as<double>();
                r.id_cuenta        = row["id_cuenta"].isNull() ? 0 : row["id_cuenta"].as<int>();
                r.estado           = row["estado"].as<std::string>();
                r.fecha_creacion   = row["fecha_creacion"].as<std::string>();
                r.nombre_cuenta    = row["nombre_cuenta"].as<std::string>();
                r.numeros_asignados= row["numeros_asignados"].as<int>();
                r.numeros_pagados  = row["numeros_pagados"].as<int>();
                r.total_recaudado  = row["total_recaudado"].as<double>();
                lista.push_back(std::move(r));
            }
            callback(std::move(lista));
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); });
}

void RifaRepository::obtener(int idRifa,
    std::function<void(Rifa)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "SELECT r.*, COALESCE(pc.nombre,'') AS nombre_cuenta "
        "FROM rifas r "
        "LEFT JOIN plan_cuentas pc ON r.id_cuenta=pc.id_cuenta "
        "WHERE r.id_rifa=?",
        [callback, errCallback](const Result& rs) {
            if (rs.empty()) { errCallback("Rifa no encontrada"); return; }
            const auto& row = rs[0];
            Rifa r;
            r.id_rifa          = row["id_rifa"].as<int>();
            r.nombre           = row["nombre"].as<std::string>();
            r.descripcion      = row["descripcion"].isNull() ? "" : row["descripcion"].as<std::string>();
            r.fecha_sorteo     = row["fecha_sorteo"].isNull() ? "" : row["fecha_sorteo"].as<std::string>();
            r.cantidad_numeros = row["cantidad_numeros"].as<int>();
            r.cant_cuotas      = row["cant_cuotas"].as<int>();
            r.monto_total      = row["monto_total"].as<double>();
            r.monto_cuota      = row["monto_cuota"].as<double>();
            r.id_cuenta        = row["id_cuenta"].isNull() ? 0 : row["id_cuenta"].as<int>();
            r.estado           = row["estado"].as<std::string>();
            r.fecha_creacion   = row["fecha_creacion"].as<std::string>();
            r.nombre_cuenta    = row["nombre_cuenta"].as<std::string>();
            callback(std::move(r));
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        idRifa);
}

void RifaRepository::insertar(const Rifa& r,
    std::function<void(int)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "INSERT INTO rifas (nombre,descripcion,fecha_sorteo,cantidad_numeros,cant_cuotas,"
        "monto_total,monto_cuota,id_cuenta,estado) "
        "VALUES (?,?,NULLIF(?,''),?,?,?,?,NULLIF(?,0),'ACTIVA')",
        [callback](const Result& rs) { callback(static_cast<int>(rs.insertId())); },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        r.nombre, r.descripcion, r.fecha_sorteo, r.cantidad_numeros, r.cant_cuotas,
        r.monto_total, r.monto_cuota, r.id_cuenta);
}

void RifaRepository::actualizar(const Rifa& r,
    std::function<void()> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "UPDATE rifas SET nombre=?,descripcion=?,fecha_sorteo=NULLIF(?,''),"
        "cantidad_numeros=?,cant_cuotas=?,monto_total=?,monto_cuota=?,"
        "id_cuenta=NULLIF(?,0),estado=? WHERE id_rifa=?",
        [callback](const Result&) { callback(); },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        r.nombre, r.descripcion, r.fecha_sorteo, r.cantidad_numeros, r.cant_cuotas,
        r.monto_total, r.monto_cuota, r.id_cuenta, r.estado, r.id_rifa);
}

// -------------------------------------------------------
// NÚMEROS
// -------------------------------------------------------
void RifaRepository::listarNumeros(int idRifa,
    std::function<void(std::vector<RifaNumero>)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "SELECT rn.*, "
        "  COALESCE(f.numero_familia,0) AS numero_familia, "
        "  COALESCE(tp.nombre_completo,'') AS nombre_tutor_principal, "
        "  COALESCE(f.telefono,'') AS telefono_familia, "
        "  ri.nombre AS nombre_rifa, ri.cant_cuotas, ri.monto_total, ri.monto_cuota, "
        "  SUM(CASE WHEN rc.estado='PAGADA' THEN 1 ELSE 0 END) AS cuotas_pagadas, "
        "  SUM(CASE WHEN rc.estado='PENDIENTE' THEN 1 ELSE 0 END) AS cuotas_pendientes "
        "FROM rifa_numeros rn "
        "JOIN rifas ri ON rn.id_rifa=ri.id_rifa "
        "LEFT JOIN familias f ON rn.id_familia=f.id_familia "
        "LEFT JOIN tutores tp ON tp.id_familia=f.id_familia AND tp.es_principal=1 "
        "LEFT JOIN rifa_cuotas rc ON rc.id_numero=rn.id_numero "
        "WHERE rn.id_rifa=? AND rn.estado!='ANULADO' "
        "GROUP BY rn.id_numero "
        "ORDER BY rn.numero",
        [callback](const Result& rs) {
            std::vector<RifaNumero> lista;
            for (const auto& row : rs) {
                RifaNumero rn;
                rn.id_numero         = row["id_numero"].as<int>();
                rn.id_rifa           = row["id_rifa"].as<int>();
                rn.numero            = row["numero"].as<int>();
                rn.id_familia        = row["id_familia"].isNull() ? 0 : row["id_familia"].as<int>();
                rn.nombre_externo    = row["nombre_externo"].isNull() ? "" : row["nombre_externo"].as<std::string>();
                rn.telefono_externo  = row["telefono_externo"].isNull() ? "" : row["telefono_externo"].as<std::string>();
                rn.saldo_pendiente   = row["saldo_pendiente"].as<double>();
                rn.estado            = row["estado"].as<std::string>();
                rn.fecha_asignacion  = row["fecha_asignacion"].as<std::string>();
                rn.numero_familia    = row["numero_familia"].as<int>();
                rn.nombre_tutor_principal = row["nombre_tutor_principal"].as<std::string>();
                rn.telefono_familia  = row["telefono_familia"].as<std::string>();
                rn.nombre_rifa       = row["nombre_rifa"].as<std::string>();
                rn.cant_cuotas       = row["cant_cuotas"].as<int>();
                rn.monto_total       = row["monto_total"].as<double>();
                rn.monto_cuota       = row["monto_cuota"].as<double>();
                rn.cuotas_pagadas    = row["cuotas_pagadas"].as<int>();
                rn.cuotas_pendientes = row["cuotas_pendientes"].as<int>();
                lista.push_back(std::move(rn));
            }
            callback(std::move(lista));
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        idRifa);
}

void RifaRepository::numerosDisponibles(int idRifa,
    std::function<void(Json::Value)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    // Primero obtenemos la cantidad total de números de la rifa
    db->execSqlAsync(
        "SELECT r.cantidad_numeros, "
        "GROUP_CONCAT(rn.numero ORDER BY rn.numero) AS ocupados "
        "FROM rifas r "
        "LEFT JOIN rifa_numeros rn ON rn.id_rifa=r.id_rifa AND rn.estado!='ANULADO' "
        "WHERE r.id_rifa=? GROUP BY r.id_rifa",
        [callback, errCallback](const Result& rs) {
            if (rs.empty()) { errCallback("Rifa no encontrada"); return; }
            int total = rs[0]["cantidad_numeros"].as<int>();
            std::string ocupStr = rs[0]["ocupados"].isNull() ? "" : rs[0]["ocupados"].as<std::string>();
            std::set<int> ocupados;
            std::istringstream ss(ocupStr);
            std::string tok;
            while (std::getline(ss, tok, ',')) if (!tok.empty()) ocupados.insert(std::stoi(tok));
            Json::Value arr(Json::arrayValue);
            for (int i = 1; i <= total; i++)
                if (ocupados.find(i) == ocupados.end())
                    arr.append(i);
            callback(arr);
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        idRifa);
}

void RifaRepository::asignarNumero(const RifaNumero& rn,
    std::function<void(int)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    // Insertar número y luego generar cuotas automáticamente
    db->execSqlAsync(
        "INSERT INTO rifa_numeros "
        "(id_rifa,numero,id_familia,nombre_externo,telefono_externo,saldo_pendiente,estado) "
        "VALUES (?,?,NULLIF(?,0),NULLIF(?,''),NULLIF(?,''),?,CASE WHEN ?>0 THEN 'ASIGNADO' ELSE 'ASIGNADO' END)",
        [db, rn, callback, errCallback](const Result& rs) {
            int idNumero = static_cast<int>(rs.insertId());
            // Generar cuotas
            if (rn.cant_cuotas <= 0 || rn.monto_cuota <= 0.0) {
                callback(idNumero);
                return;
            }
            // Construir INSERT múltiple para las cuotas
            std::string sql = "INSERT INTO rifa_cuotas (id_numero,nro_cuota,monto,estado) VALUES ";
            for (int i = 1; i <= rn.cant_cuotas; i++) {
                if (i > 1) sql += ",";
                sql += "(" + std::to_string(idNumero) + "," + std::to_string(i) +
                       "," + std::to_string(rn.monto_cuota) + ",'PENDIENTE')";
            }
            db->execSqlAsync(sql,
                [idNumero, callback](const Result&) { callback(idNumero); },
                [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); });
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        rn.id_rifa, rn.numero, rn.id_familia, rn.nombre_externo, rn.telefono_externo,
        rn.saldo_pendiente, rn.id_familia);
}

void RifaRepository::asignarATodas(int idRifa,
    std::function<void(int)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    // Obtener datos de la rifa y familias activas
    db->execSqlAsync(
        "SELECT r.cantidad_numeros, r.cant_cuotas, r.monto_cuota, "
        "GROUP_CONCAT(rn.numero ORDER BY rn.numero) AS ocupados "
        "FROM rifas r "
        "LEFT JOIN rifa_numeros rn ON rn.id_rifa=r.id_rifa AND rn.estado!='ANULADO' "
        "WHERE r.id_rifa=? GROUP BY r.id_rifa",
        [db, idRifa, callback, errCallback](const Result& rs) {
            if (rs.empty()) { errCallback("Rifa no encontrada"); return; }
            int totalNum    = rs[0]["cantidad_numeros"].as<int>();
            int cantCuotas  = rs[0]["cant_cuotas"].as<int>();
            double mCuota   = rs[0]["monto_cuota"].as<double>();
            std::string ocupStr = rs[0]["ocupados"].isNull() ? "" : rs[0]["ocupados"].as<std::string>();
            std::set<int> ocupados;
            std::istringstream ss(ocupStr);
            std::string tok;
            while (std::getline(ss, tok, ',')) if (!tok.empty()) ocupados.insert(std::stoi(tok));

            // Armar lista de números disponibles
            std::vector<int> disponibles;
            for (int i = 1; i <= totalNum; i++)
                if (ocupados.find(i) == ocupados.end()) disponibles.push_back(i);

            if (disponibles.empty()) { callback(0); return; }

            db->execSqlAsync(
                "SELECT id_familia FROM familias WHERE estado='ACTIVA' ORDER BY numero_familia",
                [db, idRifa, cantCuotas, mCuota, disponibles, callback, errCallback](const Result& rf) {
                    if (rf.empty()) { callback(0); return; }
                    size_t idx = 0;
                    std::string sqlNum = "INSERT IGNORE INTO rifa_numeros "
                        "(id_rifa,numero,id_familia,saldo_pendiente,estado) VALUES ";
                    size_t asignados = std::min(disponibles.size(), (size_t)rf.size());
                    for (size_t i = 0; i < asignados; i++) {
                        if (i > 0) sqlNum += ",";
                        sqlNum += "(" + std::to_string(idRifa) + "," +
                                  std::to_string(disponibles[i]) + "," +
                                  std::to_string(rf[i]["id_familia"].as<int>()) + ",0,'ASIGNADO')";
                    }
                    db->execSqlAsync(sqlNum,
                        [db, idRifa, cantCuotas, mCuota, asignados, callback, errCallback](const Result&) {
                            // Generar cuotas para todos los números recién asignados
                            if (cantCuotas <= 0 || mCuota <= 0.0) { callback(static_cast<int>(asignados)); return; }
                            db->execSqlAsync(
                                "INSERT INTO rifa_cuotas (id_numero,nro_cuota,monto,estado) "
                                "SELECT rn.id_numero, seq.n, ?, 'PENDIENTE' "
                                "FROM rifa_numeros rn "
                                "JOIN (SELECT 1 AS n UNION ALL SELECT 2 UNION ALL SELECT 3 "
                                "      UNION ALL SELECT 4 UNION ALL SELECT 5 UNION ALL SELECT 6 "
                                "      UNION ALL SELECT 7 UNION ALL SELECT 8 UNION ALL SELECT 9 "
                                "      UNION ALL SELECT 10 UNION ALL SELECT 11 UNION ALL SELECT 12) seq "
                                "ON seq.n <= ? "
                                "WHERE rn.id_rifa=? "
                                "AND NOT EXISTS (SELECT 1 FROM rifa_cuotas rc WHERE rc.id_numero=rn.id_numero)",
                                [asignados, callback](const Result&) { callback(static_cast<int>(asignados)); },
                                [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
                                mCuota, cantCuotas, idRifa);
                        },
                        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); });
                },
                [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); });
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        idRifa);
}

void RifaRepository::anularNumero(int idNumero,
    std::function<void()> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "UPDATE rifa_numeros SET estado='ANULADO' WHERE id_numero=?",
        [db, idNumero, callback, errCallback](const Result&) {
            db->execSqlAsync(
                "UPDATE rifa_cuotas SET estado='ANULADA' WHERE id_numero=? AND estado='PENDIENTE'",
                [callback](const Result&) { callback(); },
                [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
                idNumero);
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        idNumero);
}

// -------------------------------------------------------
// CUOTAS
// -------------------------------------------------------
void RifaRepository::listarCuotas(int idNumero,
    std::function<void(Json::Value)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "SELECT * FROM rifa_cuotas WHERE id_numero=? ORDER BY nro_cuota",
        [callback](const Result& rs) {
            Json::Value arr(Json::arrayValue);
            for (const auto& row : rs) {
                Json::Value o;
                o["id_cuota"]    = row["id_cuota"].as<int>();
                o["id_numero"]   = row["id_numero"].as<int>();
                o["nro_cuota"]   = row["nro_cuota"].as<int>();
                o["monto"]       = row["monto"].as<double>();
                o["estado"]      = row["estado"].as<std::string>();
                o["fecha_pago"]  = row["fecha_pago"].isNull() ? "" : row["fecha_pago"].as<std::string>();
                o["medio_pago"]  = row["medio_pago"].isNull() ? "" : row["medio_pago"].as<std::string>();
                arr.append(o);
            }
            callback(arr);
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        idNumero);
}

void RifaRepository::pagarCuotas(const std::vector<int>& idCuotas, const std::string& medioPago,
    std::function<void(int)> callback,
    std::function<void(const std::string&)> errCallback)
{
    if (idCuotas.empty()) { callback(0); return; }
    auto db = app().getDbClient("main");

    // 1. Obtener datos de las cuotas para calcular monto total y cuenta contable
    std::string inList;
    for (size_t i = 0; i < idCuotas.size(); i++) {
        if (i > 0) inList += ",";
        inList += std::to_string(idCuotas[i]);
    }
    db->execSqlAsync(
        "SELECT rc.id_cuota, rc.monto, ri.id_cuenta, ri.nombre AS nombre_rifa, "
        "rn.id_numero "
        "FROM rifa_cuotas rc "
        "JOIN rifa_numeros rn ON rc.id_numero=rn.id_numero "
        "JOIN rifas ri ON rn.id_rifa=ri.id_rifa "
        "WHERE rc.id_cuota IN (" + inList + ") AND rc.estado='PENDIENTE'",
        [db, idCuotas, medioPago, inList, callback, errCallback](const Result& rs) {
            if (rs.empty()) { errCallback("No hay cuotas pendientes seleccionadas"); return; }

            double total    = 0.0;
            int    idCuenta = 0;
            std::string nombreRifa;
            std::set<int> numerosAfectados;
            for (const auto& row : rs) {
                total += row["monto"].as<double>();
                idCuenta    = row["id_cuenta"].isNull() ? 0 : row["id_cuenta"].as<int>();
                nombreRifa  = row["nombre_rifa"].as<std::string>();
                numerosAfectados.insert(row["id_numero"].as<int>());
            }

            // 2. Insertar movimiento contable si hay cuenta configurada
            auto actualizarCuotas = [db, inList, medioPago, numerosAfectados, callback, errCallback](int idMov) {
                // 3. Marcar cuotas como pagadas
                db->execSqlAsync(
                    "UPDATE rifa_cuotas SET estado='PAGADA', fecha_pago=NOW(), "
                    "medio_pago=?, id_movimiento=NULLIF(?,0) "
                    "WHERE id_cuota IN (" + inList + ")",
                    [db, numerosAfectados, idMov, callback, errCallback](const Result&) {
                        // 4. Actualizar saldo pendiente y estado de cada número afectado
                        std::string numList;
                        for (auto id : numerosAfectados) {
                            if (!numList.empty()) numList += ",";
                            numList += std::to_string(id);
                        }
                        db->execSqlAsync(
                            "UPDATE rifa_numeros rn "
                            "SET saldo_pendiente = ("
                            "  SELECT COALESCE(SUM(rc2.monto),0) FROM rifa_cuotas rc2 "
                            "  WHERE rc2.id_numero=rn.id_numero AND rc2.estado='PENDIENTE'"
                            "), "
                            "estado = CASE "
                            "  WHEN (SELECT COUNT(*) FROM rifa_cuotas rc3 "
                            "        WHERE rc3.id_numero=rn.id_numero AND rc3.estado='PENDIENTE')=0 THEN 'PAGADO' "
                            "  WHEN (SELECT COUNT(*) FROM rifa_cuotas rc4 "
                            "        WHERE rc4.id_numero=rn.id_numero AND rc4.estado='PAGADA')>0 THEN 'PAGADO_PARCIAL' "
                            "  ELSE 'ASIGNADO' END "
                            "WHERE rn.id_numero IN (" + numList + ")",
                            [idMov, callback](const Result&) { callback(idMov); },
                            [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); });
                    },
                    [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
                    medioPago, idMov);
            };

            if (idCuenta > 0) {
                db->execSqlAsync(
                    "INSERT INTO movimientos (id_cuenta,fecha,descripcion,monto,tipo,medio_pago) "
                    "VALUES (?,NOW(),?,?,'INGRESO',?)",
                    [actualizarCuotas](const Result& r) {
                        actualizarCuotas(static_cast<int>(r.insertId()));
                    },
                    [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
                    idCuenta, "Pago rifa: " + nombreRifa, total, medioPago);
            } else {
                actualizarCuotas(0);
            }
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); });
}
