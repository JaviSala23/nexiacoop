#include "AdminController.h"
#include <drogon/drogon.h>
#include "../repositories/UsuarioRepository.h"
#include "../repositories/ParametroRepository.h"
#include "../repositories/ProvinciaRepository.h"
#include "../repositories/CursoRepository.h"
#include "../services/AuthService.h"

void AdminController::pagina(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    callback(drogon::HttpResponse::newFileResponse("views/admin.html"));
}

void AdminController::listarUsuarios(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    UsuarioRepository::listar(
        [callback](std::vector<Usuario> lista) {
            Json::Value arr(Json::arrayValue);
            for (const auto& u : lista) {
                Json::Value o;
                o["id_usuario"] = u.id_usuario;
                o["username"]   = u.username;
                o["nombre"]     = u.nombre;
                o["rol"]        = u.rol;
                o["activo"]     = u.activo;
                o["created_at"] = u.created_at;
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

void AdminController::crearUsuario(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    auto body = req->getJsonObject();
    if (!body) {
        Json::Value err; err["error"] = "JSON inválido";
        auto r = drogon::HttpResponse::newHttpJsonResponse(err);
        r->setStatusCode(drogon::k400BadRequest); callback(r); return;
    }
    Usuario u;
    u.username      = (*body)["username"].asString();
    u.nombre        = (*body).get("nombre","").asString();
    u.rol           = (*body).get("rol","OPERADOR").asString();
    std::string pw  = (*body).get("password","admin123").asString();
    u.password_hash = AuthService::hashPassword(pw);

    UsuarioRepository::insertar(u,
        [callback](int id) {
            Json::Value res; res["ok"] = true; res["id_usuario"] = id;
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

void AdminController::resetPassword(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id)
{
    auto body = req->getJsonObject();
    std::string newPw = body ? (*body).get("password","admin123").asString() : "admin123";
    std::string hash  = AuthService::hashPassword(newPw);
    UsuarioRepository::actualizarPassword(id, hash,
        [callback]() {
            Json::Value res; res["ok"] = true; res["mensaje"] = "Contraseña actualizada";
            callback(drogon::HttpResponse::newHttpJsonResponse(res));
        },
        [callback](const std::string& e) {
            LOG_ERROR << e;
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k500InternalServerError); callback(r);
        });
}

void AdminController::cambiarEstado(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id)
{
    auto body = req->getJsonObject();
    bool activo = body ? (*body).get("activo", true).asBool() : true;
    UsuarioRepository::cambiarEstado(id, activo,
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

void AdminController::parametros(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    ParametroRepository::obtener(
        [callback](const Parametro& p) {
            Json::Value o;
            o["id_parametro"]      = p.id_parametro;
            o["mes_cobranza"]      = p.mes_cobranza;
            o["anio_cobranza"]     = p.anio_cobranza;
            o["nombre_institucion"]= p.nombre_institucion;
            o["domicilio"]         = p.domicilio;
            o["telefono"]          = p.telefono;
            o["localidad"]         = p.localidad;
            o["provincia"]         = p.provincia;
            o["email"]             = p.email;
            o["logo_url"]          = p.logo_url;
            callback(drogon::HttpResponse::newHttpJsonResponse(o));
        },
        [callback](const std::string& e) {
            LOG_ERROR << e;
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k500InternalServerError); callback(r);
        });
}

void AdminController::actualizarParam(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    auto body = req->getJsonObject();
    if (!body) {
        Json::Value err; err["error"] = "JSON inválido";
        auto r = drogon::HttpResponse::newHttpJsonResponse(err);
        r->setStatusCode(drogon::k400BadRequest); callback(r); return;
    }
    Parametro p;
    p.id_parametro      = (*body)["id_parametro"].asInt();
    p.mes_cobranza      = (*body)["mes_cobranza"].asInt();
    p.anio_cobranza     = (*body)["anio_cobranza"].asInt();
    p.nombre_institucion= (*body)["nombre_institucion"].asString();
    p.domicilio         = (*body)["domicilio"].asString();
    p.telefono          = (*body)["telefono"].asString();
    p.localidad         = (*body)["localidad"].asString();
    p.provincia         = (*body)["provincia"].asString();
    p.email             = (*body)["email"].asString();
    p.logo_url          = (*body)["logo_url"].asString();
    ParametroRepository::actualizar(p,
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

void AdminController::listarProvincias(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    ProvinciaRepository::listarProvincias(
        [callback](std::vector<Provincia> lista) {
            Json::Value arr(Json::arrayValue);
            for (const auto& p : lista) {
                Json::Value o;
                o["id_provincia"] = p.id_provincia;
                o["nombre"]       = p.nombre;
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

void AdminController::listarLocalidades(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    int idProv = 0;
    auto p = req->getParameter("provincia");
    if (!p.empty()) idProv = std::stoi(p);
    ProvinciaRepository::listarLocalidades(idProv,
        [callback](std::vector<Localidad> lista) {
            Json::Value arr(Json::arrayValue);
            for (const auto& l : lista) {
                Json::Value o;
                o["id_localidad"]    = l.id_localidad;
                o["id_provincia"]    = l.id_provincia;
                o["nombre"]          = l.nombre;
                o["nombre_provincia"]= l.nombre_provincia;
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

void AdminController::crearLocalidad(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    auto body = req->getJsonObject();
    if (!body || !(*body)["nombre"].isString() || !(*body)["id_provincia"].isInt()) {
        Json::Value err; err["error"] = "Faltan datos (nombre, id_provincia)";
        auto r = drogon::HttpResponse::newHttpJsonResponse(err);
        r->setStatusCode(drogon::k400BadRequest); callback(r); return;
    }
    ProvinciaRepository::insertarLocalidad(
        (*body)["nombre"].asString(), (*body)["id_provincia"].asInt(),
        [callback](int id) {
            Json::Value res; res["id_localidad"] = id; res["ok"] = true;
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

void AdminController::eliminarLocalidad(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id)
{
    ProvinciaRepository::eliminarLocalidad(id,
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

void AdminController::crearProvincia(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    auto body = req->getJsonObject();
    if (!body || !(*body)["nombre"].isString()) {
        Json::Value err; err["error"] = "Falta el campo nombre";
        auto r = drogon::HttpResponse::newHttpJsonResponse(err);
        r->setStatusCode(drogon::k400BadRequest); callback(r); return;
    }
    std::string nombre = (*body)["nombre"].asString();
    ProvinciaRepository::insertarProvincia(nombre,
        [callback](int id) {
            Json::Value res; res["id_provincia"] = id; res["ok"] = true;
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

void AdminController::eliminarProvincia(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id)
{
    ProvinciaRepository::eliminarProvincia(id,
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

// -------------------------------------------------------
// Cursos
// -------------------------------------------------------
static Json::Value cursoToJson(const Curso& c) {
    Json::Value o;
    o["id_curso"] = c.id_curso;
    o["nombre"]   = c.nombre;
    o["orden"]    = c.orden;
    o["activo"]   = c.activo;
    return o;
}

void AdminController::listarCursos(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    CursoRepository::listar(
        [callback](std::vector<Curso> lista) {
            Json::Value arr(Json::arrayValue);
            for (const auto& c : lista) arr.append(cursoToJson(c));
            callback(drogon::HttpResponse::newHttpJsonResponse(arr));
        },
        [callback](const std::string& e) {
            LOG_ERROR << e;
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k500InternalServerError); callback(r);
        });
}

void AdminController::crearCurso(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    auto body = req->getJsonObject();
    if (!body || !(*body)["nombre"].isString()) {
        Json::Value err; err["error"] = "Nombre requerido";
        auto r = drogon::HttpResponse::newHttpJsonResponse(err);
        r->setStatusCode(drogon::k400BadRequest); callback(r); return;
    }
    Curso c;
    c.nombre = (*body)["nombre"].asString();
    c.orden  = (*body).get("orden", 0).asInt();
    c.activo = (*body).get("activo", true).asBool();
    CursoRepository::insertar(c,
        [callback](int id) {
            Json::Value res; res["ok"] = true; res["id_curso"] = id;
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

void AdminController::actualizarCurso(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id)
{
    auto body = req->getJsonObject();
    if (!body || !(*body)["nombre"].isString()) {
        Json::Value err; err["error"] = "Nombre requerido";
        auto r = drogon::HttpResponse::newHttpJsonResponse(err);
        r->setStatusCode(drogon::k400BadRequest); callback(r); return;
    }
    Curso c;
    c.nombre = (*body)["nombre"].asString();
    c.orden  = (*body).get("orden", 0).asInt();
    c.activo = (*body).get("activo", true).asBool();
    CursoRepository::actualizar(id, c,
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

void AdminController::eliminarCurso(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id)
{
    CursoRepository::eliminar(id,
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
