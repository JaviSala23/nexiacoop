#pragma once
#include <drogon/drogon.h>
using namespace drogon;

class FamiliaController : public drogon::HttpController<FamiliaController>
{
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(FamiliaController::pagina,       "/familias",              Get,  "AuthFilter");
    ADD_METHOD_TO(FamiliaController::listar,       "/api/familias",          Get,  "AuthFilter");
    ADD_METHOD_TO(FamiliaController::obtener,      "/api/familias/{id}",     Get,  "AuthFilter");
    ADD_METHOD_TO(FamiliaController::crear,        "/api/familias",          Post, "AuthFilter");
    ADD_METHOD_TO(FamiliaController::actualizar,   "/api/familias/{id}",     Put,  "AuthFilter");
    ADD_METHOD_TO(FamiliaController::cambiarEstado,"/api/familias/{id}/estado", Post, "AuthFilter");
    // Tutores
    ADD_METHOD_TO(FamiliaController::listarTutores,   "/api/familias/{id}/tutores",        Get,    "AuthFilter");
    ADD_METHOD_TO(FamiliaController::crearTutor,      "/api/familias/{id}/tutores",        Post,   "AuthFilter");
    ADD_METHOD_TO(FamiliaController::actualizarTutor, "/api/familias/{id}/tutores/{tid}",  Put,    "AuthFilter");
    ADD_METHOD_TO(FamiliaController::eliminarTutor,   "/api/familias/{id}/tutores/{tid}",  Delete, "AuthFilter");
    // Alumnos (por familia)
    ADD_METHOD_TO(FamiliaController::listarAlumnos,       "/api/familias/{id}/alumnos",        Get,    "AuthFilter");
    ADD_METHOD_TO(FamiliaController::crearAlumno,         "/api/familias/{id}/alumnos",        Post,   "AuthFilter");
    ADD_METHOD_TO(FamiliaController::actualizarAlumno,    "/api/familias/{id}/alumnos/{aid}",  Put,    "AuthFilter");
    ADD_METHOD_TO(FamiliaController::eliminarAlumno,      "/api/familias/{id}/alumnos/{aid}",  Delete, "AuthFilter");
    // Alumnos (globales)
    ADD_METHOD_TO(FamiliaController::paginaAlumnos,       "/alumnos",                          Get,    "AuthFilter");
    ADD_METHOD_TO(FamiliaController::listarTodosAlumnos,  "/api/alumnos",                      Get,    "AuthFilter");
    ADD_METHOD_TO(FamiliaController::statsAlumnos,        "/api/alumnos/stats",                Get,    "AuthFilter");
    ADD_METHOD_TO(FamiliaController::promoverGrados,      "/api/alumnos/promover",             Post,   "AuthFilter");
    ADD_METHOD_TO(FamiliaController::estadoUltimaPromocion,"/api/alumnos/ultima-promocion",     Get,    "AuthFilter");
    ADD_METHOD_TO(FamiliaController::revertirPromocion,   "/api/alumnos/revertir-promocion",   Post,   "AuthFilter");
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
    void cambiarEstado(const drogon::HttpRequestPtr&,
                       std::function<void(const drogon::HttpResponsePtr&)>&&, int id);

    void listarTutores(const drogon::HttpRequestPtr&,
                       std::function<void(const drogon::HttpResponsePtr&)>&&, int id);
    void crearTutor(const drogon::HttpRequestPtr&,
                    std::function<void(const drogon::HttpResponsePtr&)>&&, int id);
    void actualizarTutor(const drogon::HttpRequestPtr&,
                         std::function<void(const drogon::HttpResponsePtr&)>&&, int id, int tid);
    void eliminarTutor(const drogon::HttpRequestPtr&,
                       std::function<void(const drogon::HttpResponsePtr&)>&&, int id, int tid);

    void listarAlumnos(const drogon::HttpRequestPtr&,
                       std::function<void(const drogon::HttpResponsePtr&)>&&, int id);
    void crearAlumno(const drogon::HttpRequestPtr&,
                     std::function<void(const drogon::HttpResponsePtr&)>&&, int id);
    void actualizarAlumno(const drogon::HttpRequestPtr&,
                          std::function<void(const drogon::HttpResponsePtr&)>&&, int id, int aid);
    void eliminarAlumno(const drogon::HttpRequestPtr&,
                        std::function<void(const drogon::HttpResponsePtr&)>&&, int id, int aid);

    void paginaAlumnos(const drogon::HttpRequestPtr&,
                       std::function<void(const drogon::HttpResponsePtr&)>&&);
    void listarTodosAlumnos(const drogon::HttpRequestPtr&,
                            std::function<void(const drogon::HttpResponsePtr&)>&&);
    void statsAlumnos(const drogon::HttpRequestPtr&,
                      std::function<void(const drogon::HttpResponsePtr&)>&&);
    void promoverGrados(const drogon::HttpRequestPtr&,
                        std::function<void(const drogon::HttpResponsePtr&)>&&);
    void estadoUltimaPromocion(const drogon::HttpRequestPtr&,
                               std::function<void(const drogon::HttpResponsePtr&)>&&);
    void revertirPromocion(const drogon::HttpRequestPtr&,
                           std::function<void(const drogon::HttpResponsePtr&)>&&);
};
