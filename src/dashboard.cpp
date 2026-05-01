#include "../include/dashboard.h"
#include "../include/base_datos.h"
#include "../include/lotes.h"
#include "../include/animales.h"
#include "../include/alimentacion.h"
#include "../include/ventas.h"
#include "../include/pesaje.h"
#include "../include/metricas.h"
#include "../include/veterinaria.h"
#include "../include/inversores.h"
#include "../include/herramientas.h"
#include <iostream>
#include <iomanip>
#include <sstream>

std::string Dashboard::generarBarra(int valor, int max, int longitud) {
    if (max == 0) max = 1;
    int chars = (valor * longitud) / max;
    std::string barra = "[";
    for (int i = 0; i < longitud; i++) {
        barra += (i < chars) ? "█" : "░";
    }
    barra += "]";
    return barra;
}

void Dashboard::mostrarPanelPrincipal() {
    auto lotes = Lotes::listar();
    auto alimentos = Alimentacion::listar();
    auto ventas = Ventas::listar();
    
    int lotesActivos = 0;
    int pollosActuales = 0;
    for (const auto& l : lotes) {
        if (l.activo) {
            lotesActivos++;
            auto animales = Animales::listarPorLote(l.id);
            for (const auto& a : animales) {
                pollosActuales += a.cantidad;
            }
        }
    }
    
    int totalAlimentos = 0;
    for (const auto& a : alimentos) {
        totalAlimentos += a.inventario;
    }
    
    double totalVentas = 0;
    int ventasPendientes = 0;
    for (const auto& v : ventas) {
        totalVentas += v.total;
        if (!v.pagada) ventasPendientes++;
    }
    
    std::cout << R"(
╔══════════════════════════════════════════════════════════════════╗
║                    🐔 DASHBOARD GRANOAVICOLA                      ║
╠══════════════════════════════════════════════════════════════════╣
║  PRODUCCIÓN                                                  LOTE ║
╠─────────────────────────────────────────────────────────────────────╣
║  Lotes Activos:    )" << std::setw(5) << lotesActivos << std::setw(45) << "░░░░░░░░░░░░░░░░░░░░░" << R"(
║  Pollos en Granja: )" << std::setw(5) << pollosActuales << std::setw(45) << "░░░░░░░░░░░░░░░░░░░░░" << R"(
╠─────────────────────────────────────────────────────────────────────╣
║  INVENTARIO                                                  ALIM ║
╠─────────────────────────────────────────────────────────────────────╣
║  Total Sacos:      )" << std::setw(5) << totalAlimentos << std::setw(45) << generarBarra(totalAlimentos, 200) << R"(
╠─────────────────────────────────────────────────────────────────────╣
║  VENTAS                                                       DIN $║
║  Total Ingresos:   USD " << std::fixed << std::setprecision(2) << std::setw(8) << totalVentas << std::setw(38) << "░░░░░░░░░░░░░░░░░░" << R"(
║  Ventas Pendientes:" << std::setw(3) << ventasPendientes << std::setw(53) << "░░░░░░░░░░░░░░░░░░░░" << R"(
╠══════════════════════════════════════════════════════════════════╣
║  [1] Panel Lotes    [2] Panel Financiero   [3] Panel Producción    ║
║  [4] Gráfico Crecimiento        [5] Predicción    [0] Salir        ║
╚══════════════════════════════════════════════════════════════════╝
)";
}

void Dashboard::mostrarPanelLotes() {
    auto lotes = Lotes::listar();
    std::cout << "\n╔══════════════════════════════════════════════════════════════════╗";
    std::cout << "\n║                    PANEL DE LOTES                               ║";
    std::cout << "\n╠════════╦══════════╦═════════╦══════════╦══════════╦═══════════════╣";
    std::cout << "\n║  Lote  ║  Estado  ║ Pollos  ║ Peso Prom║  Fase    ║ Mortalidad    ║";
    std::cout << "\n╠════════╬══════════╬═════════╬══════════╬══════════╬═══════════════╣";
    
    for (const auto& l : lotes) {
        auto animales = Animales::listarPorLote(l.id);
        auto muertes = Animales::listarMuertes(l.id);
        
        int totalAnimales = 0;
        double pesoProm = 0;
        for (const auto& a : animales) {
            totalAnimales += a.cantidad;
            pesoProm = a.peso_promedio;
        }
        
        int totalMuertes = 0;
        for (const auto& m : muertes) {
            totalMuertes += m.cantidad;
        }
        
        double mort = (totalAnimales > 0) ? (totalMuertes * 100.0 / totalAnimales) : 0;
        
        std::cout << "\n║   #" << std::setw(3) << l.numero;
        std::cout << " ║ " << std::setw(8) << (l.activo ? "Activo" : "Cerrado");
        std::cout << " ║ " << std::setw(7) << totalAnimales;
        std::cout << " ║ " << std::fixed << std::setprecision(1) << std::setw(7) << pesoProm;
        std::cout << " ║ " << std::setw(8) << (l.activo ? "En curso" : "Finalizado");
        std::cout << " ║ " << std::fixed << std::setprecision(1) << std::setw(6) << mort << "%";
        std::cout << "   ║";
    }
    std::cout << "\n╚════════╩══════════╩═════════╩══════════╩══════════╩═══════════════╝\n";
}

void Dashboard::mostrarPanelFinanciero() {
    auto ventas = Ventas::listar();
    auto inversores = Inversores::listar();
    auto herramientas = Herramientas::listar();
    
    double totalVentas = 0;
    double totalCobrado = 0;
    double totalPendiente = 0;
    
    for (const auto& v : ventas) {
        totalVentas += v.total;
        totalCobrado += v.abonado;
        totalPendiente += v.pendiente;
    }
    
    double totalHerramientas = Herramientas::totalInvertido();
    double totalInversores = Inversores::totalInvertido();
    
    std::cout << R"(
╔══════════════════════════════════════════════════════════════════╗
║                    PANEL FINANCIERO                              ║
╠══════════════════════════════════════════════════════════════════╣
║  RESUMEN DE VENTAS                                              ║
║  ─────────────────────────────────────────────────────────────── ║
)";
    std::cout << "║  Total Ventas:     USD " << std::fixed << std::setprecision(2) << std::setw(10) << totalVentas << "                         ║\n";
    std::cout << "║  Total Cobrado:    USD " << std::fixed << std::setprecision(2) << std::setw(10) << totalCobrado << "                         ║\n";
    std::cout << "║  Pendiente:        USD " << std::fixed << std::setprecision(2) << std::setw(10) << totalPendiente << "                         ║\n";
    std::cout << R"(
║  INVERSIONES                                                   ║
║  ─────────────────────────────────────────────────────────────── ║
)";
    std::cout << "║  Inversores:       USD " << std::fixed << std::setprecision(2) << std::setw(10) << totalInversores << "                         ║\n";
    std::cout << "║  Herramientas:     USD " << std::fixed << std::setprecision(2) << std::setw(10) << totalHerramientas << "                         ║\n";
    std::cout << "║  ─────────────────────────────────────────────────────────────── ║\n";
    std::cout << "║  TOTAL INVERTIDO:  USD " << std::fixed << std::setprecision(2) << std::setw(10) << (totalInversores + totalHerramientas) << "                         ║\n";
    std::cout << R"(
