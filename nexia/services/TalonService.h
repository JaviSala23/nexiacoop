#pragma once
#include <vector>
#include <functional>
#include <json/json.h>
#include "../models/Talon.h"
#include "../models/Socio.h"
#include "../models/Parametro.h"

class TalonService {
public:
    // Genera talones para todos los socios activos del mes/año indicado
    static void generarMensual(
        int mes, int anio,
        std::function<void(int cantGenerados)> callback,
        std::function<void(const std::string&)> errCallback);

    // Genera talones solo para los socios indicados (por id_socio)
    // Omite los que ya tienen talón activo para ese período
    static void generarParaSocios(
        int mes, int anio,
        std::vector<int> socioIds,
        std::function<void(int cantGenerados)> callback,
        std::function<void(const std::string&)> errCallback);

    // Genera el código de barras: SSSSSS-AAAA-MM-T
    static std::string generarCodigoBarra(int numSocio, int anio, int mes, const std::string& tipo);
};
