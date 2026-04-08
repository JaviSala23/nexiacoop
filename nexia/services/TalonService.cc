#include "TalonService.h"
#include "../repositories/SocioRepository.h"
#include "../repositories/TalonRepository.h"
#include "../repositories/ParametroRepository.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

std::string TalonService::generarCodigoBarra(
    int numSocio, int anio, int mes, const std::string& tipo)
{
    // Formato: SSSSSS-AAAA-MM-T  (S=número socio, T=1 simple, 2 con cargo)
    std::ostringstream oss;
    oss << std::setw(6) << std::setfill('0') << numSocio
        << std::setw(4) << std::setfill('0') << anio
        << std::setw(2) << std::setfill('0') << mes
        << (tipo == "SIMPLE" ? "1" : "2");
    return oss.str();
}

void TalonService::generarMensual(
    int mes, int anio,
    std::function<void(int)> callback,
    std::function<void(const std::string&)> errCallback)
{
    // 1. Verificar que ya no existen talones activos para ese mes/año
    TalonRepository::existeMesAnio(mes, anio,
        [mes, anio, callback, errCallback](bool existe) {
            if (existe) {
                errCallback("Ya existen talones generados para " +
                            std::to_string(mes) + "/" + std::to_string(anio));
                return;
            }
            // 1b. Eliminar talones ANULADOS de ese mes/año (para liberar el código de barra)
            TalonRepository::eliminarAnuladosMesAnio(mes, anio,
              [mes, anio, callback, errCallback]() {
            // 2. Obtener parámetros
            ParametroRepository::obtener(
                [mes, anio, callback, errCallback](const Parametro& param) {
                    // 3. Obtener todos los socios activos
                    SocioRepository::listar("ACTIVO", 0,
                        [mes, anio, param, callback, errCallback](std::vector<Socio> socios) {
                            if (socios.empty()) {
                                errCallback("No hay socios activos");
                                return;
                            }
                            std::vector<Talon> talones;
                            for (const auto& s : socios) {
                                // Saltar socios anexos (tienen socio principal)
                                if (s.id_socio_principal > 0) continue;
                                Talon t;
                                t.id_socio     = s.id_socio;
                                t.id_cobradora = s.id_cobradora; // 0 → NULL en DB
                                t.mes          = mes;
                                t.anio         = anio;
                                t.tipo         = s.tiene_responsable ? "CON_CARGO" : "SIMPLE";
                                // CON_CARGO = cuota socio + cuota responsable = cuota_simple * 2
                                t.monto        = s.tiene_responsable
                                                 ? param.cuota_simple * 2.0
                                                 : param.cuota_simple;
                                t.codigo_barra = TalonService::generarCodigoBarra(
                                    s.numero_socio, anio, mes, t.tipo);
                                talones.push_back(std::move(t));
                            }
                            int total = static_cast<int>(talones.size());
                            TalonRepository::insertarBatch(talones,
                                [total, callback]() { callback(total); },
                                errCallback);
                        },
                        errCallback);
                },
                errCallback);
              },  // cierre lambda eliminarAnuladosMesAnio
              errCallback);
        },
        errCallback);
}

void TalonService::generarParaSocios(
    int mes, int anio,
    std::vector<int> socioIds,
    std::function<void(int)> callback,
    std::function<void(const std::string&)> errCallback)
{
    // 1. Eliminar anulados de esos socios para liberar el código de barra
    TalonRepository::eliminarAnuladosDeSocios(mes, anio, socioIds,
      [mes, anio, socioIds, callback, errCallback]() {
        // 2. Ver cuáles ya tienen talón activo
        TalonRepository::sociosConTalonActivo(mes, anio, socioIds,
          [mes, anio, socioIds, callback, errCallback](std::vector<int> yaGenerados) {
            // 3. Parámetros
            ParametroRepository::obtener(
              [mes, anio, socioIds, yaGenerados, callback, errCallback](const Parametro& param) {
                // 4. Socios activos
                SocioRepository::listar("ACTIVO", 0,
                  [mes, anio, socioIds, yaGenerados, param, callback, errCallback](std::vector<Socio> socios) {
                    std::vector<Talon> talones;
                    for (const auto& s : socios) {
                        bool pedido = std::find(socioIds.begin(), socioIds.end(), s.id_socio) != socioIds.end();
                        bool existe = std::find(yaGenerados.begin(), yaGenerados.end(), s.id_socio) != yaGenerados.end();
                        // Saltar anexos (tienen socio principal)
                        if (!pedido || existe || s.id_socio_principal > 0) continue;
                        Talon t;
                        t.id_socio     = s.id_socio;
                        t.id_cobradora = s.id_cobradora;
                        t.mes = mes; t.anio = anio;
                        t.tipo  = s.tiene_responsable ? "CON_CARGO" : "SIMPLE";
                        t.monto = s.tiene_responsable ? param.cuota_simple * 2.0 : param.cuota_simple;
                        t.codigo_barra = TalonService::generarCodigoBarra(s.numero_socio, anio, mes, t.tipo);
                        talones.push_back(std::move(t));
                    }
                    if (talones.empty()) {
                        errCallback("Todos los socios seleccionados ya tienen talón activo para ese período");
                        return;
                    }
                    int total = static_cast<int>(talones.size());
                    TalonRepository::insertarBatch(talones,
                        [total, callback]() { callback(total); },
                        errCallback);
                  },
                  errCallback);
              }, errCallback);
          }, errCallback);
      }, errCallback);
}
