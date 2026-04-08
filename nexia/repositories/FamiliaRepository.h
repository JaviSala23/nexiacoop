#pragma once
#include <functional>
#include <vector>
#include <string>
#include <json/json.h>
#include "../models/Familia.h"
#include "../models/Tutor.h"
#include "../models/Alumno.h"

class FamiliaRepository {
public:
    static void existeNumeroODni(int numero, const std::string& dni, int excluirId,
        std::function<void(bool, std::string)> callback);

    static void listar(const std::string& estado,
        std::function<void(std::vector<Familia>)> callback,
        std::function<void(const std::string&)> errCallback);

    static void buscarPorId(int id,
        std::function<void(Familia)> callback,
        std::function<void(const std::string&)> errCallback);

    static void insertar(const Familia& f,
        std::function<void(int)> callback,
        std::function<void(const std::string&)> errCallback);

    static void actualizar(const Familia& f,
        std::function<void()> callback,
        std::function<void(const std::string&)> errCallback);

    static void cambiarEstado(int id, const std::string& estado,
        std::function<void()> callback,
        std::function<void(const std::string&)> errCallback);

    // Tutores
    static void listarTutores(int idFamilia,
        std::function<void(std::vector<Tutor>)> callback,
        std::function<void(const std::string&)> errCallback);

    static void insertarTutor(const Tutor& t,
        std::function<void(int)> callback,
        std::function<void(const std::string&)> errCallback);

    static void actualizarTutor(const Tutor& t,
        std::function<void()> callback,
        std::function<void(const std::string&)> errCallback);

    static void eliminarTutor(int idTutor,
        std::function<void()> callback,
        std::function<void(const std::string&)> errCallback);

    // Alumnos
    static void listarAlumnos(int idFamilia,
        std::function<void(std::vector<Alumno>)> callback,
        std::function<void(const std::string&)> errCallback);

    static void listarTodosAlumnos(const std::string& estado, const std::string& anioEscolar,
        std::function<void(std::vector<Alumno>)> callback,
        std::function<void(const std::string&)> errCallback);

    static void statsAlumnos(
        std::function<void(Json::Value)> callback,
        std::function<void(const std::string&)> errCallback);

    static void promoverGrados(
        std::function<void(int, int, int)> callback,
        std::function<void(const std::string&)> errCallback);

    static void revertirPromocion(
        std::function<void(bool, std::string)> callback,
        std::function<void(const std::string&)> errCallback);

    static void estadoUltimaPromocion(
        std::function<void(bool, std::string)> callback,
        std::function<void(const std::string&)> errCallback);

    static void insertarAlumno(const Alumno& a,
        std::function<void(int)> callback,
        std::function<void(const std::string&)> errCallback);

    static void actualizarAlumno(const Alumno& a,
        std::function<void()> callback,
        std::function<void(const std::string&)> errCallback);

    static void eliminarAlumno(int idAlumno,
        std::function<void()> callback,
        std::function<void(const std::string&)> errCallback);

    static void contarAlumnosActivos(int idFamilia,
        std::function<void(int)> callback,
        std::function<void(const std::string&)> errCallback);
};
