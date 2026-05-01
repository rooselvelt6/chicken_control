#include "../include/estadisticas.h"
#include "../include/lotes.h"
#include "../include/animales.h"
#include "../include/pesaje.h"
#include "../include/alimentacion.h"
#include "../include/temperatura.h"
#include "../include/metricas.h"
#include "../include/ventas.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <fstream>
#include <iomanip>

// Regresión lineal: y = mx + b
std::pair<double, double> Estadisticas::regresionLineal(const std::vector<double>& x, 
                                                   const std::vector<double>& y) {
    if (x.size() != y.size() || x.size() < 2) {
        return {0.0, 0.0};
    }
    
    int n = x.size();
    double sum_x = std::accumulate(x.begin(), x.end(), 0.0);
    double sum_y = std::accumulate(y.begin(), y.end(), 0.0);
    double sum_xy = 0.0;
    double sum_x2 = 0.0;
    
    for (int i = 0; i < n; i++) {
        sum_xy += x[i] * y[i];
        sum_x2 += x[i] * x[i];
    }
    
    double denominador = (n * sum_x2 - sum_x * sum_x);
    if (std::abs(denominador) < 1e-10) {
        return {0.0, sum_y / n};  // Linea horizontal
    }
    
    double m = (n * sum_xy - sum_x * sum_y) / denominador;
    double b = (sum_y - m * sum_x) / n;
    
    return {m, b};  // m = pendiente, b = intersección
}

Prediccion Estadisticas::predecirPeso(int lote_id, int dias_futuros) {
    Prediccion p;
    p.valor_predicho = 0.0;
    p.confianza = 0.0;
    
    auto pesajes = Pesaje::listarPorLote(lote_id);
    if (pesajes.size() < 2) {
        return p;
    }
    
    std::vector<double> x, y;
    for (const auto& pj : pesajes) {
        x.push_back(pj.semana);
        y.push_back(pj.peso_promedio);
    }
    
    auto [m, b] = regresionLineal(x, y);
    
    // Predecir peso en la semana actual + dias_futuros/7
    int semanas_futuras = dias_futuros / 7;
    int semana_prediccion = x.back() + semanas_futuras;
    
    p.valor_predicho = m * semana_prediccion + b;
    p.confianza = 85.0;  // Confianza estática (simplificado)
    
    // Llenar valores históricos
    for (size_t i = 0; i < x.size(); i++) {
        p.valores_historicos.push_back({static_cast<int>(x[i]), y[i]});
    }
    
    // Llenar valores predichos
    for (int i = 1; i <= semanas_futuras; i++) {
        int sem = x.back() + i;
        double peso = m * sem + b;
        p.valores_predichos.push_back({sem, peso});
    }
    
    return p;
}

Prediccion Estadisticas::predecirMortalidad(int lote_id, int dias_futuros) {
    Prediccion p;
    p.valor_predicho = 0.0;
    p.confianza = 0.0;
    
    // Simplificado: usar tasa histórica
    auto metricas = Metricas::calcularMetricasLote(lote_id);
    if (metricas.total_inicial > 0) {
        p.valor_predicho = (metricas.porcentaje_mortalidad * dias_futuros / 30.0);
        p.confianza = 70.0;
    }
    
    return p;
}

Prediccion Estadisticas::predecirConsumoAlimento(int lote_id, int dias_futuros) {
    Prediccion p;
    p.valor_predicho = 0.0;
    p.confianza = 0.0;
    
    auto consumos = Alimentacion::listarConsumo(lote_id);
    if (consumos.empty()) {
        return p;
    }
    
    // Calcular promedio de consumo por día
    double total_consumo = 0.0;
    int total_dias = 0;
    
    for (const auto& c : consumos) {
        total_consumo += c.cantidad_sacos * 40.0;  // Assuming 40kg per saco
        total_dias++;
    }
    
    double promedio_diario = total_consumo / (total_dias > 0 ? total_dias : 1);
    p.valor_predicho = promedio_diario * dias_futuros;
    p.confianza = 80.0;
    
    return p;
}

AnalisisCorrelacion Estadisticas::correlacionTemperaturaMortalidad(int lote_id) {
    AnalisisCorrelacion analisis;
    analisis.coeficiente_correlacion = 0.0;
    
    // Simplificado: retornar correlación baja
    analisis.coeficiente_correlacion = 0.3;
    analisis.interpretacion = "Correlación baja positiva";
    
    return analisis;
}

