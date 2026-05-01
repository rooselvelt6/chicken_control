#ifndef FINANCIERO_H
#define FINANCIERO_H

#include <string>
#include <vector>
#include <utility>
#include "modelos.h"

struct MovimientoFinanciero {
    int id;
    std::string fecha;
    std::string tipo;
    std::string categoria;
    std::string descripcion;
    double monto;
    bool es_ingreso;
};

struct EstadoResultados {
    double ingresos_ventas;
    double costo_animales;
    double costo_alimento;
    double costo_servicios;
    double costo_empleados;
    double costo_veterinaria;
    double utilidad_bruta;
    double gastos_operativos;
    double utilidad_neta;
};

struct FlujoCaja {
    double saldo_inicial;
    double ingresos;
    double egresos;
    double saldo_final;
    std::string periodo;
};

struct CostoPorLote {
    int lote_id;
    int numero_lote;
    double costo_animales;
    double costo_alimento;
    double costo_servicios;
    double costo_empleados;
    double costo_veterinaria;
    double total_costo;
    double ingreso_ventas;
    double utilidad;
    double rentabilidad_pct;
};

struct CostoPorEtapa {
    Fase etapa;
    std::string nombre_etapa;
    double costo_alimento = 0.0;
    double costo_medicamentos = 0.0;
    double costo_mano_obra = 0.0;
    double costo_instalaciones = 0.0;
    double costo_total = 0.0;
    double peso_promedio = 0.0;
    int dias_duracion = 0;
    double costo_por_kg = 0.0;
    double costo_por_dia = 0.0;
};

class Financiero {
public:
    static EstadoResultados calcularEstadoResultados(const std::string& fecha_inicio, const std::string& fecha_fin);
    static FlujoCaja calcularFlujoCaja(const std::string& mes, int anio);
    static std::vector<CostoPorLote> costosPorLote();
    static CostoPorLote costoDetalladoLote(int lote_id);
    static double ingresoTotalLote(int lote_id);
    static double gastoTotalLote(int lote_id);
    static double profitMarginLote(int lote_id);
    
    // NEW: Costos por etapa
    static std::vector<CostoPorEtapa> calcularCostosPorEtapa(int lote_id);
    static void mostrarCostosPorEtapa(int lote_id);
    static CostoPorEtapa costoEtapaLote(int lote_id, Fase etapa);
    static double promedioCostoPorEtapa(int lote_id);
    
    static void mostrarEstadoResultados(const std::string& fecha_inicio, const std::string& fecha_fin);
    static void mostrarFlujoCaja(int anio);
    static void mostrarCostosPorLote();
    static void registrarGasto(const std::string& fecha, const std::string& categoria, const std::string& descripcion, double monto);
    static std::vector<MovimientoFinanciero> listarMovimientos(const std::string& fecha_inicio, const std::string& fecha_fin);
    static double gastoTotalCategoria(const std::string& categoria, const std::string& fecha_inicio, const std::string& fecha_fin);
};

#endif