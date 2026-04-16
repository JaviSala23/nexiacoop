#include "RifaController.h"
#include <drogon/drogon.h>
#include "../repositories/RifaRepository.h"

static Json::Value rifaToJson(const Rifa& r) {
    Json::Value o;
    o["id_rifa"]           = r.id_rifa;
    o["nombre"]            = r.nombre;
    o["descripcion"]       = r.descripcion;
    o["fecha_sorteo"]      = r.fecha_sorteo;
    o["cantidad_numeros"]  = r.cantidad_numeros;
    o["cant_cuotas"]       = r.cant_cuotas;
    o["monto_total"]       = r.monto_total;
    o["monto_cuota"]       = r.monto_cuota;
    o["id_cuenta"]         = r.id_cuenta;
    o["nombre_cuenta"]     = r.nombre_cuenta;
    o["estado"]            = r.estado;
    o["fecha_creacion"]    = r.fecha_creacion;
    o["numeros_asignados"] = r.numeros_asignados;
    o["numeros_pagados"]   = r.numeros_pagados;
    o["total_recaudado"]   = r.total_recaudado;
    return o;
}

static Json::Value numeroToJson(const RifaNumero& n) {
    Json::Value o;
    o["id_numero"]              = n.id_numero;
    o["id_rifa"]                = n.id_rifa;
    o["numero"]                 = n.numero;
    o["id_familia"]             = n.id_familia;
    o["nombre_externo"]         = n.nombre_externo;
    o["telefono_externo"]       = n.telefono_externo;
    o["saldo_pendiente"]        = n.saldo_pendiente;
    o["estado"]                 = n.estado;
    o["fecha_asignacion"]       = n.fecha_asignacion;
    o["numero_familia"]         = n.numero_familia;
    o["nombre_tutor_principal"] = n.nombre_tutor_principal;
    o["telefono_familia"]       = n.telefono_familia;
    o["nombre_rifa"]            = n.nombre_rifa;
    o["cant_cuotas"]            = n.cant_cuotas;
    o["monto_total"]            = n.monto_total;
    o["monto_cuota"]            = n.monto_cuota;
    o["cuotas_pagadas"]         = n.cuotas_pagadas;
    o["cuotas_pendientes"]      = n.cuotas_pendientes;
    return o;
}

// ---------- Página HTML ----------
void RifaController::pagina(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    callback(drogon::HttpResponse::newFileResponse("views/rifas.html"));
}

// ---------- Listar rifas ----------
void RifaController::listar(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    RifaRepository::listar(
        [callback](std::vector<Rifa> lista) {
            Json::Value arr(Json::arrayValue);
            for (const auto& r : lista) arr.append(rifaToJson(r));
            callback(drogon::HttpResponse::newHttpJsonResponse(arr));
        },
        [callback](const std::string& e) {
            LOG_ERROR << e;
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k500InternalServerError); callback(r);
        });
}

