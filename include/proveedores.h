#ifndef PROVEEDORES_H
#define PROVEEDORES_H

#include <vector>
#include "modelos.h"

class Proveedores {
public:
    static int agregar(const std::string& nombre, const std::string& telefono,
                       const std::string& email, const std::string& direccion,
                       const std::string& tipo, int rating);
    static Proveedor* obtener(int id);
    static Proveedor* obtenerPorNombre(const std::string& nombre);
    static std::vector<Proveedor> listar(const std::string& filtro_tipo = "");
    static std::vector<Proveedor> listarPorCategoria(const std::string& categoria);
    static void actualizar(int id, const std::string& nombre, const std::string& telefono,
                         const std::string& email, const std::string& tipo, int rating);
    static void eliminar(int id);
    static void imprimirProveedor(const Proveedor& p);
};

#endif // PROVEEDORES_H