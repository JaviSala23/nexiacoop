#pragma once
#include <vector>
#include <functional>
#include <string>
#include "../models/Talon.h"

class TalonService {
public:
    // Genera talones mensuales para todas las familias activas
    static void generarMensual(int mes, int anio,
        std::function<void(int cantGenerados)> callback,
        std::function<void(const std::string&)> errCallback);

    // Genera talones de cuota mensual solo para las familias indicadas
    static void generarParaFamilias(int mes, int anio,
        std::vector<int> familiaIds,
        std::function<void(int cantGenerados)> callback,
        std::function<void(const std::string&)> errCallback);

    // Genera un talón de concepto extra para una familia específica
    static void generarExtra(int idFamilia, int idConcepto, double monto,
        const std::string& observaciones,
        std::function<void(int idTalon)> callback,
        std::function<void(const std::string&)> errCallback);

    // Genera talones de concepto extra para TODAS las familias activas
    static void generarExtraBatch(int idConcepto, double monto,
        const std::string& observaciones,
        std::function<void(int cantGenerados)> callback,
        std::function<void(const std::string&)> errCallback);

    // Genera talones de cuota mensual para familias específicas durante N meses consecutivos
    static void generarPeriodo(int mesInicio, int anioInicio, int numMeses,
        std::vector<int> familiaIds,
        std::function<void(int cantTotal)> callback,
        std::function<void(const std::string&)> errCallback);

    // Código de barra mensual: NNNNNNAAAAMMCCC (N=numero_familia, A=anio, M=mes, C=id_concepto)
    static std::string generarCodigoBarra(int numeroFamilia, int anio, int mes, int idConcepto);
};