// ---------- Crear rifa ----------
void RifaController::crear(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    auto body = req->getJsonObject();
    if (!body || !(*body)["nombre"].isString()) {
        Json::Value err; err["error"] = "Se requiere nombre";
        auto r = drogon::HttpResponse::newHttpJsonResponse(err);
        r->setStatusCode(drogon::k400BadRequest); callback(r); return;
    }
    Rifa rifa;
    rifa.nombre           = (*body)["nombre"].asString();
    rifa.descripcion      = (*body).get("descripcion","").asString();
    rifa.fecha_sorteo     = (*body).get("fecha_sorteo","").asString();
    rifa.cantidad_numeros = (*body).get("cantidad_numeros", 100).asInt();
    rifa.cant_cuotas      = (*body).get("cant_cuotas", 1).asInt();
    rifa.monto_total      = (*body).get("monto_total", 0.0).asDouble();
    rifa.monto_cuota      = (*body).get("monto_cuota", 0.0).asDouble();
    rifa.id_cuenta        = (*body).get("id_cuenta", 0).asInt();

    if (rifa.cantidad_numeros <= 0 || rifa.cant_cuotas <= 0) {
        Json::Value err; err["error"] = "cantidad_numeros y cant_cuotas deben ser mayores a 0";
        auto r = drogon::HttpResponse::newHttpJsonResponse(err);
        r->setStatusCode(drogon::k400BadRequest); callback(r); return;
    }

    RifaRepository::insertar(rifa,
        [callback](int id) {
            Json::Value res; res["ok"] = true; res["id_rifa"] = id;
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

// ---------- Actualizar rifa ----------
void RifaController::actualizar(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id)
{
    auto body = req->getJsonObject();
    if (!body) {
        Json::Value err; err["error"] = "JSON inválido";
        auto r = drogon::HttpResponse::newHttpJsonResponse(err);
        r->setStatusCode(drogon::k400BadRequest); callback(r); return;
    }
    Rifa rifa;
    rifa.id_rifa          = id;
    rifa.nombre           = (*body).get("nombre","").asString();
    rifa.descripcion      = (*body).get("descripcion","").asString();
    rifa.fecha_sorteo     = (*body).get("fecha_sorteo","").asString();
    rifa.cantidad_numeros = (*body).get("cantidad_numeros", 100).asInt();
    rifa.cant_cuotas      = (*body).get("cant_cuotas", 1).asInt();
    rifa.monto_total      = (*body).get("monto_total", 0.0).asDouble();
    rifa.monto_cuota      = (*body).get("monto_cuota", 0.0).asDouble();
    rifa.id_cuenta        = (*body).get("id_cuenta", 0).asInt();
    rifa.estado           = (*body).get("estado","ACTIVA").asString();

    RifaRepository::actualizar(rifa,
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

// ---------- Listar números de una rifa ----------
void RifaController::listarNumeros(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id)
{
    RifaRepository::listarNumeros(id,
        [callback](std::vector<RifaNumero> lista) {
            Json::Value arr(Json::arrayValue);
            for (const auto& n : lista) arr.append(numeroToJson(n));
            callback(drogon::HttpResponse::newHttpJsonResponse(arr));
        },
        [callback](const std::string& e) {
            LOG_ERROR << e;
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k500InternalServerError); callback(r);
        });
}

// ---------- Números disponibles ----------
void RifaController::disponibles(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id)
{
    RifaRepository::numerosDisponibles(id,
        [callback](Json::Value arr) {
            callback(drogon::HttpResponse::newHttpJsonResponse(arr));
        },
        [callback](const std::string& e) {
            LOG_ERROR << e;
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k500InternalServerError); callback(r);
        });
}

// ---------- Asignar número a familia o externo ----------
void RifaController::asignarNumero(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id)
{
    auto body = req->getJsonObject();
    if (!body || !(*body)["numero"].isInt()) {
        Json::Value err; err["error"] = "Se requiere numero";
        auto r = drogon::HttpResponse::newHttpJsonResponse(err);
        r->setStatusCode(drogon::k400BadRequest); callback(r); return;
    }

    // Obtener datos de la rifa primero
    RifaRepository::obtener(id,
        [body, id, callback](const Rifa& rifa) {
            RifaNumero rn;
            rn.id_rifa          = id;
            rn.numero           = (*body)["numero"].asInt();
            rn.id_familia       = (*body).get("id_familia", 0).asInt();
            rn.nombre_externo   = (*body).get("nombre_externo", "").asString();
            rn.telefono_externo = (*body).get("telefono_externo", "").asString();
            rn.cant_cuotas      = rifa.cant_cuotas;
            rn.monto_cuota      = rifa.monto_cuota;
            rn.saldo_pendiente  = rifa.monto_total;

            if (rn.id_familia == 0 && rn.nombre_externo.empty()) {
                Json::Value err; err["error"] = "Debe indicar id_familia o nombre_externo";
                auto r = drogon::HttpResponse::newHttpJsonResponse(err);
                r->setStatusCode(drogon::k400BadRequest); callback(r); return;
            }

            RifaRepository::asignarNumero(rn,
                [callback](int idNum) {
                    Json::Value res; res["ok"] = true; res["id_numero"] = idNum;
                    auto r = drogon::HttpResponse::newHttpJsonResponse(res);
                    r->setStatusCode(drogon::k201Created); callback(r);
                },
                [callback](const std::string& e) {
                    LOG_ERROR << e;
                    Json::Value err; err["error"] = e;
                    auto r = drogon::HttpResponse::newHttpJsonResponse(err);
                    r->setStatusCode(drogon::k400BadRequest); callback(r);
                });
        },
        [callback](const std::string& e) {
            LOG_ERROR << e;
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k404NotFound); callback(r);
        });
}

// ---------- Asignar a todas las familias ----------
void RifaController::asignarATodas(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id)
{
    RifaRepository::asignarATodas(id,
        [callback](int cant) {
            Json::Value res; res["ok"] = true; res["asignados"] = cant;
            res["mensaje"] = "Se asignaron " + std::to_string(cant) + " números a familias activas";
            callback(drogon::HttpResponse::newHttpJsonResponse(res));
        },
        [callback](const std::string& e) {
            LOG_ERROR << e;
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k500InternalServerError); callback(r);
        });
}

// ---------- Anular número ----------
void RifaController::anularNumero(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id)
{
    RifaRepository::anularNumero(id,
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

// ---------- Listar cuotas de un número ----------
void RifaController::listarCuotas(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id)
{
    RifaRepository::listarCuotas(id,
        [callback](Json::Value arr) {
            callback(drogon::HttpResponse::newHttpJsonResponse(arr));
        },
        [callback](const std::string& e) {
            LOG_ERROR << e;
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k500InternalServerError); callback(r);
        });
}

// ---------- Pagar cuotas ----------
void RifaController::pagarCuotas(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    auto body = req->getJsonObject();
    if (!body || !(*body)["cuotas"].isArray() || (*body)["cuotas"].empty()) {
        Json::Value err; err["error"] = "Se requiere array de cuotas";
        auto r = drogon::HttpResponse::newHttpJsonResponse(err);
        r->setStatusCode(drogon::k400BadRequest); callback(r); return;
    }
    std::vector<int> ids;
    for (const auto& v : (*body)["cuotas"])
        if (v.isInt()) ids.push_back(v.asInt());
    if (ids.empty()) {
        Json::Value err; err["error"] = "IDs de cuotas inválidos";
        auto r = drogon::HttpResponse::newHttpJsonResponse(err);
        r->setStatusCode(drogon::k400BadRequest); callback(r); return;
    }
    std::string medio = (*body).get("medio_pago","EFECTIVO").asString();

    RifaRepository::pagarCuotas(ids, medio,
        [callback](int idMov) {
            Json::Value res; res["ok"] = true;
            if (idMov > 0) res["id_movimiento"] = idMov;
            callback(drogon::HttpResponse::newHttpJsonResponse(res));
        },
        [callback](const std::string& e) {
            LOG_ERROR << e;
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k400BadRequest); callback(r);
        });
}
