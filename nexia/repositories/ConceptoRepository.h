#pragma once
#include <functional>
#include <vector>
#include <string>
#include "../models/Concepto.h"
#include "../models/CuotaConfig.h"

class ConceptoRepository {
public:
    static void listar(
        std::function<void(std::vector<Concepto>)> callback,
        std::function<void(const std::string&)> errCallback);

    static void buscarPorId(int id,
        std::function<void(Concepto)> callback,
        std::function<void(const std::string&)> errCallback);

    static void buscarMensual(
        std::function<void(Concepto)> callback,
        std::function<void(const std::string&)> errCallback);

    static void insertar(const Concepto& c,
        std::function<void(int)> callback,
        std::function<void(const std::string&)> errCallback);

    static void actualizar(const Concepto& c,
        std::function<void()> callback,
        std::function<void(const std::string&)> errCallback);

    static void toggleActivo(int id,
        std::function<void()> callback,
        std::function<void(const std::string&)> errCallback);

    // Cuota config
    static void listarCuotasConfig(
        std::function<void(std::vector<CuotaConfig>)> callback,
        std::function<void(const std::string&)> errCallback);

    static void obtenerMontoPorHijos(int cantHijos,
        std::function<void(double)> callback,
        std::function<void(const std::string&)> errCallback);

    static void upsertCuotaConfig(int cantHijos, double monto,
        std::function<void()> callback,
        std::function<void(const std::string&)> errCallback);

    static void eliminarCuotaConfig(int idConfig,
        std::function<void()> callback,
        std::function<void(const std::string&)> errCallback);
};