╚══════════════════════════════════════════════════════════════════╝
)";
}

void Dashboard::mostrarPanelProduccion() {
    auto lotes = Lotes::listar();
    auto alimentos = Alimentacion::listar();
    auto vacunas = Veterinaria::listarVacunas();
    auto medicamentos = Veterinaria::listarMedicamentos();
    
    int totalSacos = 0;
    for (const auto& a : alimentos) {
        totalSacos += a.inventario;
    }
    
    int totalStockVacunas = 0;
    for (const auto& v : vacunas) {
        totalStockVacunas++;
    }
    
    int totalStockMedicamentos = 0;
    for (const auto& m : medicamentos) {
        totalStockMedicamentos += m.stock;
    }
    
    std::cout << R"(
╔══════════════════════════════════════════════════════════════════╗
║                    PANEL DE PRODUCCIÓN                         ║
╠══════════════════════════════════════════════════════════════════╣
║  ALIMENTACIÓN                                                  ║
║  ─────────────────────────────────────────────────────────────── ║
)";
    std::cout << "║  Sacos en inventario: " << std::setw(5) << totalSacos << "                                     ║\n";
    std::cout << R"(
║  VETERINARIA                                                   ║
║  ─────────────────────────────────────────────────────────────── ║
)";
    std::cout << "║  Vacunas disponibles: " << std::setw(3) << totalStockVacunas << "                                        ║\n";
    std::cout << "║  Medicamentos en stock: " << std::setw(3) << totalStockMedicamentos << "                                        ║\n";
    std::cout << R"(
║  GRUPOS DE EDAD                                                ║
║  ─────────────────────────────────────────────────────────────── ║
)";
    
    int bebes = 0, intermedios = 0, grandes = 0;
    for (const auto& l : lotes) {
        if (l.activo) {
            auto animales = Animales::listarPorLote(l.id);
            for (const auto& a : animales) {
                if (a.fase == Fase::Bebe) bebes += a.cantidad;
                else if (a.fase == Fase::Intermedio) intermedios += a.cantidad;
                else if (a.fase == Fase::Grande) grandes += a.cantidad;
            }
        }
    }
    std::cout << "║  pollitos (Bebé):      " << std::setw(5) << bebes << "                                     ║\n";
    std::cout << "║ /pollos (Intermedio):  " << std::setw(5) << intermedios << "                                     ║\n";
    std::cout << "║ /pollos (Grande):     " << std::setw(5) << grandes << "                                     ║\n";
    std::cout << R"(
