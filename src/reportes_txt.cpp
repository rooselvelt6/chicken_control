#include "../include/reportes_txt.h"
#include "../include/financiero.h"
#include "../include/metricas.h"
#include "../include/lotes.h"
#include "../include/ventas.h"
#include "../include/animales.h"
#include "../include/alimentacion.h"
#include <iostream>
#include <fstream>
#include <iomanip>

void ReportesTXT::exportarEstadoResultados(const std::string& archivo, const std::string& fecha_inicio, const std::string& fecha_fin) {
    auto e = Financiero::calcularEstadoResultados(fecha_inicio, fecha_fin);

    std::ofstream out(archivo);
    if (!out.is_open()) {
        std::cout << "Error al crear archivo: " << archivo << std::endl;
        return;
    }

    out << "==========================================\n";
    out << "         ESTADO DE RESULTADOS\n";
    out << "         Periodo: " << fecha_inicio << " a " << fecha_fin << "\n";
    out << "==========================================\n\n";

    out << "INGRESOS\n";
    out << "   Ventas totales:       USD " << std::fixed << std::setprecision(2) << e.ingresos_ventas << "\n\n";

    out << "COSTOS\n";
    out << "   Costo animales:       USD " << std::fixed << std::setprecision(2) << e.costo_animales << "\n";
    out << "   Costo alimento:        USD " << std::fixed << std::setprecision(2) << e.costo_alimento << "\n";
    out << "   Costo servicios:       USD " << std::fixed << std::setprecision(2) << e.costo_servicios << "\n";
    out << "   Costo empleados:      USD " << std::fixed << std::setprecision(2) << e.costo_empleados << "\n";
    out << "   Costo veterinaria:    USD " << std::fixed << std::setprecision(2) << e.costo_veterinaria << "\n\n";

    out << "RESUMEN\n";
    out << "   Utilidad Bruta:      USD " << std::fixed << std::setprecision(2) << e.utilidad_bruta << "\n";
    out << "   Gastos Operativos:    USD " << std::fixed << std::setprecision(2) << e.gastos_operativos << "\n";
    out << "   Utilidad Neta:       USD " << std::fixed << std::setprecision(2) << e.utilidad_neta << "\n";

    out.close();
    std::cout << "Reporte exportado a: " << archivo << std::endl;
}

void ReportesTXT::exportarFlujoCaja(const std::string& archivo, int anio) {
    std::vector<std::string> meses = {"Enero", "Febrero", "Marzo", "Abril", "Mayo", "Junio", "Julio", "Agosto", "Septiembre", "Octubre", "Noviembre", "Diciembre"};

    std::ofstream out(archivo);
    if (!out.is_open()) {
        std::cout << "Error al crear archivo: " << archivo << std::endl;
        return;
    }

    out << "==========================================\n";
    out << "         FLUJO DE CAJA - " << anio << "\n";
    out << "==========================================\n\n";

    out << "Mes         | Ingresos     | Egresos      | Saldo\n";
    out << "-------------------------------------------\n";

    double total_ingresos = 0, total_egresos = 0;

    for (const auto& mes : meses) {
        FlujoCaja f = Financiero::calcularFlujoCaja(mes, anio);
        out << std::setw(10) << mes << " | USD " << std::fixed << std::setprecision(2) << std::setw(9) << f.ingresos
            << " | USD " << std::setw(9) << f.egresos << " | USD " << std::setw(9) << f.saldo_final << "\n";
        total_ingresos += f.ingresos;
        total_egresos += f.egresos;
    }

    out << "-------------------------------------------\n";
    out << "TOTALES     | USD " << std::fixed << std::setprecision(2) << std::setw(9) << total_ingresos
        << " | USD " << std::setw(9) << total_egresos << " | USD " << std::setw(9) << (total_ingresos - total_egresos) << "\n";

    out.close();
    std::cout << "Reporte exportado a: " << archivo << std::endl;
}

