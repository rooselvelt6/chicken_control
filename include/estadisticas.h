#ifndef ESTADISTICAS_H
#define ESTADISTICAS_H

#include <vector>
#include <string>
#include <utility>

struct Prediccion {
    double valor_predicho;
    double confianza;  // 0-100%
    std::vector<std::pair<int, double>> valores_historicos;
    std::vector<std::pair<int, double>> valores_predichos;
};

struct AnalisisCorrelacion {
    double coeficiente_correlacion;  // -1 a 1
    std::string interpretacion;
    std::vector<std::pair<double, double>> puntos;
};

struct Anomalia {
    int dia;
    std::string tipo;  // "peso_bajo", "mortalidad_alta", "consumo_anormal"
    double valor_esperado;
    double valor_real;
    double desviacion;
};

class Estadisticas {
public:
    // Regresión lineal: y = mx + b
    static std::pair<double, double> regresionLineal(const std::vector<double>& x, 
                                                   const std::vector<double>& y);
    
    // Predicciones
    static Prediccion predecirPeso(int lote_id, int dias_futuros = 7);
    static Prediccion predecirMortalidad(int lote_id, int dias_futuros = 7);
    static Prediccion predecirConsumoAlimento(int lote_id, int dias_futuros = 7);
    
    // Análisis de correlación
    static AnalisisCorrelacion correlacionTemperaturaMortalidad(int lote_id);
    static AnalisisCorrelacion correlacionPesajeConsumo(int lote_id);
    static AnalisisCorrelacion correlacionHumedadCrecimiento(int lote_id);
    
    // Detección de anomalías
    static std::vector<Anomalia> detectarAnomaliasPeso(int lote_id);
    static std::vector<Anomalia> detectarAnomaliasMortalidad(int lote_id);
    static std::vector<Anomalia> detectarAnomaliasConsumo(int lote_id);
    
    // Estadísticas descriptivas
    static double calcularMedia(const std::vector<double>& valores);
    static double calcularMediana(const std::vector<double>& valores);
    static double calcularDesviacionEstandar(const std::vector<double>& valores);
    static double calcularVarianza(const std::vector<double>& valores);
    static std::pair<double, double> calcularMinMax(const std::vector<double>& valores);
    
    // Análisis de tendencias
    static std::string analizarTendencia(const std::vector<double>& valores);
    static double calcularTasaCrecimiento(const std::vector<double>& valores);
    static bool esTendenciaAlcista(const std::vector<double>& valores, int ventana = 5);
    
    // Proyecciones
    static Prediccion proyectarVentas(int año, int meses_futuros = 3);
    static Prediccion proyectarProduccion(int lote_id, int semanas_futuras = 4);
    
    // Reportes
    static void mostrarReporteEstadistico(int lote_id);
    static void exportarEstadisticas(const std::string& archivo, int lote_id);
};

#endif // ESTADISTICAS_H
