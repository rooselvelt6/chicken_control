#include "../include/metricas.h"
#include "../include/base_datos.h"
#include "../include/modelos.h"
#include "../include/lotes.h"
#include "../include/animales.h"
#include "../include/alimentacion.h"
#include "../include/ventas.h"
#include "../include/pesaje.h"
#include "../include/utils.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>

MetricasLote Metricas::calcularMetricasLote(int lote_id) {
    MetricasLote m;
    m.lote_id = lote_id;
    
    Lote* lote = Lotes::obtener(lote_id);
    if (!lote) return m;
    
    // Información básica
    m.corral_id = lote->corral_id;
    m.nombre_corral = "Lote " + std::to_string(lote->numero);
    
    // Animales
    int total_inicial = Animales::obtenerCantidadTotal(lote_id);
    int total_muertes = Animales::totalMuertes(lote_id);
    auto [sacrificados, kg_sacrificados] = Animales::totalSacrificios(lote_id);
    double inversion_animales = Animales::totalInvertido(lote_id);
    
    m.total_inicial = total_inicial;
    m.total_muertes = total_muertes;
    m.total_sacrificados = sacrificados;
    m.porcentaje_mortalidad = (total_inicial > 0) ? (double)total_muertes / total_inicial * 100 : 0;
    
    // Peso
    auto pesos = Ventas::listarVentasPorLote(lote_id);
    double kg_vendidos = 0;
    int pollos_vendidos = 0;
    for (auto& v : pesos) {
        for (double p : v.pesos) {
            kg_vendidos += p;
            pollos_vendidos++;
        }
    }
    m.total_vendidos = pollos_vendidos;
    m.kg_producidos = kg_vendidos + kg_sacrificados;
    
    if (pollos_vendidos > 0) {
        m.peso_promedio_final = kg_vendidos / pollos_vendidos;
    }
    m.peso_promedio_objetivo = 2.2; // Objetivo a los 45 días
    m.peso_ok = (std::abs(m.peso_promedio_final - m.peso_promedio_objetivo) <= 0.3);
    
    // Alimentación
    auto [sacos_consumidos, costo_alimento] = Alimentacion::totalConsumo(lote_id);
    m.total_alimento_kg = sacos_consumidos * 40.0;
    m.conversion_alimenticia = (m.kg_producidos > 0) ? m.total_alimento_kg / m.kg_producidos : 0;
    
    // Costos
    m.costo_animales = inversion_animales;
    m.costo_alimento = costo_alimento;
    m.costo_total = m.costo_animales + m.costo_alimento;
    
    // Ventas
    double ingreso_ventas = Ventas::totalVentas(lote_id);
    m.ingreso_ventas = ingreso_ventas;
    m.ganancia = m.ingreso_ventas - m.costo_total;
    m.rentabilidad_porcentaje = (m.costo_total > 0) ? m.ganancia / m.costo_total * 100 : 0;
    
    // Costo por kg
    m.costo_por_kg = (m.kg_producidos > 0) ? m.costo_total / m.kg_producidos : 0;
    
    return m;
}

