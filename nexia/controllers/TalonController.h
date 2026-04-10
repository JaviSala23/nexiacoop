#pragma once
#include <drogon/drogon.h>
using namespace drogon;

class TalonController : public drogon::HttpController<TalonController>
{
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(TalonController::pagina,         "/talones",                    Get, "AuthFilter");
    ADD_METHOD_TO(TalonController::listar,         "/api/talones",                Get, "AuthFilter");
    ADD_METHOD_TO(TalonController::generar,        "/api/talones/generar",        Post,"AuthFilter");
    ADD_METHOD_TO(TalonController::generarExtra,       "/api/talones/generar-extra",        Post,"AuthFilter");
    ADD_METHOD_TO(TalonController::generarExtraTodos,   "/api/talones/generar-extra-todos",  Post,"AuthFilter");
    ADD_METHOD_TO(TalonController::editarMonto,         "/api/talones/{id}/monto",           Put, "AuthFilter");
    ADD_METHOD_TO(TalonController::anular,              "/api/talones/{id}/anular",          Post,"AuthFilter");
    ADD_METHOD_TO(TalonController::anularBatch,         "/api/talones/anular-batch",         Post,"AuthFilter");
    ADD_METHOD_TO(TalonController::familiasPendientes,  "/api/talones/familias-pendientes",  Get, "AuthFilter");
    METHOD_LIST_END

    void pagina(const drogon::HttpRequestPtr&,
                std::function<void(const drogon::HttpResponsePtr&)>&&);
    void listar(const drogon::HttpRequestPtr&,
                std::function<void(const drogon::HttpResponsePtr&)>&&);
    void generar(const drogon::HttpRequestPtr&,
                 std::function<void(const drogon::HttpResponsePtr&)>&&);
    void generarExtra(const drogon::HttpRequestPtr&,
                      std::function<void(const drogon::HttpResponsePtr&)>&&);
    void generarExtraTodos(const drogon::HttpRequestPtr&,
                           std::function<void(const drogon::HttpResponsePtr&)>&&);
    void editarMonto(const drogon::HttpRequestPtr&,
                     std::function<void(const drogon::HttpResponsePtr&)>&&, int id);
    void anular(const drogon::HttpRequestPtr&,
                std::function<void(const drogon::HttpResponsePtr&)>&&, int id);
    void anularBatch(const drogon::HttpRequestPtr&,
                    std::function<void(const drogon::HttpResponsePtr&)>&&);
    void familiasPendientes(const drogon::HttpRequestPtr&,
                            std::function<void(const drogon::HttpResponsePtr&)>&&);
};
