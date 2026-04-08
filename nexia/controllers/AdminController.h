#pragma once
#include <drogon/drogon.h>
using namespace drogon;

class AdminController : public drogon::HttpController<AdminController>
{
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(AdminController::pagina,          "/admin",                   Get, "AuthFilter");
    ADD_METHOD_TO(AdminController::listarUsuarios,  "/api/admin/usuarios",      Get, "AuthFilter");
    ADD_METHOD_TO(AdminController::crearUsuario,    "/api/admin/usuarios",      Post,"AuthFilter");
    ADD_METHOD_TO(AdminController::resetPassword,   "/api/admin/usuarios/{id}/reset",Post,"AuthFilter");
    ADD_METHOD_TO(AdminController::cambiarEstado,   "/api/admin/usuarios/{id}/estado",Post,"AuthFilter");
    ADD_METHOD_TO(AdminController::parametros,        "/api/admin/parametros",        Get, "AuthFilter");
    ADD_METHOD_TO(AdminController::actualizarParam,    "/api/admin/parametros",        Put, "AuthFilter");
    ADD_METHOD_TO(AdminController::listarProvincias,   "/api/provincias",              Get, "AuthFilter");
    ADD_METHOD_TO(AdminController::listarLocalidades,  "/api/localidades",             Get, "AuthFilter");
    ADD_METHOD_TO(AdminController::crearLocalidad,     "/api/admin/localidades",       Post, "AuthFilter");
    ADD_METHOD_TO(AdminController::eliminarLocalidad,  "/api/admin/localidades/{id}",  Delete, "AuthFilter");
    ADD_METHOD_TO(AdminController::crearProvincia,     "/api/admin/provincias",        Post, "AuthFilter");
    ADD_METHOD_TO(AdminController::eliminarProvincia,  "/api/admin/provincias/{id}",   Delete, "AuthFilter");
    ADD_METHOD_TO(AdminController::listarCursos,   "/api/admin/cursos",        Get,    "AuthFilter");
    ADD_METHOD_TO(AdminController::crearCurso,     "/api/admin/cursos",        Post,   "AuthFilter");
    ADD_METHOD_TO(AdminController::actualizarCurso,"/api/admin/cursos/{id}",   Put,    "AuthFilter");
    ADD_METHOD_TO(AdminController::eliminarCurso,  "/api/admin/cursos/{id}",   Delete, "AuthFilter");
    METHOD_LIST_END

    void pagina(const drogon::HttpRequestPtr&,
                std::function<void(const drogon::HttpResponsePtr&)>&&);
    void listarUsuarios(const drogon::HttpRequestPtr&,
                        std::function<void(const drogon::HttpResponsePtr&)>&&);
    void crearUsuario(const drogon::HttpRequestPtr&,
                      std::function<void(const drogon::HttpResponsePtr&)>&&);
    void resetPassword(const drogon::HttpRequestPtr&,
                       std::function<void(const drogon::HttpResponsePtr&)>&&, int id);
    void cambiarEstado(const drogon::HttpRequestPtr&,
                       std::function<void(const drogon::HttpResponsePtr&)>&&, int id);
    void parametros(const drogon::HttpRequestPtr&,
                    std::function<void(const drogon::HttpResponsePtr&)>&&);
    void actualizarParam(const drogon::HttpRequestPtr&,
                         std::function<void(const drogon::HttpResponsePtr&)>&&);
    void listarProvincias(const drogon::HttpRequestPtr&,
                          std::function<void(const drogon::HttpResponsePtr&)>&&);
    void listarLocalidades(const drogon::HttpRequestPtr&,
                           std::function<void(const drogon::HttpResponsePtr&)>&&);
    void crearLocalidad(const drogon::HttpRequestPtr&,
                        std::function<void(const drogon::HttpResponsePtr&)>&&);
    void eliminarLocalidad(const drogon::HttpRequestPtr&,
                           std::function<void(const drogon::HttpResponsePtr&)>&&, int id);
    void crearProvincia(const drogon::HttpRequestPtr&,
                        std::function<void(const drogon::HttpResponsePtr&)>&&);
    void eliminarProvincia(const drogon::HttpRequestPtr&,
                           std::function<void(const drogon::HttpResponsePtr&)>&&, int id);
    void listarCursos(const drogon::HttpRequestPtr&,
                      std::function<void(const drogon::HttpResponsePtr&)>&&);
    void crearCurso(const drogon::HttpRequestPtr&,
                    std::function<void(const drogon::HttpResponsePtr&)>&&);
    void actualizarCurso(const drogon::HttpRequestPtr&,
                         std::function<void(const drogon::HttpResponsePtr&)>&&, int id);
    void eliminarCurso(const drogon::HttpRequestPtr&,
                       std::function<void(const drogon::HttpResponsePtr&)>&&, int id);
};
