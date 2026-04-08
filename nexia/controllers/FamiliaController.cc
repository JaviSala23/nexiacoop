#include "FamiliaController.h"
#include <drogon/drogon.h>
#include "../repositories/FamiliaRepository.h"
#include <ctime>

static std::string familiaFriendlyError(const std::string& raw) {
    if (raw.find("ya existe") != std::string::npos) return raw;
    if (raw.find("Duplicate entry") != std::string::npos) {
        if (raw.find("numero_familia") != std::string::npos)
            return "El número de familia ya está registrado.";
        if (raw.find("dni") != std::string::npos)
            return "El DNI ya está registrado.";
        return "Ya existe un registro con esos datos.";
    }
    if (raw.find("no encontrada") != std::string::npos || raw.find("no encontrado") != std::string::npos)
        return raw;
    return "Ocurrió un error inesperado. Intente nuevamente.";
}

static Json::Value familiaToJson(const Familia& f) {
    Json::Value o;
    o["id_familia"]             = f.id_familia;
    o["numero_familia"]         = f.numero_familia;
    o["direccion"]              = f.direccion;
    o["telefono"]               = f.telefono;
    o["estado"]                 = f.estado;
    o["fecha_alta"]             = f.fecha_alta;
    o["fecha_baja"]             = f.fecha_baja;
    o["observaciones"]          = f.observaciones;
    o["cantidad_alumnos"]       = f.cantidad_alumnos;
    o["nombre_tutor_principal"] = f.nombre_tutor_principal;
    o["dni_tutor_principal"]    = f.dni_tutor_principal;
    o["telefono_tutor_principal"]= f.telefono_tutor_principal;
    return o;
}

static Json::Value tutorToJson(const Tutor& t) {
    Json::Value o;
    o["id_tutor"]        = t.id_tutor;
    o["id_familia"]      = t.id_familia;
    o["dni"]             = t.dni;
    o["nombre_completo"] = t.nombre_completo;
    o["telefono"]        = t.telefono;
    o["email"]           = t.email;
    o["es_principal"]    = t.es_principal;
    return o;
}

static Json::Value alumnoToJson(const Alumno& a) {
    Json::Value o;
    o["id_alumno"]        = a.id_alumno;
    o["id_familia"]       = a.id_familia;
    o["dni"]              = a.dni;
    o["nombre_completo"]  = a.nombre_completo;
    o["fecha_nacimiento"] = a.fecha_nacimiento;
    o["anio_escolar"]     = a.anio_escolar;
    o["estado"]           = a.estado;
    return o;
}

// -------------------------------------------------------
// Familia
// -------------------------------------------------------
void FamiliaController::pagina(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    callback(drogon::HttpResponse::newFileResponse("views/familias.html"));
}

void FamiliaController::listar(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    std::string estado = req->getParameter("estado");
    FamiliaRepository::listar(estado,
        [callback](std::vector<Familia> lista) {
            Json::Value arr(Json::arrayValue);
            for (const auto& f : lista) arr.append(familiaToJson(f));
            callback(drogon::HttpResponse::newHttpJsonResponse(arr));
        },
        [callback](const std::string& e) {
            LOG_ERROR << e;
            Json::Value err; err["error"] = familiaFriendlyError(e);
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k500InternalServerError); callback(r);
        });
}

void FamiliaController::obtener(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id)
{
    FamiliaRepository::buscarPorId(id,
        [callback](const Familia& f) {
            callback(drogon::HttpResponse::newHttpJsonResponse(familiaToJson(f)));
        },
        [callback](const std::string& e) {
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k404NotFound); callback(r);
        });
}

