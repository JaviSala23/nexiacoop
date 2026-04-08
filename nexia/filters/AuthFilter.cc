#include "AuthFilter.h"
#include <drogon/drogon.h>

void AuthFilter::doFilter(const drogon::HttpRequestPtr& req,
                           drogon::FilterCallback&& fcb,
                           drogon::FilterChainCallback&& fccb)
{
    auto session = req->getSession();
    if (session && session->find("usuario_id")) {
        fccb(); // usuario autenticado, continúa
        return;
    }
    // No autenticado: para peticiones API devolver 401, para páginas redirigir al login
    if (req->path().find("/api/") != std::string::npos) {
        Json::Value res;
        res["error"] = "No autenticado";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(res);
        resp->setStatusCode(drogon::k401Unauthorized);
        fcb(resp);
    } else {
        auto resp = drogon::HttpResponse::newRedirectionResponse("/login");
        fcb(resp);
    }
}
