#pragma once
#include <drogon/drogon.h>
using namespace drogon;

class InventarioController : public drogon::HttpController<InventarioController>
{
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(InventarioController::pagina,              "/inventario",                        Get,  "AuthFilter");
    ADD_METHOD_TO(InventarioController::listarElementos,     "/api/inventario/elementos",          Get,  "AuthFilter");
    ADD_METHOD_TO(InventarioController::crearElemento,       "/api/inventario/elementos",          Post, "AuthFilter");
    ADD_METHOD_TO(InventarioController::actualizarElemento,  "/api/inventario/elementos/{id}",     Put,  "AuthFilter");
    ADD_METHOD_TO(InventarioController::listarPrestamos,     "/api/inventario/prestamos",          Get,  "AuthFilter");
    ADD_METHOD_TO(InventarioController::registrarPrestamo,   "/api/inventario/prestamos",          Post, "AuthFilter");
    ADD_METHOD_TO(InventarioController::registrarDevolucion, "/api/inventario/prestamos/{id}/devolucion", Post, "AuthFilter");
    METHOD_LIST_END

    void pagina(const drogon::HttpRequestPtr&,
                std::function<void(const drogon::HttpResponsePtr&)>&&);
    void listarElementos(const drogon::HttpRequestPtr&,
                         std::function<void(const drogon::HttpResponsePtr&)>&&);
    void crearElemento(const drogon::HttpRequestPtr&,
                       std::function<void(const drogon::HttpResponsePtr&)>&&);
    void actualizarElemento(const drogon::HttpRequestPtr&,
                            std::function<void(const drogon::HttpResponsePtr&)>&&, int id);
    void listarPrestamos(const drogon::HttpRequestPtr&,
                         std::function<void(const drogon::HttpResponsePtr&)>&&);
    void registrarPrestamo(const drogon::HttpRequestPtr&,
                           std::function<void(const drogon::HttpResponsePtr&)>&&);
    void registrarDevolucion(const drogon::HttpRequestPtr&,
                             std::function<void(const drogon::HttpResponsePtr&)>&&, int id);
};
