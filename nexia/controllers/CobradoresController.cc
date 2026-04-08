#include "CobradoresController.h"
#include <drogon/drogon.h>
#include "../repositories/CobradoresRepository.h"

static Json::Value cobToJson(const Cobradora& c) {
    Json::Value o;
    o["id_cobradora"]        = c.id_cobradora;
    o["nombre"]              = c.nombre;
    o["telefono"]            = c.telefono;
    o["porcentaje_comision"] = c.porcentaje_comision;
    o["activa"]              = c.activa;
    return o;
}

void CobradoresController::pagina(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    callback(drogon::HttpResponse::newFileResponse("views/cobradoras.html"));
}

void CobradoresController::listar(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    CobradoresRepository::listar(
        [callback](std::vector<Cobradora> lista) {
            Json::Value arr(Json::arrayValue);
            for (const auto& c : lista) arr.append(cobToJson(c));
            callback(drogon::HttpResponse::newHttpJsonResponse(arr));
        },
        [callback](const std::string& e) {
            LOG_ERROR << e;
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k500InternalServerError); callback(r);
        });
}

void CobradoresController::obtener(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id)
{
    CobradoresRepository::buscarPorId(id,
        [callback](const Cobradora& c) {
            callback(drogon::HttpResponse::newHttpJsonResponse(cobToJson(c)));
        },
        [callback](const std::string& e) {
            LOG_ERROR << e;
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k404NotFound); callback(r);
        });
}

void CobradoresController::crear(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    auto body = req->getJsonObject();
    if (!body) {
        Json::Value err; err["error"] = "JSON inválido";
        auto r = drogon::HttpResponse::newHttpJsonResponse(err);
        r->setStatusCode(drogon::k400BadRequest); callback(r); return;
    }
    Cobradora c;
    c.nombre              = (*body)["nombre"].asString();
    c.telefono            = (*body).get("telefono","").asString();
    c.porcentaje_comision = (*body).get("porcentaje_comision", 0.0).asDouble();
    c.activa              = (*body).get("activa", true).asBool();
    CobradoresRepository::insertar(c,
        [callback](int id) {
            Json::Value res; res["id_cobradora"] = id; res["ok"] = true;
            auto r = drogon::HttpResponse::newHttpJsonResponse(res);
            r->setStatusCode(drogon::k201Created); callback(r);
        },
        [callback](const std::string& e) {
            LOG_ERROR << e;
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k500InternalServerError); callback(r);
        });
}

void CobradoresController::actualizar(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id)
{
    auto body = req->getJsonObject();
    if (!body) {
        Json::Value err; err["error"] = "JSON inválido";
        auto r = drogon::HttpResponse::newHttpJsonResponse(err);
        r->setStatusCode(drogon::k400BadRequest); callback(r); return;
    }
    Cobradora c;
    c.id_cobradora        = id;
    c.nombre              = (*body)["nombre"].asString();
    c.telefono            = (*body).get("telefono","").asString();
    c.porcentaje_comision = (*body).get("porcentaje_comision", 0.0).asDouble();
    c.activa              = (*body).get("activa", true).asBool();
    CobradoresRepository::actualizar(c,
        [callback]() {
            Json::Value res; res["ok"] = true;
            callback(drogon::HttpResponse::newHttpJsonResponse(res));
        },
        [callback](const std::string& e) {
            LOG_ERROR << e;
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k500InternalServerError); callback(r);
        });
}

void CobradoresController::eliminar(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id)
{
    CobradoresRepository::eliminar(id,
        [callback]() {
            Json::Value res; res["ok"] = true;
            callback(drogon::HttpResponse::newHttpJsonResponse(res));
        },
        [callback](const std::string& e) {
            LOG_ERROR << e;
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k500InternalServerError); callback(r);
        });
}
