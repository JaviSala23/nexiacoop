#include "TalonController.h"
#include <drogon/drogon.h>
#include "../repositories/TalonRepository.h"
#include "../services/TalonService.h"

void TalonController::pagina(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    callback(drogon::HttpResponse::newFileResponse("views/talones.html"));
}

void TalonController::listar(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    int mes = 0, anio = 0, cobradora = 0;
    auto m = req->getParameter("mes");    if (!m.empty()) mes = std::stoi(m);
    auto a = req->getParameter("anio");   if (!a.empty()) anio = std::stoi(a);
    auto c = req->getParameter("cobradora"); if (!c.empty()) cobradora = std::stoi(c);

    TalonRepository::listar(mes, anio, cobradora,
        [callback](std::vector<Talon> lista) {
            Json::Value arr(Json::arrayValue);
            for (const auto& t : lista) {
                Json::Value o;
                o["id_talon"]       = t.id_talon;
                o["id_socio"]       = t.id_socio;
                o["id_cobradora"]   = t.id_cobradora;
                o["nombre_socio"]   = t.nombre_socio;
                o["nombre_cobradora"] = t.nombre_cobradora;
                o["direccion_socio"]  = t.direccion_socio;
                o["telefono_socio"]   = t.telefono_socio;
                o["dni_socio"]        = t.dni_socio;
                o["localidad_socio"]  = t.localidad_socio;
                o["provincia_socio"]  = t.provincia_socio;
                o["nombre_responsable_socio"] = t.nombre_responsable_socio;
                o["nombre_anexo"]        = t.nombre_anexo;
                o["dni_anexo"]           = t.dni_anexo;
                o["numero_socio_anexo"]  = t.numero_socio_anexo;
                o["telefono_anexo"]      = t.telefono_anexo;
                o["mes"]            = t.mes;
                o["anio"]           = t.anio;
                o["monto"]          = t.monto;
                o["tipo"]           = t.tipo;
                o["codigo_barra"]   = t.codigo_barra;
                o["estado"]         = t.estado;
                o["fecha_generacion"]= t.fecha_generacion;
                o["fecha_pago"]     = t.fecha_pago;
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

    // Opcional: lista de id_socio específicos
    std::vector<int> socioIds;
    if (body->isMember("socios") && (*body)["socios"].isArray()) {
        for (const auto& v : (*body)["socios"])
            if (v.isInt()) socioIds.push_back(v.asInt());
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

    if (!socioIds.empty())
        TalonService::generarParaSocios(mes, anio, std::move(socioIds), okCb, errCb);
    else
        TalonService::generarMensual(mes, anio, okCb, errCb);
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