╚══════════════════════════════════════════════════════════════════╝
)";
}

void Dashboard::mostrarGraficoCrecimiento(int lote_id) {
    auto pesaje = Pesaje::listarPorLote(lote_id);
    if (pesaje.empty()) {
        std::cout << "No hay datos de pesaje para el lote " << lote_id << std::endl;
        return;
    }
    
    std::cout << "\n╔══════════════════════════════════════════════════════════════════╗";
    std::cout << "\n║          CURVA DE CRECIMIENTO - LOTE #" << lote_id << "                      ║";
    std::cout << "\n╠════════╦═══════════════════╦═══════════════════╦══════════════════╣";
    std::cout << "\n║ Semana ║ Peso Real         ║ Peso Objetivo    ║ Progreso        ║";
    std::cout << "\n╠════════╬═══════════════════╬═══════════════════╬══════════════════╣";
    
    for (const auto& p : pesaje) {
        double objetivo = pesoObjetivoSemana(p.semana);
        int progreso = (objetivo > 0) ? (int)((p.peso_promedio * 100) / objetivo) : 0;
        if (progreso > 100) progreso = 100;
        
        std::cout << "\n║   " << std::setw(2) << p.semana << "   ║";
        std::cout << " " << std::fixed << std::setprecision(2) << std::setw(7) << p.peso_promedio << " kg      ║";
        std::cout << " " << std::fixed << std::setprecision(2) << std::setw(7) << objetivo << " kg      ║";
        std::cout << " " << generarBarra(progreso, 100, 14) << "║";
    }
    std::cout << "\n╚════════╩═══════════════════╩═══════════════════╩══════════════════╝\n";
}

void Dashboard::mostrarProyeccion(int lote_id) {
    auto pesaje = Pesaje::listarPorLote(lote_id);
    if (pesaje.empty()) {
        std::cout << "No hay datos suficientes para proyectar." << std::endl;
        return;
    }
    
    auto animales = Animales::listarPorLote(lote_id);
    int totalPollos = 0;
    for (const auto& a : animales) {
        totalPollos += a.cantidad;
    }
    
    double pesoActual = 0;
    for (const auto& p : pesaje) {
        pesoActual = p.peso_promedio;
    }
    
    // Proyección simple: semana 7 con peso objetivo
    double pesoSemana7 = pesoObjetivoSemana(7);
    double pesoTotalProyectado = totalPollos * pesoSemana7;
    double pesoActualTotal = totalPollos * pesoActual;
    
    double crecimientoRestante = pesoTotalProyectado - pesoActualTotal;
    double porcentajeFaltante = (pesoTotalProyectado > 0) ? (crecimientoRestante * 100 / pesoTotalProyectado) : 0;
    
    std::cout << R"(
╔══════════════════════════════════════════════════════════════════╗
║                    PROYECCIÓN DE PRODUCCIÓN                     ║
╠══════════════════════════════════════════════════════════════════╣
)";
    std::cout << "║  Pollops actualmente:        " << std::setw(6) << totalPollos << "                        ║\n";
    std::cout << "║  Peso promedio actual:       " << std::fixed << std::setprecision(2) << std::setw(7) << pesoActual << " kg                    ║\n";
    std::cout << "║  Peso objetivo semana 7:    " << std::fixed << std::setprecision(2) << std::setw(7) << pesoSemana7 << " kg                    ║\n";
    std::cout << "║  ─────────────────────────────────────────────────────────────── ║\n";
    std::cout << "║  Peso total proyectado:       " << std::fixed << std::setprecision(2) << std::setw(9) << pesoTotalProyectado << " kg                   ║\n";
    std::cout << "║  Crecimiento restante:       " << std::fixed << std::setprecision(2) << std::setw(9) << crecimientoRestante << " kg                   ║\n";
    std::cout << "║  Porcentaje por crecer:      " << std::fixed << std::setprecision(1) << std::setw(7) << porcentajeFaltante << "%                    ║\n";
    std::cout << R"(
║  ESTIMACIÓN DE INGRESOS (a precio actual)                     ║
║  ─────────────────────────────────────────────────────────────── ║
)";
    BaseDatos* db = BaseDatos::getInstancia();
    double precioKg = db->getConfiguracion().precio_kg;
    double ingresoProyectado = pesoTotalProyectado * precioKg;
    std::cout << "║  Precio por kg:              USD " << std::fixed << std::setprecision(2) << precioKg << "                      ║\n";
    std::cout << "║  Ingreso proyectado:         USD " << std::fixed << std::setprecision(2) << std::setw(9) << ingresoProyectado << "                   ║\n";
    std::cout << R"(
╚══════════════════════════════════════════════════════════════════╝
)";
}