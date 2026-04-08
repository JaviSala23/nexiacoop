#include "SocioController.h"
#include <drogon/drogon.h>
#include "../repositories/SocioRepository.h"
#include "../models/Responsable.h"

static std::string friendlyError(const std::string& raw) {
    if (raw.find("El número de socio ya existe") != std::string::npos ||
        raw.find("El DNI ya existe") != std::string::npos ||
        raw.find("ya existe") != std::string::npos)
        return raw;
    if (raw.find("Duplicate entry") != std::string::npos) {
        if (raw.find("numero_socio") != std::string::npos)
            return "El número de socio ya está registrado.";
        if (raw.find("dni") != std::string::npos)
            return "El DNI ya está registrado.";
        return "Ya existe un registro con esos datos.";
    }
    if (raw.find("Unknown column") != std::string::npos ||
        raw.find("doesn't exist") != std::string::npos ||
        raw.find("Table") != std::string::npos)
        return "Error en la base de datos. Contacte al administrador.";
    if (raw.find("Socio no encontrado") != std::string::npos)
        return "Socio no encontrado.";
    return "Ocurrió un error inesperado. Intente nuevamente.";
}

static Json::Value socioToJson(const Socio& s) {
    Json::Value o;
    o["id_socio"]            = s.id_socio;
    o["numero_socio"]        = s.numero_socio;
    o["nombre_completo"]     = s.nombre_completo;
    o["dni"]                 = s.dni;
    o["direccion"]           = s.direccion;
    o["telefono"]            = s.telefono;
    o["id_localidad"]        = s.id_localidad;
    o["id_cobradora"]        = s.id_cobradora;
    o["id_socio_principal"]  = s.id_socio_principal;
    o["estado"]              = s.estado;
    o["fecha_alta"]          = s.fecha_alta;
    o["fecha_baja"]          = s.fecha_baja;
    o["tiene_responsable"]   = s.tiene_responsable;
    o["nombre_cobradora"]    = s.nombre_cobradora;
    o["nombre_localidad"]    = s.nombre_localidad;
    o["nombre_provincia"]    = s.nombre_provincia;
    return o;
}

void SocioController::pagina(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    callback(drogon::HttpResponse::newFileResponse("views/socios.html"));
}

void SocioController::padron(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    callback(drogon::HttpResponse::newFileResponse("views/padrones.html"));
}

void SocioController::listar(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    std::string estado = req->getParameter("estado");
    int cobradora = 0;
    auto cob = req->getParameter("cobradora");
    if (!cob.empty()) cobradora = std::stoi(cob);

    SocioRepository::listar(estado, cobradora,
        [callback](std::vector<Socio> lista) {
            Json::Value arr(Json::arrayValue);
            for (const auto& s : lista) arr.append(socioToJson(s));
            callback(drogon::HttpResponse::newHttpJsonResponse(arr));
        },
        [callback](const std::string& e) {
            LOG_ERROR << e;
            Json::Value err; err["error"] = friendlyError(e);
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k500InternalServerError);
            callback(r);
        });
}

void SocioController::obtener(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id)
{
    SocioRepository::buscarPorId(id,
        [callback](const Socio& s) {
            callback(drogon::HttpResponse::newHttpJsonResponse(socioToJson(s)));
        },
        [callback](const std::string& e) {
            LOG_ERROR << e;
            Json::Value err; err["error"] = friendlyError(e);
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k404NotFound);
            callback(r);
        });
}

