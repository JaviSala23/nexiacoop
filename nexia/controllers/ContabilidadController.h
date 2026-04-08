#pragma once
#include <drogon/drogon.h>
using namespace drogon;

class ContabilidadController : public drogon::HttpController<ContabilidadController>
{
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(ContabilidadController::pagina,           "/contabilidad",                         Get,    "AuthFilter");
    ADD_METHOD_TO(ContabilidadController::listarCuentas,    "/api/contabilidad/cuentas",             Get,    "AuthFilter");
    ADD_METHOD_TO(ContabilidadController::crearCuenta,      "/api/contabilidad/cuentas",             Post,   "AuthFilter");
    ADD_METHOD_TO(ContabilidadController::actualizarCuenta, "/api/contabilidad/cuentas/{id}",        Put,    "AuthFilter");
    ADD_METHOD_TO(ContabilidadController::toggleCuenta,     "/api/contabilidad/cuentas/{id}/toggle", Post,   "AuthFilter");
    ADD_METHOD_TO(ContabilidadController::listarMovimientos,"/api/contabilidad/movimientos",         Get,    "AuthFilter");
    ADD_METHOD_TO(ContabilidadController::crearMovimiento,  "/api/contabilidad/movimientos",         Post,   "AuthFilter");
    ADD_METHOD_TO(ContabilidadController::eliminarMovimiento,"/api/contabilidad/movimientos/{id}",   Delete, "AuthFilter");
    ADD_METHOD_TO(ContabilidadController::resumen,          "/api/contabilidad/resumen",             Get,    "AuthFilter");
    ADD_METHOD_TO(ContabilidadController::resumenMensual,   "/api/contabilidad/resumen/mensual",     Get,    "AuthFilter");
    METHOD_LIST_END

    void pagina(const drogon::HttpRequestPtr&,
                std::function<void(const drogon::HttpResponsePtr&)>&&);
    void listarCuentas(const drogon::HttpRequestPtr&,
                       std::function<void(const drogon::HttpResponsePtr&)>&&);
    void crearCuenta(const drogon::HttpRequestPtr&,
                     std::function<void(const drogon::HttpResponsePtr&)>&&);
    void actualizarCuenta(const drogon::HttpRequestPtr&,
                          std::function<void(const drogon::HttpResponsePtr&)>&&, int id);
    void toggleCuenta(const drogon::HttpRequestPtr&,
                      std::function<void(const drogon::HttpResponsePtr&)>&&, int id);
    void listarMovimientos(const drogon::HttpRequestPtr&,
                           std::function<void(const drogon::HttpResponsePtr&)>&&);
    void crearMovimiento(const drogon::HttpRequestPtr&,
                         std::function<void(const drogon::HttpResponsePtr&)>&&);
    void eliminarMovimiento(const drogon::HttpRequestPtr&,
                            std::function<void(const drogon::HttpResponsePtr&)>&&, int id);
    void resumen(const drogon::HttpRequestPtr&,
                 std::function<void(const drogon::HttpResponsePtr&)>&&);
    void resumenMensual(const drogon::HttpRequestPtr&,
                        std::function<void(const drogon::HttpResponsePtr&)>&&);
};
