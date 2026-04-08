#include "InventarioController.h"
#include <drogon/drogon.h>
#include "../repositories/InventarioRepository.h"

static drogon::HttpResponsePtr jsonError(const std::string& msg,
                                         drogon::HttpStatusCode code = drogon::k400BadRequest)
{
    Json::Value e; e["error"] = msg;
    auto r = drogon::HttpResponse::newHttpJsonResponse(e);
    r->setStatusCode(code);
    return r;
}

// GET /inventario
void InventarioController::pagina(
    const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    callback(drogon::HttpResponse::newFileResponse("views/inventario.html"));
}

// GET /api/inventario/elementos
void InventarioController::listarElementos(
    const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    InventarioRepository::listarElementos(
        [callback](std::vector<ElementoInventario> lista) {
            Json::Value arr(Json::arrayValue);
            for (const auto& e : lista) {
                Json::Value o;
                o["id_elemento"]      = e.id_elemento;
                o["nombre"]           = e.nombre;
                o["descripcion"]      = e.descripcion;
                o["stock_total"]      = e.stock_total;
                o["stock_disponible"] = e.stock_disponible;
                o["stock_prestado"]   = e.stock_total - e.stock_disponible;
                arr.append(o);
            }
            callback(drogon::HttpResponse::newHttpJsonResponse(arr));
        },
        [callback](const std::string& e) {
            LOG_ERROR << e;
            callback(jsonError(e, drogon::k500InternalServerError));
        });
}

// POST /api/inventario/elementos
void InventarioController::crearElemento(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    auto body = req->getJsonObject();
    if (!body || !(*body)["nombre"].isString() || (*body)["nombre"].asString().empty()) {
        callback(jsonError("El campo 'nombre' es requerido")); return;
    }
    if (!(*body)["stock_total"].isInt() || (*body)["stock_total"].asInt() < 0) {
        callback(jsonError("El campo 'stock_total' debe ser un entero >= 0")); return;
    }

    ElementoInventario el;
    el.nombre      = (*body)["nombre"].asString();
    el.descripcion = (*body)["descripcion"].isString() ? (*body)["descripcion"].asString() : "";
    el.stock_total = (*body)["stock_total"].asInt();

    InventarioRepository::crearElemento(el,
        [callback](int newId) {
            Json::Value res; res["ok"] = true; res["id_elemento"] = newId;
            callback(drogon::HttpResponse::newHttpJsonResponse(res));
        },
        [callback](const std::string& e) {
            LOG_ERROR << e;
            callback(jsonError(e, drogon::k500InternalServerError));
        });
}

// PUT /api/inventario/elementos/{id}
void InventarioController::actualizarElemento(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    int id)
{
    auto body = req->getJsonObject();
    if (!body || !(*body)["nombre"].isString() || (*body)["nombre"].asString().empty()) {
        callback(jsonError("El campo 'nombre' es requerido")); return;
    }

    ElementoInventario el;
    el.id_elemento = id;
    el.nombre      = (*body)["nombre"].asString();
    el.descripcion = (*body)["descripcion"].isString() ? (*body)["descripcion"].asString() : "";
    el.stock_total = (*body)["stock_total"].isInt() ? (*body)["stock_total"].asInt() : 0;
    el.activo      = !(*body)["activo"].isBool() || (*body)["activo"].asBool();

    InventarioRepository::actualizarElemento(el,
        [callback]() {
            Json::Value res; res["ok"] = true;
            callback(drogon::HttpResponse::newHttpJsonResponse(res));
        },
        [callback](const std::string& e) {
            LOG_ERROR << e;
            callback(jsonError(e, drogon::k500InternalServerError));
        });
}

// GET /api/inventario/prestamos?id_elemento=&estado=
void InventarioController::listarPrestamos(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    int idElemento = 0;
    auto pe = req->getParameter("id_elemento"); if (!pe.empty()) idElemento = std::stoi(pe);
    std::string estado = req->getParameter("estado");

    InventarioRepository::listarPrestamos(idElemento, estado,
        [callback](std::vector<Prestamo> lista) {
            Json::Value arr(Json::arrayValue);
            for (const auto& p : lista) {
                Json::Value o;
                o["id_prestamo"]      = p.id_prestamo;
                o["id_elemento"]      = p.id_elemento;
                o["id_socio"]         = p.id_socio;
                o["nombre_elemento"]  = p.nombre_elemento;
                o["nombre_socio"]     = p.nombre_socio;
                o["numero_socio"]     = p.numero_socio;
                o["dni_socio"]        = p.dni_socio;
                o["fecha_prestamo"]   = p.fecha_prestamo;
                o["fecha_devolucion"] = p.fecha_devolucion;
                o["observaciones"]    = p.observaciones;
                o["estado"]           = p.estado;
                arr.append(o);
            }
            callback(drogon::HttpResponse::newHttpJsonResponse(arr));
        },
        [callback](const std::string& e) {
            LOG_ERROR << e;
            callback(jsonError(e, drogon::k500InternalServerError));
        });
}

// POST /api/inventario/prestamos
void InventarioController::registrarPrestamo(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    auto body = req->getJsonObject();
    if (!body || !(*body)["id_elemento"].isInt() || !(*body)["id_socio"].isInt()) {
        callback(jsonError("Se requieren id_elemento e id_socio")); return;
    }
    if (!(*body)["fecha_prestamo"].isString() || (*body)["fecha_prestamo"].asString().empty()) {
        callback(jsonError("Se requiere fecha_prestamo")); return;
    }

    Prestamo p;
    p.id_elemento     = (*body)["id_elemento"].asInt();
    p.id_socio        = (*body)["id_socio"].asInt();
    p.fecha_prestamo  = (*body)["fecha_prestamo"].asString();
    p.observaciones   = (*body)["observaciones"].isString() ? (*body)["observaciones"].asString() : "";

    InventarioRepository::registrarPrestamo(p,
        [callback](int newId) {
            Json::Value res; res["ok"] = true; res["id_prestamo"] = newId;
            callback(drogon::HttpResponse::newHttpJsonResponse(res));
        },
        [callback](const std::string& e) {
            LOG_ERROR << e;
            callback(jsonError(e));
        });
}

// POST /api/inventario/prestamos/{id}/devolucion
void InventarioController::registrarDevolucion(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    int id)
{
    auto body = req->getJsonObject();
    if (!body || !(*body)["fecha_devolucion"].isString() || (*body)["fecha_devolucion"].asString().empty()) {
        callback(jsonError("Se requiere fecha_devolucion")); return;
    }

    InventarioRepository::registrarDevolucion(id, (*body)["fecha_devolucion"].asString(),
        [callback]() {
            Json::Value res; res["ok"] = true;
            callback(drogon::HttpResponse::newHttpJsonResponse(res));
        },
        [callback](const std::string& e) {
            LOG_ERROR << e;
            callback(jsonError(e));
        });
}
