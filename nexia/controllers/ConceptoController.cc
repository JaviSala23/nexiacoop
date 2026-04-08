#include "ConceptoController.h"
#include <drogon/drogon.h>
#include "../repositories/ConceptoRepository.h"

void ConceptoController::pagina(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    callback(drogon::HttpResponse::newFileResponse("views/conceptos.html"));
}

void ConceptoController::listar(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    ConceptoRepository::listar(
        [callback](std::vector<Concepto> lista) {
            Json::Value arr(Json::arrayValue);
            for (const auto& c : lista) {
                Json::Value o;
                o["id_concepto"]  = c.id_concepto;
                o["nombre"]       = c.nombre;
                o["descripcion"]  = c.descripcion;
                o["tipo"]         = c.tipo;
                o["activo"]       = c.activo;
                arr.append(o);
            }
            callback(drogon::HttpResponse::newHttpJsonResponse(arr));
        },
        [callback](const std::string& e) {
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k500InternalServerError); callback(r);
        });
}

void ConceptoController::crear(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    auto body = req->getJsonObject();
    if (!body || !(*body)["nombre"].isString()) {
        Json::Value err; err["error"] = "Se requiere nombre";
        auto r = drogon::HttpResponse::newHttpJsonResponse(err);
        r->setStatusCode(drogon::k400BadRequest); callback(r); return;
    }
    Concepto c;
    c.nombre      = (*body)["nombre"].asString();
    c.descripcion = (*body).get("descripcion", "").asString();
    c.tipo        = (*body).get("tipo", "EXTRA").asString();

    ConceptoRepository::insertar(c,
        [callback](int id) {
            Json::Value res; res["ok"] = true; res["id_concepto"] = id;
            auto r = drogon::HttpResponse::newHttpJsonResponse(res);
            r->setStatusCode(drogon::k201Created); callback(r);
        },
        [callback](const std::string& e) {
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k500InternalServerError); callback(r);
        });
}

void ConceptoController::actualizar(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id)
{
    auto body = req->getJsonObject();
    if (!body) {
        Json::Value err; err["error"] = "JSON inválido";
        auto r = drogon::HttpResponse::newHttpJsonResponse(err);
        r->setStatusCode(drogon::k400BadRequest); callback(r); return;
    }
    Concepto c;
    c.id_concepto = id;
    c.nombre      = (*body)["nombre"].asString();
    c.descripcion = (*body).get("descripcion", "").asString();
    c.tipo        = (*body).get("tipo", "EXTRA").asString();

    ConceptoRepository::actualizar(c,
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

void ConceptoController::toggle(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id)
{
    ConceptoRepository::toggleActivo(id,
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

// -------------------------------------------------------
// Cuotas Config
// -------------------------------------------------------
void ConceptoController::listarCuotas(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    ConceptoRepository::listarCuotasConfig(
        [callback](std::vector<CuotaConfig> lista) {
            Json::Value arr(Json::arrayValue);
            for (const auto& cc : lista) {
                Json::Value o;
                o["id_config"]      = cc.id_config;
                o["cantidad_hijos"] = cc.cantidad_hijos;
                o["monto"]          = cc.monto;
                o["activo"]         = cc.activo;
                arr.append(o);
            }
            callback(drogon::HttpResponse::newHttpJsonResponse(arr));
        },
        [callback](const std::string& e) {
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k500InternalServerError); callback(r);
        });
}

void ConceptoController::upsertCuota(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    auto body = req->getJsonObject();
    if (!body || !(*body)["cantidad_hijos"].isInt() || !(*body)["monto"].isNumeric()) {
        Json::Value err; err["error"] = "Se requieren cantidad_hijos y monto";
        auto r = drogon::HttpResponse::newHttpJsonResponse(err);
        r->setStatusCode(drogon::k400BadRequest); callback(r); return;
    }
    int cantHijos = (*body)["cantidad_hijos"].asInt();
    double monto  = (*body)["monto"].asDouble();

    ConceptoRepository::upsertCuotaConfig(cantHijos, monto,
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

void ConceptoController::eliminarCuota(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id)
{
    ConceptoRepository::eliminarCuotaConfig(id,
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
