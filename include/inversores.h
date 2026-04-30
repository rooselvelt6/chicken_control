#ifndef INVERSORES_H
#define INVERSORES_H

#include <vector>
#include <utility>
#include "modelos.h"

class Inversores {
public:
    static int agregar(const std::string& nombre, double cantidad_invertida);
    static Inversor* obtener(int id);
    static Inversor* obtenerPorNombre(const std::string& nombre);
    static std::vector<Inversor> listar();
    static void actualizar(int id, const std::string& nombre, double cantidad_invertida);
    static void eliminar(int id);
    static double totalInvertido();
    static std::vector<std::pair<std::string, double>> gananciasPorInversor(double total_ganancias);
    static int agregarEncargado(const std::string& nombre);
    static std::vector<Encargado> listarEncargados();
    static void eliminarEncargado(int id);
    static void cargarEjemplo();
};

#endif // INVERSORES_H