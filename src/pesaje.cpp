#include "../include/pesaje.h"
#include "../include/base_datos.h"
#include "../include/modelos.h"
#include "../include/utils.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <algorithm>

int Pesaje::registrar(int lote_id, int semana, const std::string& fecha,
                     const std::string& pesos_str, const std::string& observacion) {
    auto* db = BaseDatos::getInstancia();
    
    std::vector<float> pesos = stringToFloatList(pesos_str);
    double peso_promedio = 0.0;
    double peso_minimo = 99999.0;
    double peso_maximo = 0.0;
    double peso_total = 0.0;
    
    for (float p : pesos) {
        peso_total += p;
        if (p < peso_minimo) peso_minimo = p;
        if (p > peso_maximo) peso_maximo = p;
    }
    
    if (!pesos.empty()) {
        peso_promedio = peso_total / pesos.size();
    }
    
    double peso_objetivo = pesoObjetivoSemana(semana);
    int cantidad = pesos.size();
    
    std::stringstream ss;
    ss << "INSERT INTO pesaje_semanal (lote_id, semana, fecha, cantidad_pollos_pesados, "
       << "peso_promedio, peso_total, peso_minimo, peso_maximo, peso_objetivo, observacion) VALUES ("
       << lote_id << ", " << semana << ", '" << fecha << "', " << cantidad << ", "
       << peso_promedio << ", " << peso_total << ", " << peso_minimo << ", " << peso_maximo << ", "
       << peso_objetivo << ", '" << observacion << "')";
    
    return db->insertarYGetId(ss.str());
}

std::vector<PesajeSemanal> Pesaje::listarPorLote(int lote_id) {
    auto* db = BaseDatos::getInstancia();
    std::vector<PesajeSemanal> resultado;
    sqlite3_stmt* stmt;
    std::stringstream ss;
    ss << "SELECT id, lote_id, semana, fecha, cantidad_pollos_pesados, peso_promedio, "
       << "peso_total, peso_minimo, peso_maximo, peso_objetivo, observacion "
       << "FROM pesaje_semanal WHERE lote_id = " << lote_id << " ORDER BY semana";
    
    if (sqlite3_prepare_v2(db->abrir(), ss.str().c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            PesajeSemanal p;
            p.id = sqlite3_column_int(stmt, 0);
            p.lote_id = sqlite3_column_int(stmt, 1);
            p.semana = sqlite3_column_int(stmt, 2);
            const char* f = (const char*)sqlite3_column_text(stmt, 3);
            p.fecha = f ? f : "";
            p.cantidad_pollos_pesados = sqlite3_column_int(stmt, 4);
            p.peso_promedio = sqlite3_column_double(stmt, 5);
            p.peso_total = sqlite3_column_double(stmt, 6);
            p.peso_minimo = sqlite3_column_double(stmt, 7);
            p.peso_maximo = sqlite3_column_double(stmt, 8);
            p.peso_objetivo = sqlite3_column_double(stmt, 9);
            const char* o = (const char*)sqlite3_column_text(stmt, 10);
            p.observacion = o ? o : "";
            resultado.push_back(p);
        }
    }
    sqlite3_finalize(stmt);
    return resultado;
}

double Pesaje::obtenerPesoPromedio(int lote_id) {
    auto lista = listarPorLote(lote_id);
    if (lista.empty()) return 0.0;
    return lista.back().peso_promedio;
}

bool Pesaje::estaEnPeso(int lote_id) {
    auto lista = listarPorLote(lote_id);
    if (lista.empty()) return false;
    PesajeSemanal ultimo = lista.back();
    double diferencia = std::abs(ultimo.peso_promedio - ultimo.peso_objetivo);
    return diferencia <= 0.3; // 300g de tolerancia
}

double Pesaje::getPesoObjetivo(int semana) {
    return pesoObjetivoSemana(semana);
}

void Pesaje::imprimirReporte(int lote_id) {
    auto lista = listarPorLote(lote_id);
    if (lista.empty()) {
        std::cout << "No hay registros de pesaje para el lote " << lote_id << std::endl;
        return;
    }
    
    std::cout << "================================================================================" << std::endl;
    std::cout << "  REPORTE DE PESAJE - LOTE " << lote_id << std::endl;
    std::cout << "================================================================================" << std::endl;
    
    for (auto& p : lista) {
        std::cout << "Semana " << p.semana << " (" << p.fecha << ")" << std::endl;
        std::cout << "  Pollos pesados: " << p.cantidad_pollos_pesados << std::endl;
        std::cout << "  Peso promedio: " << std::fixed << std::setprecision(2) << p.peso_promedio << " kg" << std::endl;
        std::cout << "  Peso objetivo: " << p.peso_objetivo << " kg" << std::endl;
        std::cout << "  Rango: " << p.peso_minimo << " - " << p.peso_maximo << " kg" << std::endl;
        
        double diferencia = p.peso_promedio - p.peso_objetivo;
        if (std::abs(diferencia) <= 0.1) {
            std::cout << "  Estado: ✓ EN PESO" << std::endl;
        } else if (diferencia > 0) {
            std::cout << "  Estado: ↑ POR ENCIMA (" << std::fixed << std::setprecision(2) << diferencia << " kg)" << std::endl;
        } else {
            std::cout << "  Estado: ↓ POR DEBAJO (" << std::fixed << std::setprecision(2) << diferencia << " kg)" << std::endl;
        }
        if (!p.observacion.empty()) {
            std::cout << "  Nota: " << p.observacion << std::endl;
        }
        std::cout << std::endl;
    }
}

void Pesaje::imprimirEvolucion(int lote_id) {
    auto lista = listarPorLote(lote_id);
    if (lista.empty()) {
        std::cout << "No hay datos de evolución" << std::endl;
        return;
    }
    
    std::cout << "=== EVOLUCIÓN DE PESO ===" << std::endl;
    for (auto& p : lista) {
        int barras = (int)(p.peso_promedio * 10);
        std::cout << "Sem " << p.semana << ": ";
        for (int i = 0; i < barras && i < 20; i++) std::cout << "█";
        std::cout << " " << std::fixed << std::setprecision(2) << p.peso_promedio << " kg (obj: " << p.peso_objetivo << ")" << std::endl;
    }
}