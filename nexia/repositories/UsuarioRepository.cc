#include "UsuarioRepository.h"

using namespace drogon;
using namespace drogon::orm;

void UsuarioRepository::listar(
    std::function<void(std::vector<Usuario>)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "SELECT id_usuario, username, nombre, rol, activo, created_at FROM usuarios ORDER BY username",
        [callback](const Result& r) {
            std::vector<Usuario> lista;
            for (const auto& row : r) {
                Usuario u;
                u.id_usuario  = row["id_usuario"].as<int>();
                u.username    = row["username"].as<std::string>();
                u.nombre      = row["nombre"].isNull() ? "" : row["nombre"].as<std::string>();
                u.rol         = row["rol"].as<std::string>();
                u.activo      = row["activo"].as<bool>();
                u.created_at  = row["created_at"].as<std::string>();
                lista.push_back(std::move(u));
            }
            callback(std::move(lista));
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); });
}

void UsuarioRepository::buscarPorUsername(
    const std::string& username,
    std::function<void(Usuario, bool)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "SELECT * FROM usuarios WHERE username=? AND activo=1",
        [callback](const Result& r) {
            if (r.empty()) { callback({}, false); return; }
            const auto& row = r[0];
            Usuario u;
            u.id_usuario   = row["id_usuario"].as<int>();
            u.username     = row["username"].as<std::string>();
            u.password_hash= row["password_hash"].as<std::string>();
            u.nombre       = row["nombre"].isNull() ? "" : row["nombre"].as<std::string>();
            u.rol          = row["rol"].as<std::string>();
            u.activo       = row["activo"].as<bool>();
            callback(std::move(u), true);
        },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        username);
}

void UsuarioRepository::insertar(
    const Usuario& u,
    std::function<void(int)> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "INSERT INTO usuarios (username, password_hash, nombre, rol) VALUES (?,?,?,?)",
        [callback](const Result& r) { callback(static_cast<int>(r.insertId())); },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        u.username, u.password_hash, u.nombre, u.rol);
}

void UsuarioRepository::actualizarPassword(
    int id,
    const std::string& passwordHash,
    std::function<void()> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "UPDATE usuarios SET password_hash=? WHERE id_usuario=?",
        [callback](const Result&) { callback(); },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        passwordHash, id);
}

void UsuarioRepository::cambiarEstado(
    int id,
    bool activo,
    std::function<void()> callback,
    std::function<void(const std::string&)> errCallback)
{
    auto db = app().getDbClient("main");
    db->execSqlAsync(
        "UPDATE usuarios SET activo=? WHERE id_usuario=?",
        [callback](const Result&) { callback(); },
        [errCallback](const DrogonDbException& e) { errCallback(e.base().what()); },
        activo ? 1 : 0, id);
}
