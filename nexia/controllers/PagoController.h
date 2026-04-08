#pragma once
#include <drogon/drogon.h>
using namespace drogon;

class PagoController : public drogon::HttpController<PagoController>
{
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(PagoController::pagina,          "/pagos",                        Get, "AuthFilter");
    ADD_METHOD_TO(PagoController::registrar,       "/api/pagos",                    Post,"AuthFilter");
    ADD_METHOD_TO(PagoController::listar,          "/api/pagos",                    Get, "AuthFilter");
    ADD_METHOD_TO(PagoController::morosos,         "/api/morosos",                  Get, "AuthFilter");
    ADD_METHOD_TO(PagoController::resumenMes,      "/api/pagos/resumen/mes",         Get, "AuthFilter");
    ADD_METHOD_TO(PagoController::resumenMedioPago,"/api/pagos/resumen/medio-pago",  Get, "AuthFilter");
    METHOD_LIST_END

    void pagina(const drogon::HttpRequestPtr&,
                std::function<void(const drogon::HttpResponsePtr&)>&&);
    void registrar(const drogon::HttpRequestPtr&,
                   std::function<void(const drogon::HttpResponsePtr&)>&&);
    void listar(const drogon::HttpRequestPtr&,
                std::function<void(const drogon::HttpResponsePtr&)>&&);
    void morosos(const drogon::HttpRequestPtr&,
                 std::function<void(const drogon::HttpResponsePtr&)>&&);
    void resumenMes(const drogon::HttpRequestPtr&,
                    std::function<void(const drogon::HttpResponsePtr&)>&&);
    void resumenMedioPago(const drogon::HttpRequestPtr&,
                          std::function<void(const drogon::HttpResponsePtr&)>&&);
};
