#pragma once
#include <vector>
#include <functional>
#include <string>
#include <json/json.h>
#include "../models/Rifa.h"
#include "../models/RifaNumero.h"
#include "../models/RifaCuota.h"
#include "../models/RifaPremio.h"

class RifaRepository {
public:
    // Rifas
    static void listar(
        std::function<void(std::vector<Rifa>)> callback,
        std::function<void(const std::string&)> errCallback);

    static void obtener(int idRifa,
        std::function<void(Rifa)> callback,
        std::function<void(const std::string&)> errCallback);

    static void insertar(const Rifa& r,
        std::function<void(int)> callback,
        std::function<void(const std::string&)> errCallback);

    static void actualizar(const Rifa& r,
        std::function<void()> callback,
        std::function<void(const std::string&)> errCallback);

    // Números de rifa
    static void listarNumeros(int idRifa,
        std::function<void(std::vector<RifaNumero>)> callback,
        std::function<void(const std::string&)> errCallback);

    static void numerosDisponibles(int idRifa,
        std::function<void(Json::Value)> callback,
        std::function<void(const std::string&)> errCallback);

    static void asignarNumero(const RifaNumero& rn,
        std::function<void(int idNumero)> callback,
        std::function<void(const std::string&)> errCallback);

    static void asignarATodas(int idRifa,
        std::function<void(int cantAsignados)> callback,
        std::function<void(const std::string&)> errCallback);

    static void anularNumero(int idNumero,
        std::function<void()> callback,
        std::function<void(const std::string&)> errCallback);

    static void actualizarNumero(int idNumero, int idFamilia,
        const std::string& nombreExt, const std::string& telExt,
        std::function<void()> callback,
        std::function<void(const std::string&)> errCallback);

    // Cuotas
    static void listarCuotas(int idNumero,
        std::function<void(Json::Value)> callback,
        std::function<void(const std::string&)> errCallback);

    static void pagarCuotas(const std::vector<int>& idCuotas, const std::string& medioPago,
        std::function<void(int idMovimiento)> callback,
        std::function<void(const std::string&)> errCallback);

    // Sorteo / premios
    static void sortearRifa(int idRifa, const std::vector<RifaPremio>& premios,
        int idCuentaGasto,
        std::function<void(Json::Value resultado)> callback,
        std::function<void(const std::string&)> errCallback);

    static void listarPremios(int idRifa,
        std::function<void(Json::Value)> callback,
        std::function<void(const std::string&)> errCallback);
};
