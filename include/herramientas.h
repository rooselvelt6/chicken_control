#ifndef HERRAMIENTAS_H
#define HERRAMIENTAS_H

#include <vector>
#include "modelos.h"

class Herramientas {
public:
    static int agregar(const std::string& nombre, int cantidad, double precio_unitario, const std::string& tipo);
    static Herramienta* obtener(int id);
    static std::vector<Herramienta> listar();
    static void actualizar(int id, const std::string& nombre, int cantidad, double precio_unitario);
    static void eliminar(int id);
    static double totalInvertido();
    static void cargarEjemplo();
};

#endif // HERRAMIENTAS_H