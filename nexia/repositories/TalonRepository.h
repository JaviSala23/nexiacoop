#pragma once
#include <vector>
#include <string>
#include <functional>
#include <drogon/drogon.h>
#include "../models/Talon.h"

class TalonRepository {
public:
    static void listar(
        int mes, int anio, int idCobradora,
        std::function<void(std::vector<Talon>)> callback,
        std::function<void(const std::string&)> errCallback);

    static void buscarPorCodigo(
        const std::string& codigo,
        std::function<void(Talon)> callback,
        std::function<void(const std::string&)> errCallback);

    static void insertarBatch(
        const std::vector<Talon>& talones,
        std::function<void()> callback,
        std::function<void(const std::string&)> errCallback);

    static void marcarPagado(
        int idTalon,
        std::function<void()> callback,
        std::function<void(const std::string&)> errCallback);

    static void anular(
        int idTalon,
        std::function<void()> callback,
        std::function<void(const std::string&)> errCallback);

    static void existeMesAnio(
        int mes, int anio,
        std::function<void(bool)> callback,
        std::function<void(const std::string&)> errCallback);

    static void eliminarAnuladosMesAnio(
        int mes, int anio,
        std::function<void()> callback,
        std::function<void(const std::string&)> errCallback);

    static void sociosConTalonActivo(
        int mes, int anio,
        const std::vector<int>& socioIds,
        std::function<void(std::vector<int>)> callback,
        std::function<void(const std::string&)> errCallback);

    static void eliminarAnuladosDeSocios(
        int mes, int anio,
        const std::vector<int>& socioIds,
        std::function<void()> callback,
        std::function<void(const std::string&)> errCallback);

    static void listarMorosos(
        std::function<void(Json::Value)> callback,
        std::function<void(const std::string&)> errCallback);
};
