#include "AuthController.h"
#include <drogon/drogon.h>
#include "../repositories/UsuarioRepository.h"
#include "../services/AuthService.h"

void AuthController::loginPage(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    auto resp = drogon::HttpResponse::newFileResponse("views/login.html");
    callback(resp);
}

void AuthController::dashboardPage(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    auto resp = drogon::HttpResponse::newFileResponse("views/dashboard.html");
    callback(resp);
}

void AuthController::login(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    auto body = req->getJsonObject();
    if (!body || !(*body)["username"].isString() || !(*body)["password"].isString()) {
        Json::Value err; err["error"] = "Datos inválidos";
        auto r = drogon::HttpResponse::newHttpJsonResponse(err);
        r->setStatusCode(drogon::k400BadRequest);
        callback(r); return;
    }
    std::string username = (*body)["username"].asString();
    std::string password = (*body)["password"].asString();

    UsuarioRepository::buscarPorUsername(username,
        [req, password, callback](Usuario u, bool found) {
            Json::Value res;
            if (!found || !AuthService::verifyPassword(password, u.password_hash)) {
                res["error"] = "Usuario o contraseña incorrectos";
                auto r = drogon::HttpResponse::newHttpJsonResponse(res);
                r->setStatusCode(drogon::k401Unauthorized);
                callback(r); return;
            }
            auto session = req->getSession();
            session->insert("usuario_id",  u.id_usuario);
            session->insert("usuario_rol", u.rol);
            session->insert("usuario_nombre", u.nombre);
            res["ok"]     = true;
            res["nombre"] = u.nombre;
            res["rol"]    = u.rol;
            callback(drogon::HttpResponse::newHttpJsonResponse(res));
        },
        [callback](const std::string& e) {
            LOG_ERROR << e;
            Json::Value err; err["error"] = "Usuario o contraseña incorrectos";
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k401Unauthorized);
            callback(r);
        });
}

void AuthController::logout(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    req->getSession()->clear();
    Json::Value res; res["ok"] = true;
    callback(drogon::HttpResponse::newHttpJsonResponse(res));
}

void AuthController::me(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    auto session = req->getSession();
    if (!session || !session->find("usuario_id")) {
        Json::Value err; err["error"] = "No autenticado";
        auto r = drogon::HttpResponse::newHttpJsonResponse(err);
        r->setStatusCode(drogon::k401Unauthorized);
        callback(r); return;
    }
    Json::Value res;
    res["id"]     = session->get<int>("usuario_id");
    res["rol"]    = session->get<std::string>("usuario_rol");
    res["nombre"] = session->get<std::string>("usuario_nombre");
    callback(drogon::HttpResponse::newHttpJsonResponse(res));
}
