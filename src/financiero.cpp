#include "../include/financiero.h"
#include "../include/base_datos.h"
#include "../include/utils.h"
#include "../include/ventas.h"
#include "../include/lotes.h"
#include "../include/animales.h"
#include "../include/alimentacion.h"
#include "../include/empleados.h"
#include "../include/veterinaria.h"
#include "../include/temperatura.h"
#include "../include/servicios.h"
#include <iostream>
#include <sstream>
#include <iomanip>

void Financiero::registrarGasto(const std::string& fecha, const std::string& categoria, const std::string& descripcion, double monto) {
    auto* db = BaseDatos::getInstancia();
    std::string cat_segura = sanitizarSQL(sanitizarInput(categoria, 50));
    std::string desc_segura = sanitizarSQL(sanitizarInput(descripcion, 200));
    std::string fecha_segura = sanitizarSQL(sanitizarInput(fecha, 10));
    std::string sql = "INSERT INTO movimientos_financieros (fecha, tipo, categoria, descripcion, monto, es_ingreso) VALUES ('" +
        fecha_segura + "', 'gasto', '" + cat_segura + "', '" + desc_segura + "', " + std::to_string(monto) + ", 0)";
    db->insertarYGetId(sql);
}

double Financiero::gastoTotalCategoria(const std::string& categoria, const std::string& fecha_inicio, const std::string& fecha_fin) {
    auto* db = BaseDatos::getInstancia();
    sqlite3_stmt* stmt;
    double total = 0.0;
    std::string sql = "SELECT COALESCE(SUM(monto), 0) FROM movimientos_financieros WHERE tipo = 'gasto' AND categoria = '" +
        sanitizarSQL(categoria) + "' AND fecha >= '" + sanitizarSQL(fecha_inicio) + "' AND fecha <= '" + sanitizarSQL(fecha_fin) + "'";
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            total = sqlite3_column_double(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);
    return total;
}

std::vector<MovimientoFinanciero> Financiero::listarMovimientos(const std::string& fecha_inicio, const std::string& fecha_fin) {
    auto* db = BaseDatos::getInstancia();
    std::vector<MovimientoFinanciero> resultado;
    sqlite3_stmt* stmt;
    std::string sql = "SELECT id, fecha, tipo, categoria, descripcion, monto, es_ingreso FROM movimientos_financieros WHERE fecha >= '" +
        sanitizarSQL(fecha_inicio) + "' AND fecha <= '" + sanitizarSQL(fecha_fin) + "' ORDER BY fecha DESC";
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            MovimientoFinanciero m;
            m.id = sqlite3_column_int(stmt, 0);
            const char* f = (const char*)sqlite3_column_text(stmt, 1);
            m.fecha = f ? f : "";
            const char* t = (const char*)sqlite3_column_text(stmt, 2);
            m.tipo = t ? t : "";
            const char* c = (const char*)sqlite3_column_text(stmt, 3);
            m.categoria = c ? c : "";
            const char* d = (const char*)sqlite3_column_text(stmt, 4);
            m.descripcion = d ? d : "";
            m.monto = sqlite3_column_double(stmt, 5);
            m.es_ingreso = sqlite3_column_int(stmt, 6) == 1;
            resultado.push_back(m);
        }
    }
    sqlite3_finalize(stmt);
    return resultado;
}

double Financiero::ingresoTotalLote(int lote_id) {
    return Ventas::totalVentas(lote_id);
}

double Financiero::gastoTotalLote(int lote_id) {
    double total = 0.0;

    auto* db = BaseDatos::getInstancia();
    sqlite3_stmt* stmt;

    std::string sql = "SELECT COALESCE(SUM(precio_unitario * cantidad), 0) FROM animales WHERE lote_id = " + std::to_string(lote_id);
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) total += sqlite3_column_double(stmt, 0);
    }
    sqlite3_finalize(stmt);

    sql = "SELECT COALESCE(SUM(cantidad * precio_unitario), 0) FROM consumo_alimento WHERE lote_id = " + std::to_string(lote_id);
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) total += sqlite3_column_double(stmt, 0);
    }
    sqlite3_finalize(stmt);

    return total;
}

double Financiero::profitMarginLote(int lote_id) {
    double ingreso = ingresoTotalLote(lote_id);
    double gasto = gastoTotalLote(lote_id);
    if (ingreso == 0) return 0;
    return ((ingreso - gasto) / ingreso) * 100.0;
}

