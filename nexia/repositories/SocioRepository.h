#pragma once
#include <vector>
#include <string>
#include <functional>
#include <drogon/drogon.h>
#include "../models/Socio.h"
#include "../models/Responsable.h"

class SocioRepository {
public:
    static void listar(
        const std::string& estado,
        int idCobradora,
        std::function<void(std::vector<Socio>)> callback,
        std::function<void(const std::string&)> errCallback);

    static void buscarPorId(
        int id,
        std::function<void(Socio)> callback,
        std::function<void(const std::string&)> errCallback);

    static void insertar(
        const Socio& s,
        std::function<void(int)> callback,
        std::function<void(const std::string&)> errCallback);

    static void actualizar(
        const Socio& s,
        std::function<void()> callback,
        std::function<void(const std::string&)> errCallback);

    static void cambiarEstado(
        int id,
        const std::string& estado,
        std::function<void()> callback,
        std::function<void(const std::string&)> errCallback);

    // Verifica si existe un socio con el mismo número o DNI (excluyendo un id opcional)
    static void existeNumeroODni(int numero, const std::string& dni, int excluirId,
        std::function<void(bool, std::string)> callback);
};
