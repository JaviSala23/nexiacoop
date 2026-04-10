#include "TalonService.h"
#include "../repositories/FamiliaRepository.h"
#include "../repositories/TalonRepository.h"
#include "../repositories/ConceptoRepository.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

std::string TalonService::generarCodigoBarra(int numeroFamilia, int anio, int mes, int idConcepto)
{
    // Formato: NNNNNNAAAAMMCCC (N=numero_familia, A=año, M=mes, C=id_concepto) = 15 chars
    // Usa numero_familia (el número visible del usuario) igual que los talones extras
    std::ostringstream oss;
    oss << std::setw(6) << std::setfill('0') << numeroFamilia
        << std::setw(4) << std::setfill('0') << anio
        << std::setw(2) << std::setfill('0') << mes
        << std::setw(3) << std::setfill('0') << idConcepto;
    return oss.str();
}

// -------------------------------------------------------
// Generar talones mensuales para todas las familias activas
// -------------------------------------------------------
void TalonService::generarMensual(int mes, int anio,
    std::function<void(int)> callback,
    std::function<void(const std::string&)> errCallback)
{
    // 1. Obtener concepto MENSUAL
    ConceptoRepository::buscarMensual(
        [mes, anio, callback, errCallback](const Concepto& concepto) {
            int idConcepto = concepto.id_concepto;

            // 2. Listar familias activas
            FamiliaRepository::listar("ACTIVA",
                [mes, anio, idConcepto, callback, errCallback](std::vector<Familia> familias) {
                    if (familias.empty()) {
                        errCallback("No hay familias activas");
                        return;
                    }
                    // Recolectar IDs de todas las familias con alumnos
                    std::vector<int> todosIds;
                    for (const auto& f : familias)
                        if (f.cantidad_alumnos > 0) todosIds.push_back(f.id_familia);

                    if (todosIds.empty()) {
                        errCallback("Ninguna familia tiene alumnos activos");
                        return;
                    }

                    // 3. Ver cuáles ya tienen talón activo (no anulado)
                    TalonRepository::familiasConTalonActivo(mes, anio, idConcepto, todosIds,
                        [mes, anio, idConcepto, familias, callback, errCallback](std::vector<int> yaConTalon) {
                            // 4. Filtrar: solo familias SIN talón activo
                            std::vector<Familia> aProcesar;
                            std::vector<int>     idsAProcesar;
                            for (const auto& f : familias) {
                                if (f.cantidad_alumnos <= 0) continue;
                                bool tieneActivo = std::find(yaConTalon.begin(), yaConTalon.end(), f.id_familia) != yaConTalon.end();
                                if (!tieneActivo) {
                                    aProcesar.push_back(f);
                                    idsAProcesar.push_back(f.id_familia);
                                }
                            }

                            if (aProcesar.empty()) {
                                errCallback("Ya existen cuotas mensuales generadas para " +
                                            std::to_string(mes) + "/" + std::to_string(anio));
                                return;
                            }

                            // 5. Limpiar talones ANULADOS de esas familias para ese período
                            TalonRepository::eliminarAnuladosDeFamilias(mes, anio, idConcepto, idsAProcesar,
                                [mes, anio, idConcepto, aProcesar, callback, errCallback]() {
                                    // 6. Para cada familia, calcular cuota
                                    auto talones = std::make_shared<std::vector<Talon>>();
                                    auto idx     = std::make_shared<size_t>(0);
                                    auto total   = aProcesar.size();

                                    auto procesarSiguiente = std::make_shared<std::function<void()>>();
                                    *procesarSiguiente = [procesarSiguiente, mes, anio, idConcepto, aProcesar, talones, idx, total, callback, errCallback]() mutable {
                                        if (*idx >= total) {
                                            int cnt = static_cast<int>(talones->size());
                                            TalonRepository::insertarBatch(*talones,
                                                [cnt, callback]() { callback(cnt); },
                                                errCallback);
                                            return;
                                        }
                                        const Familia& fam = aProcesar[*idx];
                                        (*idx)++;
                                        ConceptoRepository::obtenerMontoPorHijos(fam.cantidad_alumnos,
                                            [mes, anio, idConcepto, fam, talones, procesarSiguiente, errCallback](double monto) mutable {
                                                Talon t;
                                                t.id_familia   = fam.id_familia;
                                                t.id_concepto  = idConcepto;
                                                t.mes          = mes;
                                                t.anio         = anio;
                                                t.monto        = monto;
                                                t.codigo_barra = TalonService::generarCodigoBarra(
                                                    fam.numero_familia, anio, mes, idConcepto);
                                                talones->push_back(std::move(t));
                                                (*procesarSiguiente)();
                                            },
                                            errCallback);
                                    };
                                    (*procesarSiguiente)();
                                },
                                errCallback);
                        },
                        errCallback);
                },
                errCallback);
        },
        errCallback);
}

