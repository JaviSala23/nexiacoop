#include "TalonController.h"
#include <drogon/drogon.h>
#include "../repositories/TalonRepository.h"
#include "../services/TalonService.h"

static Json::Value talonToJson(const Talon& t) {
    Json::Value o;
    o["id_talon"]              = t.id_talon;
    o["id_familia"]            = t.id_familia;
    o["id_concepto"]           = t.id_concepto;
    o["numero_familia"]        = t.numero_familia;
    o["nombre_concepto"]       = t.nombre_concepto;
    o["tipo_concepto"]         = t.tipo_concepto;
    o["nombre_tutor_principal"]= t.nombre_tutor_principal;
    o["dni_tutor_principal"]   = t.dni_tutor_principal;
    o["telefono_familia"]      = t.telefono_familia;
    o["direccion_familia"]     = t.direccion_familia;
    o["cantidad_alumnos"]      = t.cantidad_alumnos;
    o["mes"]                   = t.mes;
    o["anio"]                  = t.anio;
    o["monto"]                 = t.monto;
    o["codigo_barra"]          = t.codigo_barra;
    o["estado"]                = t.estado;
    o["fecha_generacion"]      = t.fecha_generacion;
    o["fecha_pago"]            = t.fecha_pago;
    o["observaciones"]         = t.observaciones;
    return o;
}

void TalonController::pagina(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    callback(drogon::HttpResponse::newFileResponse("views/talones.html"));
}

void TalonController::listar(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    int mes = 0, anio = 0, idConcepto = 0;
    auto m = req->getParameter("mes");       if (!m.empty()) mes = std::stoi(m);
    auto a = req->getParameter("anio");      if (!a.empty()) anio = std::stoi(a);
    auto c = req->getParameter("id_concepto"); if (!c.empty()) idConcepto = std::stoi(c);
    bool soloExtras = req->getParameter("extras") == "1";

    TalonRepository::listar(mes, anio, idConcepto, soloExtras,
        [callback](std::vector<Talon> lista) {
            Json::Value arr(Json::arrayValue);
            for (const auto& t : lista) arr.append(talonToJson(t));
            callback(drogon::HttpResponse::newHttpJsonResponse(arr));
        },
        [callback](const std::string& e) {
            LOG_ERROR << e;
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k500InternalServerError); callback(r);
        });
}

void TalonController::generar(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    auto body = req->getJsonObject();
    if (!body || !(*body)["mes"].isInt() || !(*body)["anio"].isInt()) {
        Json::Value err; err["error"] = "Se requieren mes y anio";
        auto r = drogon::HttpResponse::newHttpJsonResponse(err);
        r->setStatusCode(drogon::k400BadRequest); callback(r); return;
    }
    int mes  = (*body)["mes"].asInt();
    int anio = (*body)["anio"].asInt();

    // Opcional: lista de id_familia específicas
    std::vector<int> familiaIds;
    if (body->isMember("familias") && (*body)["familias"].isArray()) {
        for (const auto& v : (*body)["familias"])
            if (v.isInt()) familiaIds.push_back(v.asInt());
    }

    auto okCb = [callback](int cant) {
        Json::Value res;
        res["ok"] = true; res["generados"] = cant;
        res["mensaje"] = "Se generaron " + std::to_string(cant) + " talones correctamente";
        callback(drogon::HttpResponse::newHttpJsonResponse(res));
    };
    auto errCb = [callback](const std::string& e) {
        LOG_ERROR << e;
        Json::Value err; err["error"] = e;
        auto r = drogon::HttpResponse::newHttpJsonResponse(err);
        r->setStatusCode(drogon::k400BadRequest); callback(r);
    };

    if (!familiaIds.empty())
        TalonService::generarParaFamilias(mes, anio, std::move(familiaIds), okCb, errCb);
    else
        TalonService::generarMensual(mes, anio, okCb, errCb);
}

void TalonController::generarExtra(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    auto body = req->getJsonObject();
    if (!body || !(*body)["id_familia"].isInt() || !(*body)["id_concepto"].isInt()) {
        Json::Value err; err["error"] = "Se requieren id_familia, id_concepto y monto";
        auto r = drogon::HttpResponse::newHttpJsonResponse(err);
        r->setStatusCode(drogon::k400BadRequest); callback(r); return;
    }
    int    idFamilia  = (*body)["id_familia"].asInt();
    int    idConcepto = (*body)["id_concepto"].asInt();
    double monto      = (*body).get("monto", 0.0).asDouble();
    std::string obs   = (*body).get("observaciones", "").asString();

    TalonService::generarExtra(idFamilia, idConcepto, monto, obs,
        [callback](int id) {
            Json::Value res; res["ok"] = true; res["id_talon"] = id;
            auto r = drogon::HttpResponse::newHttpJsonResponse(res);
            r->setStatusCode(drogon::k201Created); callback(r);
        },
        [callback](const std::string& e) {
            LOG_ERROR << e;
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k400BadRequest); callback(r);
        });
}

void TalonController::anular(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id)
{
    TalonRepository::anular(id,
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