void FamiliaController::crear(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    auto body = req->getJsonObject();
    if (!body) {
        Json::Value err; err["error"] = "JSON inválido";
        auto r = drogon::HttpResponse::newHttpJsonResponse(err);
        r->setStatusCode(drogon::k400BadRequest); callback(r); return;
    }

    // Validar tutores obligatorios
    if (!body->isMember("tutores") || !(*body)["tutores"].isArray() || (*body)["tutores"].empty()) {
        Json::Value err; err["error"] = "Se requiere al menos un tutor";
        auto r = drogon::HttpResponse::newHttpJsonResponse(err);
        r->setStatusCode(drogon::k400BadRequest); callback(r); return;
    }

    Familia f;
    f.numero_familia = (*body)["numero_familia"].asInt();
    f.direccion      = (*body).get("direccion", "").asString();
    f.telefono       = (*body).get("telefono", "").asString();
    f.estado         = (*body).get("estado", "ACTIVA").asString();
    // Si fecha_alta no se provee o es vacía, usar hoy
    std::string fa = (*body).get("fecha_alta", "").asString();
    if (fa.empty()) {
        time_t now = time(nullptr);
        struct tm tm_info{};
        localtime_r(&now, &tm_info);
        char buf[11];
        strftime(buf, sizeof(buf), "%Y-%m-%d", &tm_info);
        fa = buf;
    }
    f.fecha_alta     = fa;
    f.observaciones  = (*body).get("observaciones", "").asString();

    // Validar el DNI del primer tutor para unicidad
    std::string dniPrincipal = (*body)["tutores"][0].get("dni", "").asString();

    auto errCb = [callback](const std::string& e) {
        LOG_ERROR << e;
        Json::Value err; err["error"] = familiaFriendlyError(e);
        auto r = drogon::HttpResponse::newHttpJsonResponse(err);
        r->setStatusCode(drogon::k400BadRequest); callback(r);
    };

    FamiliaRepository::existeNumeroODni(f.numero_familia, dniPrincipal, 0,
        [body, f, errCb, callback](bool existe, std::string msg) mutable {
            if (existe) { errCb(msg); return; }

            FamiliaRepository::insertar(f,
                [body, errCb, callback](int idFamilia) {
                    // Insertar tutores
                    const auto& tutoresJson = (*body)["tutores"];
                    auto pending = std::make_shared<int>(static_cast<int>(tutoresJson.size()));

                    for (Json::ArrayIndex i = 0; i < tutoresJson.size(); ++i) {
                        Tutor t;
                        t.id_familia      = idFamilia;
                        t.dni             = tutoresJson[i].get("dni", "").asString();
                        t.nombre_completo = tutoresJson[i].get("nombre_completo", "").asString();
                        t.telefono        = tutoresJson[i].get("telefono", "").asString();
                        t.email           = tutoresJson[i].get("email", "").asString();
                        t.es_principal    = (i == 0); // El primero es principal

                        FamiliaRepository::insertarTutor(t,
                            [pending, body, idFamilia, errCb, callback](int /*idTutor*/) {
                                (*pending)--;
                                if (*pending > 0) return;
                                // Insertar alumnos si los hay
                                if (!body->isMember("alumnos") || !(*body)["alumnos"].isArray()) {
                                    Json::Value res; res["ok"] = true; res["id_familia"] = idFamilia;
                                    auto r = drogon::HttpResponse::newHttpJsonResponse(res);
                                    r->setStatusCode(drogon::k201Created); callback(r); return;
                                }
                                const auto& alumnosJson = (*body)["alumnos"];
                                auto pendA = std::make_shared<int>(static_cast<int>(alumnosJson.size()));
                                if (*pendA == 0) {
                                    Json::Value res; res["ok"] = true; res["id_familia"] = idFamilia;
                                    auto r = drogon::HttpResponse::newHttpJsonResponse(res);
                                    r->setStatusCode(drogon::k201Created); callback(r); return;
                                }
                                for (Json::ArrayIndex j = 0; j < alumnosJson.size(); ++j) {
                                    Alumno a;
                                    a.id_familia      = idFamilia;
                                    a.dni             = alumnosJson[j].get("dni", "").asString();
                                    a.nombre_completo = alumnosJson[j].get("nombre_completo", "").asString();
                                    a.fecha_nacimiento= alumnosJson[j].get("fecha_nacimiento", "").asString();
                                    a.anio_escolar    = alumnosJson[j].get("anio_escolar", "").asString();
                                    a.estado          = alumnosJson[j].get("estado", "ACTIVO").asString();
                                    FamiliaRepository::insertarAlumno(a,
                                        [pendA, idFamilia, callback](int /*idAlumno*/) {
                                            (*pendA)--;
                                            if (*pendA > 0) return;
                                            Json::Value res; res["ok"] = true; res["id_familia"] = idFamilia;
                                            auto r = drogon::HttpResponse::newHttpJsonResponse(res);
                                            r->setStatusCode(drogon::k201Created); callback(r);
                                        },
                                        errCb);
                                }
                            },
                            errCb);
                    }
                },
                errCb);
        });
}

