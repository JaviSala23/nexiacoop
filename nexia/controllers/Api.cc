
#include <drogon/drogon.h>
#include "Api.h"

void Api::status(const HttpRequestPtr &req,
                 std::function<void (const HttpResponsePtr &)> &&callback)
{
    Json::Value json;
    json["sistema"] = "Nexia";
    json["estado"] = "OK";

    auto resp = drogon::HttpResponse::newHttpJsonResponse(json);
    callback(resp);
}