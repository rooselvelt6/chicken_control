#ifndef LOTES_H
#define LOTES_H

#include <vector>
#include "modelos.h"

class Lotes {
public:
    static int crear(int numero, const std::string& fecha_inicio, int duracion);
    static Lote* obtener(int id);
    static std::vector<Lote> listar(bool activos = true);
    static void cerrar(int id, const std::string& fecha_fin);
    static void actualizar(int id, int numero, const std::string& fecha_inicio, int duracion);
    static void eliminar(int id);
    static Lote* loteActivoActual();
    static int ultimoNumero();
    static void imprimirLote(const Lote& l);
};

#endif // LOTES_H