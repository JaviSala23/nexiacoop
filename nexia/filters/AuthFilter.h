#pragma once
#include <drogon/drogon.h>
using namespace drogon;

class AuthFilter : public drogon::HttpFilter<AuthFilter>
{
public:
    METHOD_LIST_BEGIN
    METHOD_LIST_END

    void doFilter(const drogon::HttpRequestPtr& req,
                  drogon::FilterCallback&&      fcb,
                  drogon::FilterChainCallback&& fccb) override;
};
