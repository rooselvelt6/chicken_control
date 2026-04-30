#ifndef ANIMALES_H
#define ANIMALES_H

#include <vector>
#include "modelos.h"

class Animales {
public:
    static int agregar(int lote_id, int cantidad, double precio_unitario, double peso_promedio, const std::string& fecha_ingreso);
    static Animal* obtener(int id);
    static std::vector<Animal> listarPorLote(int lote_id);
    static int obtenerCantidadTotal(int lote_id);
    static int registrarMuerte(int lote_id, CausaMuerte causa, int cantidad, const std::string& fecha);
    static std::vector<Muerte> listarMuertes(int lote_id);
    static int totalMuertes(int lote_id);
    static int registrarSacrificio(int lote_id, int cantidad, double peso_total, const std::string& fecha);
    static std::vector<Sacrificio> listarSacrificios(int lote_id);
    static std::pair<int, double> totalSacrificios(int lote_id);
    static double totalInvertido(int lote_id);
    static double calcularPerdidasMuertes(int lote_id);
    static std::pair<int, double> animalesVivos(int lote_id);
    static void imprimirResumen(int lote_id);
};

#endif // ANIMALES_H