// -------------------------------------------------------
// Generar para familias específicas
// -------------------------------------------------------
void TalonService::generarParaFamilias(int mes, int anio,
    std::vector<int> familiaIds,
    std::function<void(int)> callback,
    std::function<void(const std::string&)> errCallback)
{
    ConceptoRepository::buscarMensual(
        [mes, anio, familiaIds, callback, errCallback](const Concepto& concepto) {
            int idConcepto = concepto.id_concepto;
            TalonRepository::familiasConTalonActivo(mes, anio, idConcepto, familiaIds,
                [mes, anio, idConcepto, familiaIds, callback, errCallback](std::vector<int> yaGenerados) {
                    FamiliaRepository::listar("ACTIVA",
                        [mes, anio, idConcepto, familiaIds, yaGenerados, callback, errCallback](std::vector<Familia> familias) {
                            auto lista = std::make_shared<std::vector<Familia>>();
                            std::vector<int> idsAProcesar;
                            for (const auto& f : familias) {
                                bool pedida = std::find(familiaIds.begin(), familiaIds.end(), f.id_familia) != familiaIds.end();
                                bool existe  = std::find(yaGenerados.begin(), yaGenerados.end(), f.id_familia) != yaGenerados.end();
                                if (pedida && !existe && f.cantidad_alumnos > 0) {
                                    lista->push_back(f);
                                    idsAProcesar.push_back(f.id_familia);
                                }
                            }
                            if (lista->empty()) { callback(0); return; }

                            // Limpiar talones ANULADOS de estas familias para el período
                            TalonRepository::eliminarAnuladosDeFamilias(mes, anio, idConcepto, idsAProcesar,
                                [mes, anio, idConcepto, lista, callback, errCallback]() {
                                    auto talones = std::make_shared<std::vector<Talon>>();
                                    auto idx   = std::make_shared<size_t>(0);
                                    auto total = lista->size();
                                    auto proc  = std::make_shared<std::function<void()>>();
                                    *proc = [proc, mes, anio, idConcepto, lista, talones, idx, total, callback, errCallback]() mutable {
                                        if (*idx >= total) {
                                            int cnt = static_cast<int>(talones->size());
                                            TalonRepository::insertarBatch(*talones,
                                                [cnt, callback]() { callback(cnt); }, errCallback);
                                            return;
                                        }
                                        const Familia& fam = (*lista)[*idx];
                                        (*idx)++;
                                        ConceptoRepository::obtenerMontoPorHijos(fam.cantidad_alumnos,
                                            [mes, anio, idConcepto, fam, talones, proc, errCallback](double monto) mutable {
                                                Talon t;
                                                t.id_familia   = fam.id_familia;
                                                t.id_concepto  = idConcepto;
                                                t.mes          = mes;
                                                t.anio         = anio;
                                                t.monto        = monto;
                                                t.codigo_barra = TalonService::generarCodigoBarra(
                                                    fam.numero_familia, anio, mes, idConcepto);
                                                talones->push_back(std::move(t));
                                                (*proc)();
                                            }, errCallback);
                                    };
                                    (*proc)();
                                },
                                errCallback);
                        }, errCallback);
                }, errCallback);
        }, errCallback);
}

// -------------------------------------------------------
// Generar talón de concepto EXTRA para una familia
// -------------------------------------------------------
void TalonService::generarExtra(int idFamilia, int idConcepto, double monto,
    const std::string& observaciones,
    std::function<void(int)> callback,
    std::function<void(const std::string&)> errCallback)
{
    FamiliaRepository::buscarPorId(idFamilia,
        [idConcepto, monto, observaciones, callback, errCallback](const Familia& fam) {
            // Código de barra único: familia + concepto + timestamp (truncado a segundos)
            auto now = std::chrono::system_clock::now();
            auto ts  = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
            std::ostringstream oss;
            oss << std::setw(6) << std::setfill('0') << fam.numero_familia
                << std::setw(3) << std::setfill('0') << idConcepto
                << (ts % 100000000LL); // 8 dígitos del timestamp
            std::string codigo = oss.str();

            Talon t;
            t.id_familia   = fam.id_familia;
            t.id_concepto  = idConcepto;
            t.mes          = 0; // sin mes específico para extras
            t.anio         = 0;
            t.monto        = monto;
            t.codigo_barra = codigo;
            t.observaciones = observaciones;

            TalonRepository::insertar(t, callback, errCallback);
        },
        errCallback);
}

// -------------------------------------------------------
// Generar talones de concepto EXTRA para TODAS las familias activas
// -------------------------------------------------------
void TalonService::generarExtraBatch(int idConcepto, double monto,
    const std::string& observaciones,
    std::function<void(int)> callback,
    std::function<void(const std::string&)> errCallback)
{
    FamiliaRepository::listar("ACTIVA",
        [idConcepto, monto, observaciones, callback, errCallback](std::vector<Familia> familias) {
            if (familias.empty()) { errCallback("No hay familias activas"); return; }
            auto now = std::chrono::system_clock::now();
            auto ts  = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
            std::vector<Talon> talones;
            talones.reserve(familias.size());
            for (const auto& fam : familias) {
                std::ostringstream oss;
                oss << std::setw(6) << std::setfill('0') << fam.numero_familia
                    << std::setw(3) << std::setfill('0') << idConcepto
                    << std::setw(8) << std::setfill('0') << ((ts + fam.id_familia) % 100000000LL);
                Talon t;
                t.id_familia    = fam.id_familia;
                t.id_concepto   = idConcepto;
                t.mes           = 0;
                t.anio          = 0;
                t.monto         = monto;
                t.codigo_barra  = oss.str();
                t.observaciones = observaciones;
                talones.push_back(std::move(t));
            }
            int cnt = static_cast<int>(talones.size());
            TalonRepository::insertarBatch(talones,
                [cnt, callback]() { callback(cnt); },
                errCallback);
        },
        errCallback);
}
