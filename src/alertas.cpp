#include "../include/alertas.h"
#include "../include/base_datos.h"
#include "../include/lotes.h"
#include "../include/animales.h"
#include "../include/alimentacion.h"
#include "../include/ventas.h"
#include "../include/pesaje.h"
#include <iostream>
#include <sstream>
#include <iomanip>

void Alertas::verificarInventario(std::vector<Alerta>& alertas) {
    auto alimentos = Alimentacion::listar();
    for (const auto& a : alimentos) {
        if (a.inventario < 10) {
            alertas.push_back({
                "Inventario",
                "Alimento '" + a.nombre + "' con stock bajo: " + std::to_string(a.inventario) + " sacks",
                2
            });
        }
    }
}

void Alertas::verificarMortandad(std::vector<Alerta>& alertas) {
    auto lotes = Lotes::listar(true);
    for (const auto& l : lotes) {
        auto muertes = Animales::listarMuertes(l.id);
        auto animales = Animales::listarPorLote(l.id);
        
        int totalInicial = 0;
        for (const auto& a : animales) {
            totalInicial += a.cantidad;
        }
        
        int totalMuertes = 0;
        for (const auto& m : muertes) {
            totalMuertes += m.cantidad;
        }
        
        if (totalInicial > 0) {
            double porcentaje = (totalMuertes * 100.0) / totalInicial;
            if (porcentaje > 5.0) {
                std::ostringstream oss;
                oss << "Lote #" << l.numero << ": Mortalidad del " << std::fixed << std::setprecision(1) << porcentaje << "%";
                alertas.push_back({"Mortalidad", oss.str(), porcentaje > 10 ? 3 : 2});
            }
        }
    }
}

void Alertas::verificarDeudas(std::vector<Alerta>& alertas) {
    auto ventas = Ventas::listar();
    double totalDeuda = 0;
    int cantidad = 0;
    
    for (const auto& v : ventas) {
        if (!v.pagada && v.pendiente > 0) {
            totalDeuda += v.pendiente;
            cantidad++;
        }
    }
    
    if (cantidad > 0) {
        std::ostringstream oss;
        oss << cantidad << " ventas pendientes por Bs. " << std::fixed << std::setprecision(2) << totalDeuda;
        alertas.push_back({"Deudas", oss.str(), 2});
    }
}

void Alertas::verificarLotesActivos(std::vector<Alerta>& alertas) {
    auto lotes = Lotes::listar(true);
    for (const auto& l : lotes) {
        if (l.activo) {
            auto pesaje = Pesaje::listarPorLote(l.id);
            if (pesaje.empty()) {
                alertas.push_back({
                    "Pesaje",
                    "Lote #" + std::to_string(l.numero) + " sin registro de pesaje",
                    1
                });
            }
        }
    }
}

std::vector<Alerta> Alertas::verificarAlertas() {
    std::vector<Alerta> alertas;
    verificarInventario(alertas);
    verificarMortandad(alertas);
    verificarDeudas(alertas);
    return alertas;
}

void Alertas::mostrarAlertas() {
    auto alertas = verificarAlertas();
    if (alertas.empty()) {
        std::cout << "✓ No hay alertas pendientes" << std::endl;
        return;
    }
    
    std::cout << "\n===== ALERTAS DEL SISTEMA =====" << std::endl;
    for (const auto& a : alertas) {
        std::string icono = a.prioridad == 3 ? "🔴" : (a.prioridad == 2 ? "🟡" : "🔵");
        std::cout << icono << " [" << a.tipo << "] " << a.mensaje << std::endl;
    }
    std::cout << "===============================" << std::endl;
}