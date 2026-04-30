#ifndef PESAJE_H
#define PESAJE_H

#include <vector>
#include "modelos.h"

class Pesaje {
public:
    static int registrar(int lote_id, int semana, const std::string& fecha,
                         const std::string& pesos_str, const std::string& observacion);
    static std::vector<PesajeSemanal> listarPorLote(int lote_id);
    static PesajeSemanal* obtener(int id);
    
    // Estadísticas
    static double obtenerPesoPromedio(int lote_id);
    static double obtenerPesoSemana(int lote_id, int semana);
    static bool estaEnPeso(int lote_id); // true si el peso está en el rango objetivo
    static double getPesoObjetivo(int semana);
    
    // Reportes
    static void imprimirReporte(int lote_id);
    static void imprimirEvolucion(int lote_id);
};

#endif // PESAJE_H