void SocioController::crear(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    auto body = req->getJsonObject();
    if (!body) {
        Json::Value err; err["error"] = "JSON inválido";
        auto r = drogon::HttpResponse::newHttpJsonResponse(err);
        r->setStatusCode(drogon::k400BadRequest); callback(r); return;
    }
    Socio s;
    s.numero_socio      = (*body)["numero_socio"].asInt();
    s.nombre_completo   = (*body)["nombre_completo"].asString();
    s.dni               = (*body)["dni"].asString();
    s.direccion         = (*body)["direccion"].asString();
    s.telefono          = (*body).get("telefono", "").asString();
    s.id_localidad      = (*body).get("id_localidad", 0).asInt();
    s.id_cobradora      = (*body)["id_cobradora"].asInt();
    s.estado            = (*body).get("estado", "ACTIVO").asString();
    s.fecha_alta        = (*body)["fecha_alta"].asString();
    s.tiene_responsable = (*body).get("tiene_responsable", false).asBool();

    auto okCb = [callback](int id) {
        Json::Value res; res["id_socio"] = id; res["ok"] = true;
        auto r = drogon::HttpResponse::newHttpJsonResponse(res);
        r->setStatusCode(drogon::k201Created); callback(r);
    };
    auto okVoidCb = [callback]() {
        Json::Value res; res["ok"] = true;
        auto r = drogon::HttpResponse::newHttpJsonResponse(res);
        r->setStatusCode(drogon::k200OK); callback(r);
    };
    auto errCb = [callback](const std::string& e) {
        LOG_ERROR << e;
            Json::Value err; err["error"] = friendlyError(e);
        auto r = drogon::HttpResponse::newHttpJsonResponse(err);
        r->setStatusCode(drogon::k500InternalServerError); callback(r);
    };

    // Validar principal
    SocioRepository::existeNumeroODni(s.numero_socio, s.dni, 0, [body, s, okCb, okVoidCb, errCb](bool existe, std::string msg) {
        if (existe) { errCb(msg); return; }
        if (s.tiene_responsable && body->isMember("responsable")) {
            const auto& rj = (*body)["responsable"];
            Socio dependiente;
            dependiente.numero_socio    = rj.get("numero_socio", 0).asInt();
            dependiente.nombre_completo = rj.get("nombre_completo", "").asString();
            dependiente.dni             = rj.get("dni", "").asString();
            dependiente.direccion       = rj.get("direccion", "").asString();
            dependiente.telefono        = rj.get("telefono", "").asString();
            dependiente.id_localidad    = rj.get("id_localidad", 0).asInt();
            dependiente.id_cobradora    = rj.get("id_cobradora", 0).asInt();
            dependiente.estado          = rj.get("estado", "ACTIVO").asString();
            dependiente.fecha_alta      = rj.get("fecha_alta", "").asString();
            dependiente.tiene_responsable = false;
            dependiente.id_socio_principal = 0; // temporal
            // Si no se especifica dirección, hereda la del principal
            if (dependiente.direccion.empty()) dependiente.direccion = (*body)["direccion"].asString();
            if (dependiente.id_localidad == 0) dependiente.id_localidad = (*body)["id_localidad"].asInt();
            if (dependiente.id_cobradora == 0) dependiente.id_cobradora = (*body)["id_cobradora"].asInt();
            if (dependiente.fecha_alta.empty()) dependiente.fecha_alta = (*body)["fecha_alta"].asString();
            // Validar anexo
            SocioRepository::existeNumeroODni(dependiente.numero_socio, dependiente.dni, 0, [body, s, dependiente, okCb, okVoidCb, errCb](bool existe2, std::string msg2) mutable {
                if (existe2) { errCb(msg2); return; }
                // Insertar principal
                SocioRepository::insertar(s, [body, dependiente, okCb, okVoidCb, errCb](int idPrincipal) mutable {
                    Socio dep = dependiente;
                    dep.id_socio_principal = idPrincipal;
                    SocioRepository::insertar(dep, [okCb](int /*idDep*/) { okCb(1); }, errCb);
                }, errCb);
            });
        } else {
            SocioRepository::insertar(s, okCb, errCb);
        }
    });
}

