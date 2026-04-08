
#include <drogon/drogon.h>
using namespace drogon;

class Api : public drogon::HttpController<Api>
{
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(Api::status,"/api/status",Get);
    METHOD_LIST_END

    void status(const HttpRequestPtr &req,
                std::function<void (const HttpResponsePtr &)> &&callback);
};