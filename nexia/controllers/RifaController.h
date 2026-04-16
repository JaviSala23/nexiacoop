#pragma once
#include <drogon/drogon.h>
using namespace drogon;

class RifaController : public drogon::HttpController<RifaController>
{
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(RifaController::pagina,            "/rifas",                             Get,  "AuthFilter");
    ADD_METHOD_TO(RifaController::listar,            "/api/rifas",                         Get,  "AuthFilter");
    ADD_METHOD_TO(RifaController::crear,             "/api/rifas",                         Post, "AuthFilter");
    ADD_METHOD_TO(RifaController::actualizar,        "/api/rifas/{id}",                    Put,  "AuthFilter");
    ADD_METHOD_TO(RifaController::listarNumeros,     "/api/rifas/{id}/numeros",            Get,  "AuthFilter");
    ADD_METHOD_TO(RifaController::disponibles,       "/api/rifas/{id}/disponibles",        Get,  "AuthFilter");
    ADD_METHOD_TO(RifaController::asignarNumero,     "/api/rifas/{id}/numeros",            Post, "AuthFilter");
    ADD_METHOD_TO(RifaController::asignarATodas,     "/api/rifas/{id}/asignar-todas",      Post, "AuthFilter");
    ADD_METHOD_TO(RifaController::anularNumero,      "/api/rifas/numeros/{id}/anular",     Post, "AuthFilter");
    ADD_METHOD_TO(RifaController::actualizarNumero,  "/api/rifas/numeros/{id}",            Put,  "AuthFilter");
    ADD_METHOD_TO(RifaController::listarCuotas,      "/api/rifas/numeros/{id}/cuotas",     Get,  "AuthFilter");
    ADD_METHOD_TO(RifaController::pagarCuotas,       "/api/rifas/cuotas/pagar",            Post, "AuthFilter");
    ADD_METHOD_TO(RifaController::sortear,           "/api/rifas/{id}/sortear",            Post, "AuthFilter");
    ADD_METHOD_TO(RifaController::listarPremios,     "/api/rifas/{id}/premios",            Get,  "AuthFilter");
    METHOD_LIST_END

    void pagina(const drogon::HttpRequestPtr&,
                std::function<void(const drogon::HttpResponsePtr&)>&&);
    void listar(const drogon::HttpRequestPtr&,
                std::function<void(const drogon::HttpResponsePtr&)>&&);
    void crear(const drogon::HttpRequestPtr&,
               std::function<void(const drogon::HttpResponsePtr&)>&&);
    void actualizar(const drogon::HttpRequestPtr&,
                    std::function<void(const drogon::HttpResponsePtr&)>&&, int id);
    void listarNumeros(const drogon::HttpRequestPtr&,
                       std::function<void(const drogon::HttpResponsePtr&)>&&, int id);
    void disponibles(const drogon::HttpRequestPtr&,
                     std::function<void(const drogon::HttpResponsePtr&)>&&, int id);
    void asignarNumero(const drogon::HttpRequestPtr&,
                       std::function<void(const drogon::HttpResponsePtr&)>&&, int id);
    void asignarATodas(const drogon::HttpRequestPtr&,
                       std::function<void(const drogon::HttpResponsePtr&)>&&, int id);
    void anularNumero(const drogon::HttpRequestPtr&,
                      std::function<void(const drogon::HttpResponsePtr&)>&&, int id);
    void actualizarNumero(const drogon::HttpRequestPtr&,
                          std::function<void(const drogon::HttpResponsePtr&)>&&, int id);
    void listarCuotas(const drogon::HttpRequestPtr&,
                      std::function<void(const drogon::HttpResponsePtr&)>&&, int id);
    void pagarCuotas(const drogon::HttpRequestPtr&,
                     std::function<void(const drogon::HttpResponsePtr&)>&&);
    void sortear(const drogon::HttpRequestPtr&,
                 std::function<void(const drogon::HttpResponsePtr&)>&&, int id);
    void listarPremios(const drogon::HttpRequestPtr&,
                       std::function<void(const drogon::HttpResponsePtr&)>&&, int id);
};