CostoPorLote Financiero::costoDetalladoLote(int lote_id) {
    CostoPorLote c;
    c.lote_id = lote_id;
    c.costo_animales = 0;
    c.costo_alimento = 0;
    c.costo_servicios = 0;
    c.costo_empleados = 0;
    c.costo_veterinaria = 0;
    c.total_costo = 0;
    c.ingreso_ventas = 0;
    c.utilidad = 0;
    c.rentabilidad_pct = 0;

    Lote* l = Lotes::obtener(lote_id);
    if (l) c.numero_lote = l->numero;

    auto* db = BaseDatos::getInstancia();
    sqlite3_stmt* stmt;
    std::string sql;

    sql = "SELECT COALESCE(SUM(precio_unitario * cantidad), 0) FROM animales WHERE lote_id = " + std::to_string(lote_id);
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) c.costo_animales = sqlite3_column_double(stmt, 0);
    }
    sqlite3_finalize(stmt);

    sql = "SELECT COALESCE(SUM(cantidad * precio_unitario), 0) FROM consumo_alimento WHERE lote_id = " + std::to_string(lote_id);
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) c.costo_alimento = sqlite3_column_double(stmt, 0);
    }
    sqlite3_finalize(stmt);

    c.costo_servicios = 0;
    c.costo_empleados = 0;
    c.costo_veterinaria = 0;

    c.total_costo = c.costo_animales + c.costo_alimento + c.costo_servicios + c.costo_empleados + c.costo_veterinaria;
    c.ingreso_ventas = ingresoTotalLote(lote_id);
    c.utilidad = c.ingreso_ventas - c.total_costo;
    c.rentabilidad_pct = (c.ingreso_ventas > 0) ? (c.utilidad / c.ingreso_ventas) * 100.0 : 0;

    return c;
}

std::vector<CostoPorLote> Financiero::costosPorLote() {
    std::vector<CostoPorLote> resultado;
    auto lotes = Lotes::listar(false);
    for (const auto& l : lotes) {
        resultado.push_back(costoDetalladoLote(l.id));
    }
    return resultado;
}

EstadoResultados Financiero::calcularEstadoResultados(const std::string& fecha_inicio, const std::string& fecha_fin) {
    EstadoResultados e = {};

    auto ventas = Ventas::listar();
    for (const auto& v : ventas) {
        if (v.fecha >= fecha_inicio && v.fecha <= fecha_fin) {
            e.ingresos_ventas += v.total;
        }
    }

    auto lotes = Lotes::listar(false);
    double costo_animales = 0, costo_alimento = 0;
    for (const auto& l : lotes) {
        costo_animales += gastoTotalLote(l.id);
        costo_alimento += gastoTotalLote(l.id);
    }
    e.costo_animales = costo_animales * 0.3;
    e.costo_alimento = costo_alimento * 0.5;

    e.costo_servicios = gastoTotalCategoria("servicios", fecha_inicio, fecha_fin);
    e.costo_empleados = gastoTotalCategoria("nomina", fecha_inicio, fecha_fin);
    e.costo_veterinaria = gastoTotalCategoria("veterinaria", fecha_inicio, fecha_fin);

    e.utilidad_bruta = e.ingresos_ventas - (e.costo_animales + e.costo_alimento + e.costo_servicios);
    e.gastos_operativos = e.costo_empleados + e.costo_veterinaria;
    e.utilidad_neta = e.utilidad_bruta - e.gastos_operativos;

    return e;
}

FlujoCaja Financiero::calcularFlujoCaja(const std::string& mes, int anio) {
    FlujoCaja f;
    f.saldo_inicial = 0;
    f.ingresos = 0;
    f.egresos = 0;
    f.saldo_final = 0;
    f.periodo = mes + "-" + std::to_string(anio);

    std::stringstream ss;
    ss << anio << "-";
    if (mes == "Enero") ss << "01";
    else if (mes == "Febrero") ss << "02";
    else if (mes == "Marzo") ss << "03";
    else if (mes == "Abril") ss << "04";
    else if (mes == "Mayo") ss << "05";
    else if (mes == "Junio") ss << "06";
    else if (mes == "Julio") ss << "07";
    else if (mes == "Agosto") ss << "08";
    else if (mes == "Septiembre") ss << "09";
    else if (mes == "Octubre") ss << "10";
    else if (mes == "Noviembre") ss << "11";
    else if (mes == "Diciembre") ss << "12";
    std::string prefijo = ss.str();

    auto ventas = Ventas::listar();
    for (const auto& v : ventas) {
        if (v.fecha.substr(0, 7) == prefijo) {
            f.ingresos += v.total;
        }
    }

    auto movimientos = listarMovimientos(prefijo + "-01", prefijo + "-31");
    for (const auto& m : movimientos) {
        f.egresos += m.monto;
    }

    f.saldo_final = f.saldo_inicial + f.ingresos - f.egresos;

    return f;
}

