#include "InventarioRepository.h"
#include <drogon/drogon.h>

using namespace drogon;
using namespace drogon::orm;

// ---------------------------------------------------------------
// ELEMENTOS
// ---------------------------------------------------------------

void InventarioRepository::listarElementos(
    std::function<void(std::vector<ElementoInventario>)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "SELECT * FROM elementos_inventario WHERE activo = 1 ORDER BY nombre",
        [callback](const Result& r) {
            std::vector<ElementoInventario> lista;
            for (const auto& row : r) {
                ElementoInventario e;
                e.id_elemento      = row["id_elemento"].as<int>();
                e.nombre           = row["nombre"].as<std::string>();
                e.descripcion      = row["descripcion"].isNull() ? "" : row["descripcion"].as<std::string>();
                e.stock_total      = row["stock_total"].as<int>();
                e.stock_disponible = row["stock_disponible"].as<int>();
                e.activo           = row["activo"].as<int>() == 1;
                lista.push_back(std::move(e));
            }
            callback(std::move(lista));
        },
        [errCallback](const DrogonDbException& e) {
            errCallback(e.base().what());
        });
}

void InventarioRepository::crearElemento(
    const ElementoInventario& el,
    std::function<void(int)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "INSERT INTO elementos_inventario (nombre, descripcion, stock_total, stock_disponible) "
        "VALUES (?, ?, ?, ?)",
        [callback](const Result& r) {
            callback(static_cast<int>(r.insertId()));
        },
        [errCallback](const DrogonDbException& e) {
            errCallback(e.base().what());
        },
        el.nombre, el.descripcion, el.stock_total, el.stock_total);
}

void InventarioRepository::actualizarElemento(
    const ElementoInventario& el,
    std::function<void()> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "UPDATE elementos_inventario SET nombre=?, descripcion=?, stock_total=?, activo=? "
        "WHERE id_elemento=?",
        [callback](const Result&) { callback(); },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        el.nombre, el.descripcion, el.stock_total, el.activo ? 1 : 0, el.id_elemento);
}

// ---------------------------------------------------------------
// PRÉSTAMOS
// ---------------------------------------------------------------

void InventarioRepository::listarPrestamos(
    int idElemento, const std::string& estado,
    std::function<void(std::vector<Prestamo>)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    std::string sql =
        "SELECT p.*, "
        "e.nombre AS nombre_elemento, "
        "s.nombre_completo AS nombre_socio, s.numero_socio, s.dni AS dni_socio "
        "FROM prestamos p "
        "JOIN elementos_inventario e ON p.id_elemento = e.id_elemento "
        "JOIN socios s ON p.id_socio = s.id_socio "
        "WHERE 1=1 ";
    if (idElemento > 0) sql += "AND p.id_elemento = " + std::to_string(idElemento) + " ";
    if (!estado.empty()) sql += "AND p.estado = '" + estado + "' ";
    sql += "ORDER BY p.fecha_prestamo DESC";

    db->execSqlAsync(sql,
        [callback](const Result& r) {
            std::vector<Prestamo> lista;
            for (const auto& row : r) {
                Prestamo p;
                p.id_prestamo       = row["id_prestamo"].as<int>();
                p.id_elemento       = row["id_elemento"].as<int>();
                p.id_socio          = row["id_socio"].as<int>();
                p.fecha_prestamo    = row["fecha_prestamo"].as<std::string>();
                p.fecha_devolucion  = row["fecha_devolucion"].isNull() ? "" : row["fecha_devolucion"].as<std::string>();
                p.observaciones     = row["observaciones"].isNull() ? "" : row["observaciones"].as<std::string>();
                p.estado            = row["estado"].as<std::string>();
                p.nombre_elemento   = row["nombre_elemento"].as<std::string>();
                p.nombre_socio      = row["nombre_socio"].as<std::string>();
                p.numero_socio      = row["numero_socio"].as<int>();
                p.dni_socio         = row["dni_socio"].isNull() ? "" : row["dni_socio"].as<std::string>();
                lista.push_back(std::move(p));
            }
            callback(std::move(lista));
        },
        [errCallback](const DrogonDbException& e) {
            errCallback(e.base().what());
        });
}

void InventarioRepository::registrarPrestamo(
    const Prestamo& p,
    std::function<void(int)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    // Insertar préstamo y decrementar stock_disponible en una transacción
    auto trans = db->newTransaction();
    trans->execSqlAsync(
        "INSERT INTO prestamos (id_elemento, id_socio, fecha_prestamo, observaciones) "
        "VALUES (?, ?, ?, ?)",
        [trans, callback, errCallback, idElemento = p.id_elemento](const Result& r) {
            int newId = static_cast<int>(r.insertId());
            trans->execSqlAsync(
                "UPDATE elementos_inventario "
                "SET stock_disponible = stock_disponible - 1 "
                "WHERE id_elemento = ? AND stock_disponible > 0",
                [callback, newId](const Result& upd) {
                    if (upd.affectedRows() == 0) {
                        // No hay stock disponible — la transacción se revierte
                        throw std::runtime_error("Sin stock disponible para este elemento");
                    }
                    callback(newId);
                },
                [errCallback](const DrogonDbException& e) {
                    errCallback(e.base().what());
                },
                idElemento);
        },
        [errCallback](const DrogonDbException& e) {
            errCallback(e.base().what());
        },
        p.id_elemento, p.id_socio, p.fecha_prestamo, p.observaciones);
}

void InventarioRepository::registrarDevolucion(
    int idPrestamo, const std::string& fechaDevolucion,
    std::function<void()> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    auto trans = db->newTransaction();
    trans->execSqlAsync(
        "UPDATE prestamos SET estado='DEVUELTO', fecha_devolucion=? "
        "WHERE id_prestamo=? AND estado='PRESTADO'",
        [trans, callback, errCallback, idPrestamo](const Result& r) {
            if (r.affectedRows() == 0) {
                throw std::runtime_error("Préstamo no encontrado o ya devuelto");
            }
            trans->execSqlAsync(
                "UPDATE elementos_inventario ei "
                "JOIN prestamos p ON p.id_elemento = ei.id_elemento "
                "SET ei.stock_disponible = ei.stock_disponible + 1 "
                "WHERE p.id_prestamo = ?",
                [callback](const Result&) { callback(); },
                [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
                idPrestamo);
        },
        [errCallback](const DrogonDbException& e) {
            errCallback(e.base().what());
        },
        fechaDevolucion, idPrestamo);
}
