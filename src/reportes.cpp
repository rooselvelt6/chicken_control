#include "../include/reportes.h"
#include "../include/lotes.h"
#include "../include/animales.h"
#include "../include/alimentacion.h"
#include "../include/ventas.h"
#include "../include/inversores.h"
#include "../include/herramientas.h"
#include "../include/base_datos.h"
#include "../include/utils.h"
#include <iostream>
#include <iomanip>

void Reportes::reporteLote(int lote_id) {
    Lote* lote = Lotes::obtener(lote_id);
    if (!lote) { std::cout << "Lote no encontrado" << std::endl; return; }
    printHeader("LOTE #" + std::to_string(lote->numero));
    std::cout << "Fecha de inicio: " << lote->fecha_inicio << std::endl;
    std::cout << "Duracion: " << lote->duracion_dias << " dias" << std::endl;
    std::cout << "Estado: " << (lote->activo ? "ACTIVO" : "CERRADO") << std::endl << std::endl;
    printHeader("ANIMALES");
    Animales::imprimirResumen(lote_id);
    std::cout << std::endl;
    Fase fase = clasificarFaseDias(calcularDias(lote->fecha_inicio, fechaActual()));
    std::cout << "Fase actual: " << faseToString(fase) << std::endl << std::endl;
    printHeader("ALIMENTACION");
    auto resultado = Alimentacion::totalConsumo(lote_id);
    std::cout << "  Consumo: " << resultado.first << " sacks (USD " << std::fixed << std::setprecision(2) << resultado.second << ")" << std::endl;
}

void Reportes::reporteFinanciero() {
    printHeader("REPORTE FINANCIERO");
    Configuracion config = BaseDatos::getInstancia()->getConfiguracion();
    std::cout << "Precio por Kg: USD " << std::fixed << std::setprecision(2) << config.precio_kg << std::endl << std::endl;
    printHeader("DEUDAS PENDIENTES");
    std::cout << "  No hay deudas pendientes" << std::endl << std::endl;
    printHeader("INVERSORES");
    for (auto& inv : Inversores::listar()) {
        std::cout << "  " << inv.nombre << ": USD " << std::fixed << std::setprecision(2) << inv.cantidad_invertida << std::endl;
    }
    double total_inv = Inversores::totalInvertido();
    std::cout << "Total invertido: USD " << std::fixed << std::setprecision(2) << total_inv << std::endl;
}

void Reportes::reporteInventario() {
    printHeader("INVENTARIO");
    printHeader("Lotes");
    for (auto& l : Lotes::listar()) {
        auto vivos = Animales::animalesVivos(l.id);
        Fase fase = clasificarFaseDias(calcularDias(l.fecha_inicio, fechaActual()));
        std::cout << "  Lote #" << l.numero << " - Fase: " << faseToString(fase) << " - Animales: " << vivos.first << " - " << (l.activo ? "ACTIVO" : "CERRADO") << std::endl;
    }
    std::cout << std::endl;
    printHeader("Alimentos");
    Alimentacion::imprimirInventario();
    std::cout << std::endl;
    printHeader("Herramientas");
    for (auto& h : Herramientas::listar()) {
        std::cout << "  " << h.nombre << ": " << h.cantidad << " x USD " << std::fixed << std::setprecision(2) << h.precio_unitario << " = USD " << h.cantidad * h.precio_unitario << std::endl;
    }
    std::cout << "Total herramientas: USD " << std::fixed << std::setprecision(2) << Herramientas::totalInvertido() << std::endl;
}

void Reportes::reporteDeudas() {
    printHeader("DEUDAS PENDIENTES");
    std::cout << "  No hay deudas pendientes" << std::endl;
}

void Reportes::reporteConsumo(int lote_id) {
    printHeader("CONSUMO DE ALIMENTO - LOTE " + std::to_string(lote_id));
    auto resultado = Alimentacion::totalConsumo(lote_id);
    std::cout << "Total: " << resultado.first << " sacks - Costo: USD " << std::fixed << std::setprecision(2) << resultado.second << std::endl;
}

void Reportes::reporteCompleto() {
    reporteInventario();
    std::cout << std::endl;
    reporteFinanciero();
}