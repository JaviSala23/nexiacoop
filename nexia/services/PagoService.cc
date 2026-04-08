#include "PagoService.h"
#include "../repositories/TalonRepository.h"
#include "../repositories/PagoRepository.h"

void PagoService::registrarPorCodigo(
    const std::string& codigoBarra,
    int idCobradora,
    const std::string& observaciones,
    std::function<void(Json::Value)> callback,
    std::function<void(const std::string&)> errCallback)
{
    // Buscar talón por código
    TalonRepository::buscarPorCodigo(codigoBarra,
        [idCobradora, observaciones, callback, errCallback](const Talon& t) {
            if (t.estado == "PAGADO") {
                errCallback("El talón ya fue pagado");
                return;
            }
            if (t.estado == "ANULADO") {
                errCallback("El talón está anulado");
                return;
            }
            // Marcar como pagado
            TalonRepository::marcarPagado(t.id_talon,
                [t, idCobradora, observaciones, callback, errCallback]() {
                    // Insertar registro de pago; usa la cobradora del talón si no se especifica
                    Pago p;
                    p.id_talon      = t.id_talon;
                    p.id_cobradora  = (idCobradora > 0) ? idCobradora : t.id_cobradora;
                    p.monto         = t.monto;
                    p.observaciones = observaciones;
                    PagoRepository::insertar(p,
                        [t, callback](int idPago) {
                            Json::Value res;
                            res["id_pago"]     = idPago;
                            res["socio"]       = t.nombre_socio;
                            res["monto"]       = t.monto;
                            res["codigo"]      = t.codigo_barra;
                            res["mensaje"]     = "Pago registrado correctamente";
                            callback(res);
                        },
                        errCallback);
                },
                errCallback);
        },
        errCallback);
}