void FamiliaController::actualizar(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id)
{
    auto body = req->getJsonObject();
    if (!body) {
        Json::Value err; err["error"] = "JSON inválido";
        auto r = drogon::HttpResponse::newHttpJsonResponse(err);
        r->setStatusCode(drogon::k400BadRequest); callback(r); return;
    }
    Familia f;
    f.id_familia    = id;
    f.direccion     = (*body).get("direccion", "").asString();
    f.telefono      = (*body).get("telefono", "").asString();
    f.estado        = (*body).get("estado", "ACTIVA").asString();
    f.observaciones = (*body).get("observaciones", "").asString();

    FamiliaRepository::actualizar(f,
        [callback]() {
            Json::Value res; res["ok"] = true;
            callback(drogon::HttpResponse::newHttpJsonResponse(res));
        },
        [callback](const std::string& e) {
            LOG_ERROR << e;
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k500InternalServerError); callback(r);
        });
}

void FamiliaController::cambiarEstado(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id)
{
    auto body = req->getJsonObject();
    std::string estado = body ? (*body).get("estado", "BAJA").asString() : "BAJA";
    FamiliaRepository::cambiarEstado(id, estado,
        [callback]() {
            Json::Value res; res["ok"] = true;
            callback(drogon::HttpResponse::newHttpJsonResponse(res));
        },
        [callback](const std::string& e) {
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k500InternalServerError); callback(r);
        });
}

// -------------------------------------------------------
// Tutores
// -------------------------------------------------------
void FamiliaController::listarTutores(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id)
{
    FamiliaRepository::listarTutores(id,
        [callback](std::vector<Tutor> lista) {
            Json::Value arr(Json::arrayValue);
            for (const auto& t : lista) arr.append(tutorToJson(t));
            callback(drogon::HttpResponse::newHttpJsonResponse(arr));
        },
        [callback](const std::string& e) {
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k500InternalServerError); callback(r);
        });
}

void FamiliaController::crearTutor(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id)
{
    auto body = req->getJsonObject();
    if (!body) {
        Json::Value err; err["error"] = "JSON inválido";
        auto r = drogon::HttpResponse::newHttpJsonResponse(err);
        r->setStatusCode(drogon::k400BadRequest); callback(r); return;
    }
    Tutor t;
    t.id_familia      = id;
    t.dni             = (*body).get("dni", "").asString();
    t.nombre_completo = (*body)["nombre_completo"].asString();
    t.telefono        = (*body).get("telefono", "").asString();
    t.email           = (*body).get("email", "").asString();
    t.es_principal    = (*body).get("es_principal", false).asBool();

    FamiliaRepository::insertarTutor(t,
        [callback](int idTutor) {
            Json::Value res; res["ok"] = true; res["id_tutor"] = idTutor;
            auto r = drogon::HttpResponse::newHttpJsonResponse(res);
            r->setStatusCode(drogon::k201Created); callback(r);
        },
        [callback](const std::string& e) {
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k500InternalServerError); callback(r);
        });
}

void FamiliaController::actualizarTutor(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback, int /*id*/, int tid)
{
    auto body = req->getJsonObject();
    if (!body) {
        Json::Value err; err["error"] = "JSON inválido";
        auto r = drogon::HttpResponse::newHttpJsonResponse(err);
        r->setStatusCode(drogon::k400BadRequest); callback(r); return;
    }
    Tutor t;
    t.id_tutor        = tid;
    t.dni             = (*body).get("dni", "").asString();
    t.nombre_completo = (*body)["nombre_completo"].asString();
    t.telefono        = (*body).get("telefono", "").asString();
    t.email           = (*body).get("email", "").asString();
    t.es_principal    = (*body).get("es_principal", false).asBool();

    FamiliaRepository::actualizarTutor(t,
        [callback]() {
            Json::Value res; res["ok"] = true;
            callback(drogon::HttpResponse::newHttpJsonResponse(res));
        },
        [callback](const std::string& e) {
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k500InternalServerError); callback(r);
        });
}

