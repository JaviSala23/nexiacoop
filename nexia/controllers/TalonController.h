#pragma once
#include <drogon/drogon.h>
using namespace drogon;

class TalonController : public drogon::HttpController<TalonController>
{
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(TalonController::pagina,   "/talones",              Get, "AuthFilter");
    ADD_METHOD_TO(TalonController::listar,   "/api/talones",          Get, "AuthFilter");
    ADD_METHOD_TO(TalonController::generar,  "/api/talones/generar",  Post,"AuthFilter");
    ADD_METHOD_TO(TalonController::anular,   "/api/talones/{id}/anular",Post,"AuthFilter");
    METHOD_LIST_END

    void pagina(const drogon::HttpRequestPtr&,
                std::function<void(const drogon::HttpResponsePtr&)>&&);
    void listar(const drogon::HttpRequestPtr&,
                std::function<void(const drogon::HttpResponsePtr&)>&&);
    void generar(const drogon::HttpRequestPtr&,
                 std::function<void(const drogon::HttpResponsePtr&)>&&);
    void anular(const drogon::HttpRequestPtr&,
                std::function<void(const drogon::HttpResponsePtr&)>&&, int id);
};
