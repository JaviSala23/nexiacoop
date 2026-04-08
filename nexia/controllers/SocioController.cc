#include "SocioController.h"
#include <drogon/drogon.h>

static void notFound(std::function<void(const drogon::HttpResponsePtr&)>&& cb) {
    Json::Value err; err["error"] = "Este módulo no está disponible en esta versión del sistema";
    auto r = drogon::HttpResponse::newHttpJsonResponse(err);
    r->setStatusCode(drogon::k404NotFound); cb(r);
}

void SocioController::pagina(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& cb) {
    auto r = drogon::HttpResponse::newHttpResponse();
    r->setStatusCode(drogon::k302Found);
    r->addHeader("Location", "/familias");
    cb(r);
}
void SocioController::padron(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& cb) {
    auto r = drogon::HttpResponse::newHttpResponse();
    r->setStatusCode(drogon::k302Found);
    r->addHeader("Location", "/familias");
    cb(r);
}
void SocioController::listar(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& cb) { notFound(std::move(cb)); }
void SocioController::obtener(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& cb, int) { notFound(std::move(cb)); }
void SocioController::crear(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& cb) { notFound(std::move(cb)); }
void SocioController::actualizar(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& cb, int) { notFound(std::move(cb)); }
void SocioController::cambiarEstado(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& cb, int) { notFound(std::move(cb)); }
