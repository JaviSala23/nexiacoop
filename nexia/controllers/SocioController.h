#pragma once
#include <drogon/drogon.h>
using namespace drogon;

class SocioController : public drogon::HttpController<SocioController>
{
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(SocioController::pagina,   "/socios",        Get,  "AuthFilter");
    ADD_METHOD_TO(SocioController::padron,   "/padron",         Get,  "AuthFilter");
    ADD_METHOD_TO(SocioController::listar,   "/api/socios",    Get,  "AuthFilter");
    ADD_METHOD_TO(SocioController::obtener,  "/api/socios/{id}",Get, "AuthFilter");
    ADD_METHOD_TO(SocioController::crear,    "/api/socios",    Post, "AuthFilter");
    ADD_METHOD_TO(SocioController::actualizar,"/api/socios/{id}",Put,"AuthFilter");
    ADD_METHOD_TO(SocioController::cambiarEstado,"/api/socios/{id}/estado",Post,"AuthFilter");
    METHOD_LIST_END

    void pagina(const drogon::HttpRequestPtr&,
                std::function<void(const drogon::HttpResponsePtr&)>&&);
    void padron(const drogon::HttpRequestPtr&,
                std::function<void(const drogon::HttpResponsePtr&)>&&);
    void listar(const drogon::HttpRequestPtr&,
                std::function<void(const drogon::HttpResponsePtr&)>&&);
    void obtener(const drogon::HttpRequestPtr&,
                 std::function<void(const drogon::HttpResponsePtr&)>&&, int id);
    void crear(const drogon::HttpRequestPtr&,
               std::function<void(const drogon::HttpResponsePtr&)>&&);
    void actualizar(const drogon::HttpRequestPtr&,
                    std::function<void(const drogon::HttpResponsePtr&)>&&, int id);
    void cambiarEstado(const drogon::HttpRequestPtr&,
                       std::function<void(const drogon::HttpResponsePtr&)>&&, int id);
};
