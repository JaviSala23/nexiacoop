#include "ContabilidadController.h"
#include <drogon/drogon.h>
#include "../repositories/ContabilidadRepository.h"

static Json::Value cuentaToJson(const PlanCuenta& c) {
    Json::Value o;
    o["id_cuenta"] = c.id_cuenta;
    o["codigo"]    = c.codigo;
    o["nombre"]    = c.nombre;
    o["tipo"]      = c.tipo;
    o["activo"]    = c.activo;
    return o;
}

static Json::Value movimientoToJson(const Movimiento& m) {
    Json::Value o;
    o["id_movimiento"] = m.id_movimiento;
    o["id_cuenta"]     = m.id_cuenta;
    o["nombre_cuenta"] = m.nombre_cuenta;
    o["codigo_cuenta"] = m.codigo_cuenta;
    o["fecha"]         = m.fecha;
    o["descripcion"]   = m.descripcion;
    o["monto"]         = m.monto;
    o["tipo"]          = m.tipo;
    o["medio_pago"]    = m.medio_pago;
    o["id_pago"]       = m.id_pago;
    o["es_manual"]     = (m.id_pago == 0);
    return o;
}

void ContabilidadController::pagina(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    callback(drogon::HttpResponse::newFileResponse("views/contabilidad.html"));
}

void ContabilidadController::listarCuentas(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    ContabilidadRepository::listarCuentas(
        [callback](std::vector<PlanCuenta> lista) {
            Json::Value arr(Json::arrayValue);
            for (const auto& c : lista) arr.append(cuentaToJson(c));
            callback(drogon::HttpResponse::newHttpJsonResponse(arr));
        },
        [callback](const std::string& e) {
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k500InternalServerError); callback(r);
        });
}

void ContabilidadController::crearCuenta(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    auto body = req->getJsonObject();
    if (!body || !(*body)["nombre"].isString() || !(*body)["codigo"].isString()) {
        Json::Value err; err["error"] = "Se requieren codigo, nombre y tipo";
        auto r = drogon::HttpResponse::newHttpJsonResponse(err);
        r->setStatusCode(drogon::k400BadRequest); callback(r); return;
    }
    PlanCuenta c;
    c.codigo = (*body)["codigo"].asString();
    c.nombre = (*body)["nombre"].asString();
    c.tipo   = (*body).get("tipo", "EGRESO").asString();

    ContabilidadRepository::insertarCuenta(c,
        [callback](int id) {
            Json::Value res; res["ok"] = true; res["id_cuenta"] = id;
            auto r = drogon::HttpResponse::newHttpJsonResponse(res);
            r->setStatusCode(drogon::k201Created); callback(r);
        },
        [callback](const std::string& e) {
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k500InternalServerError); callback(r);
        });
}

void ContabilidadController::actualizarCuenta(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id)
{
    auto body = req->getJsonObject();
    if (!body) {
        Json::Value err; err["error"] = "JSON inválido";
        auto r = drogon::HttpResponse::newHttpJsonResponse(err);
        r->setStatusCode(drogon::k400BadRequest); callback(r); return;
    }
    PlanCuenta c;
    c.id_cuenta = id;
    c.codigo    = (*body)["codigo"].asString();
    c.nombre    = (*body)["nombre"].asString();
    c.tipo      = (*body).get("tipo", "EGRESO").asString();

    ContabilidadRepository::actualizarCuenta(c,
        [callback]() {
            Json::Value res; res["ok"] = true;
            callback(drogon::HttpResponse::newHttpJsonResponse(res));
        },
        [callback](const std::string& e) {
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k500InternalServerError); callback(r);
        });
}

void ContabilidadController::toggleCuenta(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id)
{
    ContabilidadRepository::toggleActivoCuenta(id,
        [callback]() {
            Json::Value res; res["ok"] = true;
            callback(drogon::HttpResponse::newHttpJsonResponse(res));
        },
        [callback](const std::string& e) {
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k500InternalServerError); callback(r);
        });
}

void ContabilidadController::listarMovimientos(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    std::string tipo  = req->getParameter("tipo");
    std::string desde = req->getParameter("desde");
    std::string hasta = req->getParameter("hasta");

    ContabilidadRepository::listarMovimientos(tipo, desde, hasta,
        [callback](std::vector<Movimiento> lista) {
            Json::Value arr(Json::arrayValue);
            for (const auto& m : lista) arr.append(movimientoToJson(m));
            callback(drogon::HttpResponse::newHttpJsonResponse(arr));
        },
        [callback](const std::string& e) {
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k500InternalServerError); callback(r);
        });
}

void ContabilidadController::crearMovimiento(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    auto body = req->getJsonObject();
    if (!body || !(*body)["id_cuenta"].isInt() || !(*body)["monto"].isNumeric()) {
        Json::Value err; err["error"] = "Se requieren id_cuenta, monto, tipo y fecha";
        auto r = drogon::HttpResponse::newHttpJsonResponse(err);
        r->setStatusCode(drogon::k400BadRequest); callback(r); return;
    }
    Movimiento m;
    m.id_cuenta   = (*body)["id_cuenta"].asInt();
    m.fecha       = (*body).get("fecha", "").asString();
    m.descripcion = (*body).get("descripcion", "").asString();
    m.monto       = (*body)["monto"].asDouble();
    m.tipo        = (*body).get("tipo", "EGRESO").asString();
    m.medio_pago  = (*body).get("medio_pago", "EFECTIVO").asString();
    m.id_pago     = 0; // Si es manual, no tiene pago vinculado

    ContabilidadRepository::insertarMovimiento(m,
        [callback](int id) {
            Json::Value res; res["ok"] = true; res["id_movimiento"] = id;
            auto r = drogon::HttpResponse::newHttpJsonResponse(res);
            r->setStatusCode(drogon::k201Created); callback(r);
        },
        [callback](const std::string& e) {
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k500InternalServerError); callback(r);
        });
}

void ContabilidadController::eliminarMovimiento(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id)
{
    ContabilidadRepository::eliminarMovimiento(id,
        [callback]() {
            Json::Value res; res["ok"] = true;
            callback(drogon::HttpResponse::newHttpJsonResponse(res));
        },
        [callback](const std::string& e) {
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k500InternalServerError); callback(r);
        });
}

void ContabilidadController::resumen(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    int anio = 0;
    auto a = req->getParameter("anio");
    if (!a.empty()) anio = std::stoi(a);

    ContabilidadRepository::resumenPorCuenta(anio,
        [callback](const Json::Value& arr) {
            callback(drogon::HttpResponse::newHttpJsonResponse(arr));
        },
        [callback](const std::string& e) {
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k500InternalServerError); callback(r);
        });
}

void ContabilidadController::resumenMensual(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    int anio = 2026;
    auto a = req->getParameter("anio");
    if (!a.empty()) anio = std::stoi(a);

    ContabilidadRepository::resumenMensual(anio,
        [callback](const Json::Value& arr) {
            callback(drogon::HttpResponse::newHttpJsonResponse(arr));
        },
        [callback](const std::string& e) {
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k500InternalServerError); callback(r);
        });
}
