#include "PagoService.h"
#include "../repositories/TalonRepository.h"
#include "../repositories/PagoRepository.h"
#include "../repositories/ContabilidadRepository.h"
#include "../repositories/ConceptoRepository.h"
#include <chrono>
#include <iomanip>
#include <sstream>

void PagoService::registrarPorCodigo(
    const std::string& codigoBarra,
    const std::string& medioPago,
    const std::string& observaciones,
    std::function<void(Json::Value)> callback,
    std::function<void(const std::string&)> errCallback)
{
    // 1. Buscar talón por código de barra
    TalonRepository::buscarPorCodigo(codigoBarra,
        [medioPago, observaciones, callback, errCallback](const Talon& t) {
            if (t.estado == "PAGADO") { errCallback("El talón ya fue pagado"); return; }
            if (t.estado == "ANULADO") { errCallback("El talón está anulado"); return; }

            // 2. Marcar talón como PAGADO
            TalonRepository::marcarPagado(t.id_talon,
                [t, medioPago, observaciones, callback, errCallback]() {
                    // 3. Registrar pago
                    Pago p;
                    p.id_talon      = t.id_talon;
                    p.monto         = t.monto;
                    p.medio_pago    = medioPago.empty() ? "EFECTIVO" : medioPago;
                    p.observaciones = observaciones;

                    PagoRepository::insertar(p,
                        [t, p, callback, errCallback](int idPago) {
                            // 4. Crear movimiento contable automático (INGRESO)
                            // Buscar la cuenta de ingreso apropiada según concepto
                            // Por defecto usamos la primera cuenta de INGRESO
                            ContabilidadRepository::listarCuentas(
                                [t, p, idPago, callback, errCallback](std::vector<PlanCuenta> cuentas) {
                                    int idCuenta = 0;
                                    // Buscar una cuenta activa de INGRESO
                                    for (const auto& c : cuentas) {
                                        if (c.tipo == "INGRESO" && c.activo) {
                                            idCuenta = c.id_cuenta;
                                            // Intentar hacer match con el tipo de concepto
                                            if (t.tipo_concepto == "MENSUAL" && c.codigo == "ING-001") break;
                                            if (t.nombre_concepto.find("Matr") != std::string::npos && c.codigo == "ING-002") break;
                                            if (t.nombre_concepto.find("Merienda") != std::string::npos && c.codigo == "ING-003") break;
                                            if (t.nombre_concepto.find("Viaje") != std::string::npos && c.codigo == "ING-004") break;
                                        }
                                    }
                                    if (idCuenta == 0) {
                                        // No hay plan de cuentas → responder con OK igual
                                        Json::Value res;
                                        res["id_pago"]  = idPago;
                                        res["familia"]  = t.nombre_tutor_principal;
                                        res["concepto"] = t.nombre_concepto;
                                        res["monto"]    = t.monto;
                                        res["codigo"]   = t.codigo_barra;
                                        res["mensaje"]  = "Pago registrado";
                                        callback(res);
                                        return;
                                    }

                                    // Fecha de hoy
                                    auto now = std::chrono::system_clock::now();
                                    auto tt  = std::chrono::system_clock::to_time_t(now);
                                    std::ostringstream oss;
                                    oss << std::put_time(std::localtime(&tt), "%Y-%m-%d");

                                    Movimiento mv;
                                    mv.id_cuenta   = idCuenta;
                                    mv.fecha       = oss.str();
                                    mv.descripcion = "Pago de " + t.nombre_concepto +
                                                     " - Familia Nº " + std::to_string(t.numero_familia) +
                                                     " - " + t.nombre_tutor_principal;
                                    mv.monto      = t.monto;
                                    mv.tipo       = "INGRESO";
                                    mv.medio_pago = p.medio_pago;
                                    mv.id_pago    = idPago;

                                    ContabilidadRepository::insertarMovimiento(mv,
                                        [t, idPago, callback](int /*idMov*/) {
                                            Json::Value res;
                                            res["id_pago"]  = idPago;
                                            res["familia"]  = t.nombre_tutor_principal;
                                            res["concepto"] = t.nombre_concepto;
                                            res["monto"]    = t.monto;
                                            res["codigo"]   = t.codigo_barra;
                                            res["mensaje"]  = "Pago e ingreso contable registrados";
                                            callback(res);
                                        },
                                        [t, idPago, callback](const std::string& /*e*/) {
                                            // Si falla el movimiento contable, el pago ya quedó. Responder OK de todas formas.
                                            Json::Value res;
                                            res["id_pago"]  = idPago;
                                            res["familia"]  = t.nombre_tutor_principal;
                                            res["concepto"] = t.nombre_concepto;
                                            res["monto"]    = t.monto;
                                            res["codigo"]   = t.codigo_barra;
                                            res["mensaje"]  = "Pago registrado (sin movimiento contable)";
                                            callback(res);
                                        });
                                },
                                [t, idPago, callback](const std::string& /*e*/) {
                                    Json::Value res;
                                    res["id_pago"]  = idPago;
                                    res["mensaje"]  = "Pago registrado";
                                    callback(res);
                                });
                        },
                        errCallback);
                },
                errCallback);
        },
        errCallback);
}
