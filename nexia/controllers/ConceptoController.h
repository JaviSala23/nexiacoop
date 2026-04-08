#pragma once
#include <drogon/drogon.h>
using namespace drogon;

class ConceptoController : public drogon::HttpController<ConceptoController>
{
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(ConceptoController::pagina,            "/conceptos",                  Get,    "AuthFilter");
    ADD_METHOD_TO(ConceptoController::listar,            "/api/conceptos",              Get,    "AuthFilter");
    ADD_METHOD_TO(ConceptoController::crear,             "/api/conceptos",              Post,   "AuthFilter");
    ADD_METHOD_TO(ConceptoController::actualizar,        "/api/conceptos/{id}",         Put,    "AuthFilter");
    ADD_METHOD_TO(ConceptoController::toggle,            "/api/conceptos/{id}/toggle",  Post,   "AuthFilter");
    ADD_METHOD_TO(ConceptoController::listarCuotas,      "/api/cuotas-config",          Get,    "AuthFilter");
    ADD_METHOD_TO(ConceptoController::upsertCuota,       "/api/cuotas-config",          Post,   "AuthFilter");
    ADD_METHOD_TO(ConceptoController::eliminarCuota,     "/api/cuotas-config/{id}",     Delete, "AuthFilter");
    METHOD_LIST_END

    void pagina(const drogon::HttpRequestPtr&,
                std::function<void(const drogon::HttpResponsePtr&)>&&);
    void listar(const drogon::HttpRequestPtr&,
                std::function<void(const drogon::HttpResponsePtr&)>&&);
    void crear(const drogon::HttpRequestPtr&,
               std::function<void(const drogon::HttpResponsePtr&)>&&);
    void actualizar(const drogon::HttpRequestPtr&,
                    std::function<void(const drogon::HttpResponsePtr&)>&&, int id);
    void toggle(const drogon::HttpRequestPtr&,
                std::function<void(const drogon::HttpResponsePtr&)>&&, int id);

    void listarCuotas(const drogon::HttpRequestPtr&,
                      std::function<void(const drogon::HttpResponsePtr&)>&&);
    void upsertCuota(const drogon::HttpRequestPtr&,
                     std::function<void(const drogon::HttpResponsePtr&)>&&);
    void eliminarCuota(const drogon::HttpRequestPtr&,
                       std::function<void(const drogon::HttpResponsePtr&)>&&, int id);
};
