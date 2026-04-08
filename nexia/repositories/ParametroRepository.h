#pragma once
#include <functional>
#include <drogon/drogon.h>
#include "../models/Parametro.h"

class ParametroRepository {
public:
    static void obtener(
        std::function<void(Parametro)> callback,
        std::function<void(const std::string&)> errCallback);

    static void actualizar(
        const Parametro& p,
        std::function<void()> callback,
        std::function<void(const std::string&)> errCallback);
};
