#pragma once
#include <string>

struct Parametro {
    int    id_parametro{0};
    double cuota_simple{5000.0};
    double cuota_con_responsable{8000.0};
    int    mes_cobranza{1};
    int    anio_cobranza{2026};
    std::string nombre_asociacion;
    std::string domicilio;
    std::string telefono;
    std::string localidad;
    std::string provincia;
    std::string email;
    std::string logo_url;
};