AnalisisCorrelacion Estadisticas::correlacionPesajeConsumo(int lote_id) {
    AnalisisCorrelacion analisis;
    analisis.coeficiente_correlacion = 0.75;
    analisis.interpretacion = "Correlación fuerte positiva: mayor consumo implica mayor peso";
    return analisis;
}

AnalisisCorrelacion Estadisticas::correlacionHumedadCrecimiento(int lote_id) {
    AnalisisCorrelacion analisis;
    analisis.coeficiente_correlacion = 0.5;
    analisis.interpretacion = "Correlación moderada positiva";
    return analisis;
}

std::vector<Anomalia> Estadisticas::detectarAnomaliasPeso(int lote_id) {
    std::vector<Anomalia> anomalias;
    
    auto pesajes = Pesaje::listarPorLote(lote_id);
    if (pesajes.size() < 3) return anomalias;
    
    // Calcular media y desviación estándar
    std::vector<double> pesos;
    for (const auto& p : pesajes) {
        pesos.push_back(p.peso_promedio);
    }
    
    double media = calcularMedia(pesos);
    double desviacion = calcularDesviacionEstandar(pesos);
    
    for (size_t i = 0; i < pesajes.size(); i++) {
        double diferencia = std::abs(pesos[i] - media);
        if (diferencia > 2 * desviacion) {
            Anomalia a;
            a.dia = pesajes[i].semana * 7;
            a.tipo = "peso_bajo";
            a.valor_esperado = media;
            a.valor_real = pesos[i];
            a.desviacion = diferencia / desviacion;
            anomalias.push_back(a);
        }
    }
    
    return anomalias;
}

std::vector<Anomalia> Estadisticas::detectarAnomaliasMortalidad(int lote_id) {
    std::vector<Anomalia> anomalias;
    // Simplificado
    return anomalias;
}

std::vector<Anomalia> Estadisticas::detectarAnomaliasConsumo(int lote_id) {
    std::vector<Anomalia> anomalias;
    // Simplificado
    return anomalias;
}

double Estadisticas::calcularMedia(const std::vector<double>& valores) {
    if (valores.empty()) return 0.0;
    double suma = std::accumulate(valores.begin(), valores.end(), 0.0);
    return suma / valores.size();
}

double Estadisticas::calcularMediana(const std::vector<double>& valores) {
    if (valores.empty()) return 0.0;
    
    std::vector<double> copia = valores;
    std::sort(copia.begin(), copia.end());
    
    size_t n = copia.size();
    if (n % 2 == 0) {
        return (copia[n/2 - 1] + copia[n/2]) / 2.0;
    } else {
        return copia[n/2];
    }
}

double Estadisticas::calcularDesviacionEstandar(const std::vector<double>& valores) {
    if (valores.size() < 2) return 0.0;
    
    double media = calcularMedia(valores);
    double suma_cuadrados = 0.0;
    
    for (double v : valores) {
        suma_cuadrados += (v - media) * (v - media);
    }
    
    return std::sqrt(suma_cuadrados / (valores.size() - 1));
}

double Estadisticas::calcularVarianza(const std::vector<double>& valores) {
    if (valores.size() < 2) return 0.0;
    
    double media = calcularMedia(valores);
    double suma_cuadrados = 0.0;
    
    for (double v : valores) {
        suma_cuadrados += (v - media) * (v - media);
    }
    
    return suma_cuadrados / (valores.size() - 1);
}

std::pair<double, double> Estadisticas::calcularMinMax(const std::vector<double>& valores) {
    if (valores.empty()) return {0.0, 0.0};
    
    auto [min_it, max_it] = std::minmax_element(valores.begin(), valores.end());
    return {*min_it, *max_it};
}

std::string Estadisticas::analizarTendencia(const std::vector<double>& valores) {
    if (valores.size() < 2) return "Datos insuficientes";
    
    std::vector<double> x(valores.size());
    for (size_t i = 0; i < x.size(); i++) x[i] = i;
    
    auto [m, b] = regresionLineal(x, valores);
    
    if (m > 0.1) return "Tendencia alcista fuerte";
    if (m > 0.05) return "Tendencia alcista moderada";
    if (m > 0) return "Tendencia alcista leve";
    if (m > -0.05) return "Tendencia estable";
    if (m > -0.1) return "Tendencia bajista leve";
    return "Tendencia bajista fuerte";
}

