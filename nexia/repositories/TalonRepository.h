#pragma once
#include <vector>
#include <string>
#include <functional>
#include <drogon/drogon.h>
#include "../models/Talon.h"

class TalonRepository {
public:
    static void listar(int mes, int anio, int idConcepto,
        std::function<void(std::vector<Talon>)> callback,
        std::function<void(const std::string&)> errCallback);

    static void buscarPorCodigo(const std::string& codigo,
        std::function<void(Talon)> callback,
        std::function<void(const std::string&)> errCallback);

    static void insertar(const Talon& t,
        std::function<void(int)> callback,
        std::function<void(const std::string&)> errCallback);

    static void insertarBatch(const std::vector<Talon>& talones,
        std::function<void()> callback,
        std::function<void(const std::string&)> errCallback);

    static void marcarPagado(int idTalon,
        std::function<void()> callback,
        std::function<void(const std::string&)> errCallback);

    static void anular(int idTalon,
        std::function<void()> callback,
        std::function<void(const std::string&)> errCallback);

    static void existeMesAnioConcepto(int mes, int anio, int idConcepto,
        std::function<void(bool)> callback,
        std::function<void(const std::string&)> errCallback);

    static void eliminarAnuladosMesAnioConcepto(int mes, int anio, int idConcepto,
        std::function<void()> callback,
        std::function<void(const std::string&)> errCallback);

    static void familiasConTalonActivo(int mes, int anio, int idConcepto,
        const std::vector<int>& familiaIds,
        std::function<void(std::vector<int>)> callback,
        std::function<void(const std::string&)> errCallback);

    static void listarMorosos(
        std::function<void(Json::Value)> callback,
        std::function<void(const std::string&)> errCallback);
};
