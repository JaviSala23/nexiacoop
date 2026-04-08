#include "PagoController.h"
#include <drogon/drogon.h>
#include "../repositories/PagoRepository.h"
#include "../repositories/TalonRepository.h"
#include "../services/PagoService.h"

void PagoController::pagina(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    callback(drogon::HttpResponse::newFileResponse("views/pagos.html"));
}

void PagoController::registrar(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    auto body = req->getJsonObject();
    if (!body || !(*body)["codigo_barra"].isString()) {
        Json::Value err; err["error"] = "Se requiere codigo_barra";
        auto r = drogon::HttpResponse::newHttpJsonResponse(err);
        r->setStatusCode(drogon::k400BadRequest); callback(r); return;
    }
    std::string codigo = (*body)["codigo_barra"].asString();
    int idCobradora    = (*body).get("id_cobradora", 0).asInt();
    std::string obs    = (*body).get("observaciones","").asString();

    PagoService::registrarPorCodigo(codigo, idCobradora, obs,
        [callback](const Json::Value& res) {
            callback(drogon::HttpResponse::newHttpJsonResponse(res));
        },
        [callback](const std::string& e) {
            LOG_ERROR << e;
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k400BadRequest); callback(r);
        });
}

void PagoController::listar(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    int mes = 0, anio = 0;
    auto m = req->getParameter("mes");  if (!m.empty()) mes = std::stoi(m);
    auto a = req->getParameter("anio"); if (!a.empty()) anio = std::stoi(a);

    PagoRepository::listar(mes, anio,
        [callback](std::vector<Pago> lista) {
            Json::Value arr(Json::arrayValue);
            for (const auto& p : lista) {
                Json::Value o;
                o["id_pago"]         = p.id_pago;
                o["numero_socio"]    = p.numero_socio;
                o["nombre_socio"]    = p.nombre_socio;
                o["nombre_cobradora"]= p.nombre_cobradora;
                o["fecha_pago"]      = p.fecha_pago;
                o["mes"]             = p.mes;
                o["anio"]            = p.anio;
                o["codigo_barra"]    = p.codigo_barra;
                o["monto"]           = p.monto;
                o["observaciones"]   = p.observaciones;
                arr.append(o);
            }
            callback(drogon::HttpResponse::newHttpJsonResponse(arr));
        },
        [callback](const std::string& e) {
            LOG_ERROR << e;
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k500InternalServerError); callback(r);
        });
}

void PagoController::morosos(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    TalonRepository::listarMorosos(
        [callback](const Json::Value& arr) {
            callback(drogon::HttpResponse::newHttpJsonResponse(arr));
        },
        [callback](const std::string& e) {
            LOG_ERROR << e;
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k500InternalServerError); callback(r);
        });
}

void PagoController::resumenMes(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    int anio = 0;
    auto a = req->getParameter("anio"); if (!a.empty()) anio = std::stoi(a);
    PagoRepository::resumenMes(anio,
        [callback](const Json::Value& arr) {
            callback(drogon::HttpResponse::newHttpJsonResponse(arr));
        },
        [callback](const std::string& e) {
            LOG_ERROR << e;
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k500InternalServerError); callback(r);
        });
}

void PagoController::resumenCobradora(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    int mes = 0, anio = 0;
    auto m = req->getParameter("mes");  if (!m.empty()) mes  = std::stoi(m);
    auto a = req->getParameter("anio"); if (!a.empty()) anio = std::stoi(a);
    PagoRepository::resumenCobradora(mes, anio,
        [callback](const Json::Value& arr) {
            callback(drogon::HttpResponse::newHttpJsonResponse(arr));
        },
        [callback](const std::string& e) {
            LOG_ERROR << e;
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k500InternalServerError); callback(r);
        });
}