void FamiliaController::eliminarTutor(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback, int /*id*/, int tid)
{
    FamiliaRepository::eliminarTutor(tid,
        [callback]() {
            Json::Value res; res["ok"] = true;
            callback(drogon::HttpResponse::newHttpJsonResponse(res));
        },
        [callback](const std::string& e) {
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k500InternalServerError); callback(r);
        });
}

// -------------------------------------------------------
// Alumnos
// -------------------------------------------------------
void FamiliaController::listarAlumnos(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id)
{
    FamiliaRepository::listarAlumnos(id,
        [callback](std::vector<Alumno> lista) {
            Json::Value arr(Json::arrayValue);
            for (const auto& a : lista) arr.append(alumnoToJson(a));
            callback(drogon::HttpResponse::newHttpJsonResponse(arr));
        },
        [callback](const std::string& e) {
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k500InternalServerError); callback(r);
        });
}

void FamiliaController::crearAlumno(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback, int id)
{
    auto body = req->getJsonObject();
    if (!body) {
        Json::Value err; err["error"] = "JSON inválido";
        auto r = drogon::HttpResponse::newHttpJsonResponse(err);
        r->setStatusCode(drogon::k400BadRequest); callback(r); return;
    }
    Alumno a;
    a.id_familia      = id;
    a.dni             = (*body).get("dni", "").asString();
    a.nombre_completo = (*body)["nombre_completo"].asString();
    a.fecha_nacimiento= (*body).get("fecha_nacimiento", "").asString();
    a.anio_escolar    = (*body).get("anio_escolar", "").asString();
    a.estado          = (*body).get("estado", "ACTIVO").asString();

    FamiliaRepository::insertarAlumno(a,
        [callback](int idAlumno) {
            Json::Value res; res["ok"] = true; res["id_alumno"] = idAlumno;
            auto r = drogon::HttpResponse::newHttpJsonResponse(res);
            r->setStatusCode(drogon::k201Created); callback(r);
        },
        [callback](const std::string& e) {
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k500InternalServerError); callback(r);
        });
}

void FamiliaController::actualizarAlumno(const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback, int /*id*/, int aid)
{
    auto body = req->getJsonObject();
    if (!body) {
        Json::Value err; err["error"] = "JSON inválido";
        auto r = drogon::HttpResponse::newHttpJsonResponse(err);
        r->setStatusCode(drogon::k400BadRequest); callback(r); return;
    }
    Alumno a;
    a.id_alumno        = aid;
    a.dni              = (*body).get("dni", "").asString();
    a.nombre_completo  = (*body)["nombre_completo"].asString();
    a.fecha_nacimiento = (*body).get("fecha_nacimiento", "").asString();
    a.anio_escolar     = (*body).get("anio_escolar", "").asString();
    a.estado           = (*body).get("estado", "ACTIVO").asString();

    FamiliaRepository::actualizarAlumno(a,
        [callback]() {
            Json::Value res; res["ok"] = true;
            callback(drogon::HttpResponse::newHttpJsonResponse(res));
        },
        [callback](const std::string& e) {
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k500InternalServerError); callback(r);
        });
}

void FamiliaController::eliminarAlumno(const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback, int /*id*/, int aid)
{
    FamiliaRepository::eliminarAlumno(aid,
        [callback]() {
            Json::Value res; res["ok"] = true;
            callback(drogon::HttpResponse::newHttpJsonResponse(res));
        },
        [callback](const std::string& e) {
            Json::Value err; err["error"] = e;
            auto r = drogon::HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(drogon::k500InternalServerError); callback(r);
        });
}