void SocioController::actualizar(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id)
{
    auto body = req->getJsonObject();
    if (!body) {
        Json::Value err; err["error"] = "JSON inválido";
        auto r = drogon::HttpResponse::newHttpJsonResponse(err);
        r->setStatusCode(drogon::k400BadRequest); callback(r); return;
    }
    Socio s;
    s.id_socio          = id;
    s.nombre_completo   = (*body)["nombre_completo"].asString();
    s.dni               = (*body)["dni"].asString();
    s.direccion         = (*body)["direccion"].asString();
    s.telefono          = (*body).get("telefono", "").asString();
    s.id_localidad      = (*body).get("id_localidad", 0).asInt();
    s.id_cobradora      = (*body)["id_cobradora"].asInt();
    s.estado            = (*body)["estado"].asString();
    s.tiene_responsable = (*body).get("tiene_responsable", false).asBool();

    SocioRepository::existeNumeroODni(s.numero_socio, s.dni, s.id_socio, [body, s, callback](bool existe, std::string msg) {
        if (existe) {
            Json::Value err; err["error"] = msg;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k400BadRequest); callback(r); return;
        }
        // Si hay anexo, validar y actualizar/crear el anexo también
        if (s.tiene_responsable && body->isMember("responsable")) {
            const auto& rj = (*body)["responsable"];
            Socio dependiente;
            dependiente.numero_socio    = rj.get("numero_socio", 0).asInt();
            dependiente.nombre_completo = rj.get("nombre_completo", "").asString();
            dependiente.dni             = rj.get("dni", "").asString();
            dependiente.telefono        = rj.get("telefono", "").asString();
            dependiente.estado          = rj.get("estado", "ACTIVO").asString();
            dependiente.fecha_alta      = rj.get("fecha_alta", "").asString();
            dependiente.tiene_responsable = false;
            dependiente.id_socio_principal = s.id_socio;
            // Hereda dirección, localidad y cobradora del principal
            dependiente.direccion     = s.direccion;
            dependiente.id_localidad  = s.id_localidad;
            dependiente.id_cobradora  = s.id_cobradora;
            // Buscar si ya existe un anexo para este principal
            auto db = drogon::app().getDbClient("main");
            db->execSqlAsync(
                "SELECT id_socio FROM socios WHERE id_socio_principal=?",
                [s, dependiente, callback, body](const drogon::orm::Result& r) mutable {
                    int idAnexo = r.empty() ? 0 : r[0]["id_socio"].as<int>();
                    // Validar número y dni únicos (excluyendo el anexo si existe)
                    SocioRepository::existeNumeroODni(dependiente.numero_socio, dependiente.dni, idAnexo, [idAnexo, dependiente, callback](bool existe2, std::string msg2) mutable {
                        if (existe2) {
                            Json::Value err; err["error"] = msg2;
                            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
                            r->setStatusCode(drogon::k400BadRequest); callback(r); return;
                        }
                        if (idAnexo) {
                            // Actualizar anexo
                            Socio dep = dependiente;
                            dep.id_socio = idAnexo;
                            SocioRepository::actualizar(dep,
                                [callback]() {
                                    Json::Value res; res["ok"] = true;
                                    callback(drogon::HttpResponse::newHttpJsonResponse(res));
                                },
                                [callback](const std::string& e) {
                                    LOG_ERROR << e;
                                    Json::Value err; err["error"] = friendlyError(e);
                                    auto r = drogon::HttpResponse::newHttpJsonResponse(err);
                                    r->setStatusCode(drogon::k500InternalServerError); callback(r);
                                });
                        } else {
                            // Crear anexo
                            SocioRepository::insertar(dependiente,
                                [callback](int /*idDep*/) {
                                    Json::Value res; res["ok"] = true;
                                    callback(drogon::HttpResponse::newHttpJsonResponse(res));
                                },
                                [callback](const std::string& e) {
                                    LOG_ERROR << e;
                                    Json::Value err; err["error"] = friendlyError(e);
                                    auto r = drogon::HttpResponse::newHttpJsonResponse(err);
                                    r->setStatusCode(drogon::k500InternalServerError); callback(r);
                                });
                        }
                    });
                },
                [callback](const drogon::orm::DrogonDbException& e) {
                    Json::Value err; err["error"] = friendlyError(e.base().what());
                    auto r = drogon::HttpResponse::newHttpJsonResponse(err);
                    r->setStatusCode(drogon::k500InternalServerError); callback(r);
                },
                s.id_socio
            );
            // Actualizar principal
            SocioRepository::actualizar(s, [](){}, [](const std::string&){});
        } else {
            SocioRepository::actualizar(s,
                [callback]() {
                    Json::Value res; res["ok"] = true;
                    callback(drogon::HttpResponse::newHttpJsonResponse(res));
                },
                [callback](const std::string& e) {
                    LOG_ERROR << e;
                    Json::Value err; err["error"] = friendlyError(e);
                    auto r = drogon::HttpResponse::newHttpJsonResponse(err);
                    r->setStatusCode(drogon::k500InternalServerError); callback(r);
                });
        }
    });
}

void SocioController::cambiarEstado(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id)
{
    auto body = req->getJsonObject();
    if (!body || !(*body)["estado"].isString()) {
        Json::Value err; err["error"] = "Falta el campo estado";
        auto r = drogon::HttpResponse::newHttpJsonResponse(err);
        r->setStatusCode(drogon::k400BadRequest); callback(r); return;
    }
    std::string estado = (*body)["estado"].asString();
    SocioRepository::cambiarEstado(id, estado,
        [callback]() {
            Json::Value res; res["ok"] = true;
            callback(drogon::HttpResponse::newHttpJsonResponse(res));
        },
        [callback](const std::string& e) {
            LOG_ERROR << e;
            Json::Value err; err["error"] = friendlyError(e);
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k500InternalServerError); callback(r);
        });
}
