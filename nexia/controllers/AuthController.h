#pragma once
#include <drogon/drogon.h>
using namespace drogon;

class AuthController : public drogon::HttpController<AuthController>
{
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(AuthController::loginPage,     "/login",           Get);
    ADD_METHOD_TO(AuthController::dashboardPage, "/dashboard",       Get, "AuthFilter");
    ADD_METHOD_TO(AuthController::login,         "/api/auth/login",  Post);
    ADD_METHOD_TO(AuthController::logout,        "/api/auth/logout", Post);
    ADD_METHOD_TO(AuthController::me,            "/api/auth/me",     Get, "AuthFilter");
    METHOD_LIST_END

    void loginPage(const drogon::HttpRequestPtr& req,
                   std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void dashboardPage(const drogon::HttpRequestPtr& req,
                       std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void login(const drogon::HttpRequestPtr& req,
               std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void logout(const drogon::HttpRequestPtr& req,
                std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void me(const drogon::HttpRequestPtr& req,
            std::function<void(const drogon::HttpResponsePtr&)>&& callback);
};
