#ifndef REPORTES_TXT_H
#define REPORTES_TXT_H

#include <string>

class ReportesTXT {
public:
    static void exportarEstadoResultados(const std::string& archivo, const std::string& fecha_inicio, const std::string& fecha_fin);
    static void exportarFlujoCaja(const std::string& archivo, int anio);
    static void exportarCostosPorLote(const std::string& archivo);
    static void exportarMetricasLote(const std::string& archivo, int lote_id);
    static void exportarDashboard(const std::string& archivo);
};

#endif