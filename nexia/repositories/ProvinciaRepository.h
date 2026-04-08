#pragma once
#include <vector>
#include <string>
#include <functional>
#include <drogon/drogon.h>

struct Provincia {
    int id_provincia{0};
    std::string nombre;
};

struct Localidad {
    int id_localidad{0};
    int id_provincia{0};
    std::string nombre;
    std::string nombre_provincia;
};

class ProvinciaRepository {
public:
    static void listarProvincias(
        std::function<void(std::vector<Provincia>)> cb,
        std::function<void(const std::string&)> err);

    static void listarLocalidades(
        int idProvincia,
        std::function<void(std::vector<Localidad>)> cb,
        std::function<void(const std::string&)> err);

    static void insertarLocalidad(
        const std::string& nombre, int idProvincia,
        std::function<void(int)> cb,
        std::function<void(const std::string&)> err);

    static void eliminarLocalidad(
        int idLocalidad,
        std::function<void()> cb,
        std::function<void(const std::string&)> err);

    static void insertarProvincia(
        const std::string& nombre,
        std::function<void(int)> cb,
        std::function<void(const std::string&)> err);

    static void eliminarProvincia(
        int idProvincia,
        std::function<void()> cb,
        std::function<void(const std::string&)> err);
};
