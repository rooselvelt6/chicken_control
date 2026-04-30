#ifndef METRICAS_H
#define METRICAS_H

#include <vector>
#include "modelos.h"

class Metricas {
public:
    // Métricas por lote
    static MetricasLote calcularMetricasLote(int lote_id);
    static void imprimirMetricasLote(int lote_id);
    
    // Métricas por granja
    static MetricasGranja calcularMetricasGranja(int granja_id);
    static void imprimirMetricasGranja(int granja_id);
    
    // Dashboard general
    static void dashboardGeneral();
    static void dashboardGranja(int granja_id);
    static void dashboardLote(int lote_id);
    
    // Reportes específicos
    static void reporteMortalidad(int lote_id);
    static void reporteConversion(int lote_id);
    static void reporteCostos(int lote_id);
    static void reporteProduccion(int lote_id);
    
    // Comparativas
    static void compararLotes(int lote1_id, int lote2_id);
    static void rankingLotes(int limite = 10);
};

#endif // METRICAS_H