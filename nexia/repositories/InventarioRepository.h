#pragma once
#include <functional>
#include <vector>
#include <string>
#include "../models/ElementoInventario.h"
#include "../models/Prestamo.h"

class InventarioRepository {
public:
    // Elementos
    static void listarElementos(
        std::function<void(std::vector<ElementoInventario>)> callback,
        std::function<void(const std::string&)> errCallback);

    static void crearElemento(
        const ElementoInventario& el,
        std::function<void(int)> callback,
        std::function<void(const std::string&)> errCallback);

    static void actualizarElemento(
        const ElementoInventario& el,
        std::function<void()> callback,
        std::function<void(const std::string&)> errCallback);

    // Préstamos
    static void listarPrestamos(
        int idElemento, const std::string& estado,
        std::function<void(std::vector<Prestamo>)> callback,
        std::function<void(const std::string&)> errCallback);

    static void registrarPrestamo(
        const Prestamo& p,
        std::function<void(int)> callback,
        std::function<void(const std::string&)> errCallback);

    static void registrarDevolucion(
        int idPrestamo, const std::string& fechaDevolucion,
        std::function<void()> callback,
        std::function<void(const std::string&)> errCallback);
};
