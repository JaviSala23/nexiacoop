#pragma once
#include <drogon/drogon.h>
using namespace drogon;

class CobradoresController : public drogon::HttpController<CobradoresController>
{
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(CobradoresController::pagina,   "/cobradoras",        Get, "AuthFilter");
    ADD_METHOD_TO(CobradoresController::listar,   "/api/cobradoras",    Get, "AuthFilter");
    ADD_METHOD_TO(CobradoresController::obtener,  "/api/cobradoras/{id}",Get,"AuthFilter");
    ADD_METHOD_TO(CobradoresController::crear,    "/api/cobradoras",    Post,"AuthFilter");
    ADD_METHOD_TO(CobradoresController::actualizar,"/api/cobradoras/{id}",Put,"AuthFilter");
    ADD_METHOD_TO(CobradoresController::eliminar, "/api/cobradoras/{id}",Delete,"AuthFilter");
    METHOD_LIST_END

    void pagina(const drogon::HttpRequestPtr&,
                std::function<void(const drogon::HttpResponsePtr&)>&&);
    void listar(const drogon::HttpRequestPtr&,
                std::function<void(const drogon::HttpResponsePtr&)>&&);
    void obtener(const drogon::HttpRequestPtr&,
                 std::function<void(const drogon::HttpResponsePtr&)>&&, int id);
    void crear(const drogon::HttpRequestPtr&,
               std::function<void(const drogon::HttpResponsePtr&)>&&);
    void actualizar(const drogon::HttpRequestPtr&,
                    std::function<void(const drogon::HttpResponsePtr&)>&&, int id);
    void eliminar(const drogon::HttpRequestPtr&,
                  std::function<void(const drogon::HttpResponsePtr&)>&&, int id);
};