void ReportesTXT::exportarCostosPorLote(const std::string& archivo) {
    auto costos = Financiero::costosPorLote();

    std::ofstream out(archivo);
    if (!out.is_open()) {
        std::cout << "Error al crear archivo: " << archivo << std::endl;
        return;
    }

    out << "==========================================\n";
    out << "         COSTOS POR LOTE\n";
    out << "==========================================\n\n";

    out << "Lote | Animales   | Alimento   | Servicios  | Empleados | Vet.      | Total      | Ingresos   | Utilidad\n";
    out << "-------------------------------------------------------------------------------------------------\n";

    for (const auto& c : costos) {
        out << "#" << std::setw(3) << c.numero_lote << " | USD " << std::fixed << std::setprecision(2) << std::setw(8) << c.costo_animales
            << " | USD " << std::setw(8) << c.costo_alimento << " | USD " << std::setw(8) << c.costo_servicios
            << " | USD " << std::setw(8) << c.costo_empleados << " | USD " << std::setw(8) << c.costo_veterinaria
            << " | USD " << std::setw(8) << c.total_costo << " | USD " << std::setw(8) << c.ingreso_ventas
            << " | USD " << std::setw(8) << c.utilidad << "\n";
    }

    out.close();
    std::cout << "Reporte exportado a: " << archivo << std::endl;
}

void ReportesTXT::exportarMetricasLote(const std::string& archivo, int lote_id) {
    std::ofstream out(archivo);
    if (!out.is_open()) {
        std::cout << "Error al crear archivo: " << archivo << std::endl;
        return;
    }

    out << "==========================================\n";
    out << "         METRICAS LOTE #" << lote_id << "\n";
    out << "==========================================\n\n";

    Lote* l = Lotes::obtener(lote_id);
    if (l) {
        out << "Numero de Lote: " << l->numero << "\n";
        out << "Fecha Inicio: " << l->fecha_inicio << "\n";
        out << "Fecha Fin: " << (l->fecha_fin.empty() ? "En curso" : l->fecha_fin) << "\n";
        out << "Activo: " << (l->activo ? "Sí" : "No") << "\n\n";
    }

    auto metricas = Metricas::calcularMetricasLote(lote_id);
    out << "RESUMEN FINANCIERO\n";
    out << "  Total Animales Inicial: " << metricas.total_inicial << "\n";
    out << "  Total Muertes: " << metricas.total_muertes << "\n";
    out << "  Mortalidad: " << std::fixed << std::setprecision(1) << metricas.porcentaje_mortalidad << "%\n";
    out << "  Ingreso Ventas: USD " << std::fixed << std::setprecision(2) << metricas.ingreso_ventas << "\n";
    out << "  Costo Total: USD " << std::fixed << std::setprecision(2) << metricas.costo_total << "\n";
    out << "  Ganancia: USD " << std::fixed << std::setprecision(2) << metricas.ganancia << "\n";
    out << "  Rentabilidad: " << std::fixed << std::setprecision(1) << metricas.rentabilidad_porcentaje << "%\n";

    out.close();
    std::cout << "Reporte exportado a: " << archivo << std::endl;
}

void ReportesTXT::exportarDashboard(const std::string& archivo) {
    std::ofstream out(archivo);
    if (!out.is_open()) {
        std::cout << "Error al crear archivo: " << archivo << std::endl;
        return;
    }

    out << "==========================================\n";
    out << "         DASHBOARD GENERAL\n";
    out << "==========================================\n\n";

    auto lotes = Lotes::listar(false);
    out << "PRODUCCION\n";
    out << "  Total Lotes: " << lotes.size() << "\n";
    int total_animales = 0;
    for (const auto& l : lotes) {
        total_animales += Animales::obtenerCantidadTotal(l.id);
    }
    out << "  Total Animales: " << total_animales << "\n\n";

    out << "VENTAS\n";
    auto ventas = Ventas::listar();
    double total_ventas = 0;
    for (const auto& v : ventas) total_ventas += v.total;
    out << "  Total Ventas: USD " << std::fixed << std::setprecision(2) << total_ventas << "\n";
    out << "  Deudas Pendientes: USD " << std::fixed << std::setprecision(2) << Ventas::totalDeudasPendientes() << "\n\n";

    out << "INVENTARIO\n";
    auto alimentos = Alimentacion::listar();
    int stock_total = 0;
    for (const auto& a : alimentos) stock_total += a.inventario;
    out << "  Total Alimentos: " << alimentos.size() << " tipos\n";
    out << "  Stock Total: " << stock_total << " unidades\n";

    out.close();
    std::cout << "Dashboard exportado a: " << archivo << std::endl;
}