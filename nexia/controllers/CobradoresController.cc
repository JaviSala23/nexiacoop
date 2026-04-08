#include "CobradoresController.h"
#include <drogon/drogon.h>

static void notFound(std::function<void(const drogon::HttpResponsePtr&)>&& cb) {
    Json::Value err; err["error"] = "Este módulo no está disponible en esta versión del sistema";
    auto r = drogon::HttpResponse::newHttpJsonResponse(err);
    r->setStatusCode(drogon::k404NotFound); cb(r);
}

void CobradoresController::pagina(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& cb) {
    auto r = drogon::HttpResponse::newHttpResponse();
    r->setStatusCode(drogon::k302Found);
    r->addHeader("Location", "/dashboard");
    cb(r);
}
void CobradoresController::listar(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& cb) { notFound(std::move(cb)); }
void CobradoresController::obtener(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& cb, int) { notFound(std::move(cb)); }
void CobradoresController::crear(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& cb) { notFound(std::move(cb)); }
void CobradoresController::actualizar(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& cb, int) { notFound(std::move(cb)); }
void CobradoresController::eliminar(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& cb, int) { notFound(std::move(cb)); }