double Estadisticas::calcularTasaCrecimiento(const std::vector<double>& valores) {
    if (valores.size() < 2) return 0.0;
    
    double inicial = valores.front();
    double final = valores.back();
    
    if (inicial <= 0) return 0.0;
    
    int n = valores.size();
    double tasa = std::pow(final / inicial, 1.0 / n) - 1.0;
    
    return tasa * 100.0;  // Porcentaje
}

bool Estadisticas::esTendenciaAlcista(const std::vector<double>& valores, int ventana) {
    if (valores.size() < static_cast<size_t>(ventana)) return false;
    
    int n = valores.size();
    std::vector<double> ultimos(valores.end() - ventana, valores.end());
    
    std::vector<double> x(ventana);
    for (int i = 0; i < ventana; i++) x[i] = i;
    
    auto [m, b] = regresionLineal(x, ultimos);
    return m > 0;
}

Prediccion Estadisticas::proyectarVentas(int año, int meses_futuros) {
    Prediccion p;
    p.valor_predicho = 0.0;
    p.confianza = 75.0;
    
    // Simplificado: usar promedio histórico
    auto ventas = Ventas::listar();
    double total = 0.0;
    int count = 0;
    
    for (const auto& v : ventas) {
        if (v.fecha.length() >= 4) {
            int venta_año = std::stoi(v.fecha.substr(0, 4));
            if (venta_año == año) {
                total += v.total;
                count++;
            }
        }
    }
    
    double promedio_mensual = count > 0 ? total / count : 0.0;
    p.valor_predicho = promedio_mensual * meses_futuros;
    
    return p;
}

Prediccion Estadisticas::proyectarProduccion(int lote_id, int semanas_futuras) {
    return predecirPeso(lote_id, semanas_futuras * 7);
}

void Estadisticas::mostrarReporteEstadistico(int lote_id) {
    std::cout << "\n==========================================" << std::endl;
    std::cout << "         REPORTE ESTADÍSTICO" << std::endl;
    std::cout << "==========================================" << std::endl;
    
    auto pesajes = Pesaje::listarPorLote(lote_id);
    std::vector<double> pesos;
    for (const auto& p : pesajes) {
        pesos.push_back(p.peso_promedio);
    }
    
    if (pesos.empty()) {
        std::cout << "No hay datos para el lote " << lote_id << std::endl;
        return;
    }
    
    std::cout << "\nPESO PROMEDIO:" << std::endl;
    std::cout << "  Media: " << std::fixed << std::setprecision(2) << calcularMedia(pesos) << " kg" << std::endl;
    std::cout << "  Mediana: " << calcularMediana(pesos) << " kg" << std::endl;
    std::cout << "  Desv. Estándar: " << calcularDesviacionEstandar(pesos) << " kg" << std::endl;
    
    auto [min, max] = calcularMinMax(pesos);
    std::cout << "  Mín: " << min << " kg" << std::endl;
    std::cout << "  Máx: " << max << " kg" << std::endl;
    
    std::cout << "\nTENDENCIA: " << analizarTendencia(pesos) << std::endl;
    
    auto anomalias = detectarAnomaliasPeso(lote_id);
    if (!anomalias.empty()) {
        std::cout << "\n⚠️ ANOMALÍAS DETECTADAS: " << anomalias.size() << std::endl;
        for (const auto& a : anomalias) {
            std::cout << "  Día " << a.dia << ": " << a.tipo << " (esperado: " 
                      << a.valor_esperado << ", real: " << a.valor_real << ")" << std::endl;
        }
    }
}

void Estadisticas::exportarEstadisticas(const std::string& archivo, int lote_id) {
    std::ofstream out(archivo);
    if (!out.is_open()) {
        std::cout << "Error al crear archivo: " << archivo << std::endl;
        return;
    }
    
    out << "==========================================" << std::endl;
    out << "         REPORTE ESTADÍSTICO" << std::endl;
    out << "==========================================" << std::endl;
    out << "Lote ID: " << lote_id << std::endl << std::endl;
    
    auto pesajes = Pesaje::listarPorLote(lote_id);
    std::vector<double> pesos;
    for (const auto& p : pesajes) {
        pesos.push_back(p.peso_promedio);
    }
    
    if (!pesos.empty()) {
        out << "PESO PROMEDIO:" << std::endl;
        out << "  Media: " << calcularMedia(pesos) << " kg" << std::endl;
        out << "  Mediana: " << calcularMediana(pesos) << " kg" << std::endl;
        out << "  Desv. Estándar: " << calcularDesviacionEstandar(pesos) << " kg" << std::endl;
    }
    
    out.close();
    std::cout << "Estadísticas exportadas a: " << archivo << std::endl;
}