void Financiero::mostrarEstadoResultados(const std::string& fecha_inicio, const std::string& fecha_fin) {
    EstadoResultados e = calcularEstadoResultados(fecha_inicio, fecha_fin);

    std::cout << "\n╔════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║          ESTADO DE RESULTADOS                            ║" << std::endl;
    std::cout << "║          Periodo: " << fecha_inicio << " a " << fecha_fin << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ INGRESOS                                                  ║" << std::endl;
    std::cout << "║   Ventas totals:        USD " << std::fixed << std::setprecision(2) << std::setw(10) << e.ingresos_ventas << "      ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ COSTOS                                                    ║" << std::endl;
    std::cout << "║   Costo animales:       USD " << std::fixed << std::setprecision(2) << std::setw(10) << e.costo_animales << "      ║" << std::endl;
    std::cout << "║   Costo alimento:       USD " << std::fixed << std::setprecision(2) << std::setw(10) << e.costo_alimento << "      ║" << std::endl;
    std::cout << "║   Costo servicios:      USD " << std::fixed << std::setprecision(2) << std::setw(10) << e.costo_servicios << "      ║" << std::endl;
    std::cout << "║   Costo empleados:     USD " << std::fixed << std::setprecision(2) << std::setw(10) << e.costo_empleados << "      ║" << std::endl;
    std::cout << "║   Costo veterinaria:   USD " << std::fixed << std::setprecision(2) << std::setw(10) << e.costo_veterinaria << "      ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ RESUMEN                                                   ║" << std::endl;
    std::cout << "║   Utilidad Bruta:      USD " << std::fixed << std::setprecision(2) << std::setw(10) << e.utilidad_bruta << "      ║" << std::endl;
    std::cout << "║   Utilidad Neta:       USD " << std::fixed << std::setprecision(2) << std::setw(10) << e.utilidad_neta << "      ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════╝" << std::endl;
}

void Financiero::mostrarFlujoCaja(int anio) {
    std::vector<std::string> meses = {"Enero", "Febrero", "Marzo", "Abril", "Mayo", "Junio", "Julio", "Agosto", "Septiembre", "Octubre", "Noviembre", "Diciembre"};

    std::cout << "\n╔═══════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                    FLUJO DE CAJA - " << anio << "                                  ║" << std::endl;
    std::cout << "╠═══════════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ Mes        │ Ingresos    │ Egresos     │ Saldo      │                     ║" << std::endl;
    std::cout << "╠═══════════════════════════════════════════════════════════════════════╣" << std::endl;

    double total_ingresos = 0, total_egresos = 0;

    for (const auto& mes : meses) {
        FlujoCaja f = calcularFlujoCaja(mes, anio);
        std::cout << "║ " << std::setw(10) << mes << " │ USD " << std::fixed << std::setprecision(2) << std::setw(9) << f.ingresos
                  << " │ USD " << std::setw(9) << f.egresos << " │ USD " << std::setw(9) << f.saldo_final << " │" << std::endl;
        total_ingresos += f.ingresos;
        total_egresos += f.egresos;
    }

    std::cout << "╠═══════════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ TOTALES    │ USD " << std::fixed << std::setprecision(2) << std::setw(9) << total_ingresos
              << " │ USD " << std::setw(9) << total_egresos << " │ USD " << std::setw(9) << (total_ingresos - total_egresos) << " │" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════════════════╝" << std::endl;
}

void Financiero::mostrarCostosPorLote() {
    auto costos = costosPorLote();

    std::cout << "\n╔═══════════════════════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                                    COSTOS POR LOTE                                                                        ║" << std::endl;
    std::cout << "╠═══════════════════════════════════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ Lote │ Animales  │ Alimento   │ Servicios │ Empleados  │ VETERINARIA │ TOTAL      │ INGRESOS   │ UTILIDAD  │ MARGEN  ║" << std::endl;
    std::cout << "╠═══════════════════════════════════════════════════════════════════════════════════════════════════════════════╣" << std::endl;

    for (const auto& c : costos) {
        std::cout << "║ #" << std::setw(3) << c.numero_lote << " │ USD " << std::fixed << std::setprecision(2)
                  << std::setw(8) << c.costo_animales << " │ USD " << std::setw(9) << c.costo_alimento
                  << " │ USD " << std::setw(8) << c.costo_servicios << " │ USD " << std::setw(9) << c.costo_empleados
                  << " │ USD " << std::setw(10) << c.costo_veterinaria << " │ USD " << std::setw(9) << c.total_costo
                  << " │ USD " << std::setw(9) << c.ingreso_ventas << " │ USD " << std::setw(9) << c.utilidad
                  << " │ " << std::setw(5) << std::fixed << std::setprecision(1) << c.rentabilidad_pct << "%" << std::endl;
    }

    std::cout << "╚═══════════════════════════════════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
}