void Metricas::imprimirMetricasLote(int lote_id) {
    MetricasLote m = calcularMetricasLote(lote_id);
    
    printHeader("MÉTRICAS DEL LOTE " + std::to_string(lote_id));
    
    std::cout << "--- ANIMALES ---" << std::endl;
    std::cout << "Total inicial: " << m.total_inicial << std::endl;
    std::cout << "Muertes: " << m.total_muertes << " (" << std::fixed << std::setprecision(1) << m.porcentaje_mortalidad << "%)" << std::endl;
    std::cout << "Sacrificados: " << m.total_sacrificados << std::endl;
    std::cout << "Vendidos: " << m.total_vendidos << std::endl << std::endl;
    
    std::cout << "--- PESO ---" << std::endl;
    std::cout << "Peso promedio final: " << std::fixed << std::setprecision(2) << m.peso_promedio_final << " kg" << std::endl;
    std::cout << "Peso objetivo: " << m.peso_promedio_objetivo << " kg" << std::endl;
    std::cout << "Estado: " << (m.peso_ok ? "✓ OK" : "✗ FUERA DE PESO") << std::endl << std::endl;
    
    std::cout << "--- ALIMENTACIÓN ---" << std::endl;
    std::cout << "Total consumido: " << std::fixed << std::setprecision(1) << m.total_alimento_kg << " kg" << std::endl;
    std::cout << "Conversión alimenticia: " << std::setprecision(2) << m.conversion_alimenticia << std::endl;
    std::cout << "(ideal: 1.5 - 1.8)" << std::endl << std::endl;
    
    std::cout << "--- COSTOS ---" << std::endl;
    std::cout << "Animales: Bs. " << std::fixed << std::setprecision(2) << m.costo_animales << std::endl;
    std::cout << "Alimento: Bs. " << m.costo_alimento << std::endl;
    std::cout << "TOTAL: Bs. " << m.costo_total << std::endl;
    std::cout << "Costo por kg: Bs. " << m.costo_por_kg << std::endl << std::endl;
    
    std::cout << "--- VENTAS ---" << std::endl;
    std::cout << "Ingreso: Bs. " << m.ingreso_ventas << std::endl;
    std::cout << "Ganancia: Bs. " << m.ganancia << std::endl;
    std::cout << "Rentabilidad: " << std::setprecision(1) << m.rentabilidad_porcentaje << "%" << std::endl;
}

void Metricas::dashboardGeneral() {
    printHeader("DASHBOARD GENERAL");
    
    auto lotes = Lotes::listar();
    std::cout << "Total de lotes: " << lotes.size() << std::endl;
    
    int total_animales = 0;
    double total_inversion = 0;
    double total_ingresos = 0;
    double total_muertes = 0;
    
    for (auto& lote : lotes) {
        total_animales += Animales::obtenerCantidadTotal(lote.id);
        total_inversion += Animales::totalInvertido(lote.id);
        total_muertes += Animales::totalMuertes(lote.id);
    }
    
    std::cout << std::endl;
    std::cout << "--- RESUMEN ---" << std::endl;
    std::cout << "Animales en sistema: " << total_animales << std::endl;
    std::cout << "Inversión total: Bs. " << std::fixed << std::setprecision(2) << total_inversion << std::endl;
    std::cout << "Muertes totales: " << total_muertes << std::endl;
    if (total_animales > 0) {
        std::cout << "Tasa mortalidad: " << std::setprecision(2) << (total_muertes * 100.0 / total_animales) << "%" << std::endl;
    }
}

void Metricas::dashboardLote(int lote_id) {
    imprimirMetricasLote(lote_id);
    std::cout << std::endl;
    Pesaje::imprimirReporte(lote_id);
}

void Metricas::reporteMortalidad(int lote_id) {
    printHeader("REPORTE DE MORTALIDAD - LOTE " + std::to_string(lote_id));
    auto muertes = Animales::listarMuertes(lote_id);
    
    int total = 0;
    for (auto& m : muertes) {
        std::cout << "- " << causaMuerteToString(m.causa) << ": " << m.cantidad << " (" << m.fecha << ")" << std::endl;
        total += m.cantidad;
    }
    
    int total_inicial = Animales::obtenerCantidadTotal(lote_id);
    std::cout << std::endl << "Total: " << total << " (" << std::fixed << std::setprecision(1) 
              << (total_inicial > 0 ? total * 100.0 / total_inicial : 0) << "%)" << std::endl;
}

void Metricas::reporteCostos(int lote_id) {
    MetricasLote m = calcularMetricasLote(lote_id);
    
    printHeader("REPORTE DE COSTOS - LOTE " + std::to_string(lote_id));
    
    std::cout << "Costo animales: Bs. " << std::fixed << std::setprecision(2) << m.costo_animales << std::endl;
    std::cout << "Costo alimento: Bs. " << m.costo_alimento << std::endl;
    std::cout << "Costo total: Bs. " << m.costo_total << std::endl;
    std::cout << "Costo por kg producido: Bs. " << m.costo_por_kg << std::endl;
}