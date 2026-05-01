#include "../include/beneficio.h"
#include "../include/base_datos.h"
#include "../include/utils.h"
#include <iostream>
#include <sstream>
#include <iomanip>

int Beneficio::registrar(int lote_id, int cantidad, double peso_total, TipoBeneficio tipo_beneficio, const std::string& operador_nombre, const std::string& operador_cedula, const std::string& observaciones) {
    auto* db = BaseDatos::getInstancia();
    std::string fecha = fechaActual();
    std::string tipo_str = tipoBeneficioToString(tipo_beneficio);
    
    std::string sql = "INSERT INTO beneficio (lote_id, cantidad, peso_total, fecha, tipo_beneficio, operador_nombre, operador_cedula, observaciones) VALUES (" +
        std::to_string(lote_id) + ", " + std::to_string(cantidad) + ", " + std::to_string(peso_total) + ", '" +
        fecha + "', '" + tipo_str + "', '" + operador_nombre + "', '" + operador_cedula + "', '" + observaciones + "')";
    
    return db->insertarYGetId(sql);
}

RegistroBeneficio Beneficio::obtener(int id) {
    auto* db = BaseDatos::getInstancia();
    RegistroBeneficio b;
    sqlite3_stmt* stmt;
    std::string sql = "SELECT id, lote_id, cantidad, peso_total, fecha, tipo_beneficio, operador_nombre, operador_cedula, observaciones FROM beneficio WHERE id = " + std::to_string(id);
    
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            b.id = sqlite3_column_int(stmt, 0);
            b.lote_id = sqlite3_column_int(stmt, 1);
            b.cantidad = sqlite3_column_int(stmt, 2);
            b.peso_total = sqlite3_column_double(stmt, 3);
            const char* fe = (const char*)sqlite3_column_text(stmt, 4);
            b.fecha = fe ? fe : "";
            std::string tb = (const char*)sqlite3_column_text(stmt, 5);
            b.tipo_beneficio = stringToTipoBeneficio(tb);
            const char* on = (const char*)sqlite3_column_text(stmt, 6);
            b.operador_nombre = on ? on : "";
            const char* oc = (const char*)sqlite3_column_text(stmt, 7);
            b.operador_cedula = oc ? oc : "";
            const char* ob = (const char*)sqlite3_column_text(stmt, 8);
            b.observaciones = ob ? ob : "";
        }
    }
    sqlite3_finalize(stmt);
    return b;
}

std::vector<RegistroBeneficio> Beneficio::listar() {
    auto* db = BaseDatos::getInstancia();
    std::vector<RegistroBeneficio> resultado;
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db->abrir(), "SELECT id, lote_id, cantidad, peso_total, fecha, tipo_beneficio, operador_nombre, operador_cedula, observaciones FROM beneficio ORDER BY id DESC", -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            RegistroBeneficio b;
            b.id = sqlite3_column_int(stmt, 0);
            b.lote_id = sqlite3_column_int(stmt, 1);
            b.cantidad = sqlite3_column_int(stmt, 2);
            b.peso_total = sqlite3_column_double(stmt, 3);
            const char* fe = (const char*)sqlite3_column_text(stmt, 4);
            b.fecha = fe ? fe : "";
            std::string tb = (const char*)sqlite3_column_text(stmt, 5);
            b.tipo_beneficio = stringToTipoBeneficio(tb);
            const char* on = (const char*)sqlite3_column_text(stmt, 6);
            b.operador_nombre = on ? on : "";
            const char* oc = (const char*)sqlite3_column_text(stmt, 7);
            b.operador_cedula = oc ? oc : "";
            const char* ob = (const char*)sqlite3_column_text(stmt, 8);
            b.observaciones = ob ? ob : "";
            resultado.push_back(b);
        }
    }
    sqlite3_finalize(stmt);
    return resultado;
}

std::vector<RegistroBeneficio> Beneficio::listarPorLote(int lote_id) {
    auto* db = BaseDatos::getInstancia();
    std::vector<RegistroBeneficio> resultado;
    sqlite3_stmt* stmt;
    std::string sql = "SELECT id, lote_id, cantidad, peso_total, fecha, tipo_beneficio, operador_nombre, operador_cedula, observaciones FROM beneficio WHERE lote_id = " + std::to_string(lote_id);
    
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            RegistroBeneficio b;
            b.id = sqlite3_column_int(stmt, 0);
            b.lote_id = sqlite3_column_int(stmt, 1);
            b.cantidad = sqlite3_column_int(stmt, 2);
            b.peso_total = sqlite3_column_double(stmt, 3);
            const char* fe = (const char*)sqlite3_column_text(stmt, 4);
            b.fecha = fe ? fe : "";
            std::string tb = (const char*)sqlite3_column_text(stmt, 5);
            b.tipo_beneficio = stringToTipoBeneficio(tb);
            const char* on = (const char*)sqlite3_column_text(stmt, 6);
            b.operador_nombre = on ? on : "";
            const char* oc = (const char*)sqlite3_column_text(stmt, 7);
            b.operador_cedula = oc ? oc : "";
            const char* ob = (const char*)sqlite3_column_text(stmt, 8);
            b.observaciones = ob ? ob : "";
            resultado.push_back(b);
        }
    }
    sqlite3_finalize(stmt);
    return resultado;
}

bool Beneficio::eliminar(int id) {
    auto* db = BaseDatos::getInstancia();
    return db->ejecutarSQL("DELETE FROM beneficio WHERE id = " + std::to_string(id));
}

std::pair<int, double> Beneficio::getTotalBeneficio(int lote_id) {
    auto* db = BaseDatos::getInstancia();
    sqlite3_stmt* stmt;
    int cantidad = 0;
    double peso = 0.0;
    
    std::string sql = "SELECT COALESCE(SUM(cantidad), 0), COALESCE(SUM(peso_total), 0) FROM beneficio WHERE lote_id = " + std::to_string(lote_id);
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            cantidad = sqlite3_column_int(stmt, 0);
            peso = sqlite3_column_double(stmt, 1);
        }
    }
    sqlite3_finalize(stmt);
    return {cantidad, peso};
}

void Beneficio::mostrarEstadisticas() {
    auto registros = listar();
    if (registros.empty()) {
        std::cout << "No hay registros de beneficio" << std::endl;
        return;
    }
    
    int total_pollos = 0;
    double total_peso = 0.0;
    int manual = 0, automatico = 0, mixto = 0;
    
    for (const auto& r : registros) {
        total_pollos += r.cantidad;
        total_peso += r.peso_total;
        if (r.tipo_beneficio == TipoBeneficio::Manual) manual++;
        else if (r.tipo_beneficio == TipoBeneficio::Automatico) automatico++;
        else mixto++;
    }
    
    std::cout << "\n=== ESTADISTICAS DE BENEFICIO ===" << std::endl;
    std::cout << "Total de procesos: " << registros.size() << std::endl;
    std::cout << "Total pollos beneficiados: " << total_pollos << std::endl;
    std::cout << "Peso total: " << std::fixed << std::setprecision(2) << total_peso << " kg" << std::endl;
    std::cout << "Peso promedio: " << std::fixed << std::setprecision(2) << (total_pollos > 0 ? total_peso / total_pollos : 0) << " kg" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Tipo de beneficio:" << std::endl;
    std::cout << "  Manual: " << manual << std::endl;
    std::cout << "  Automatico: " << automatico << std::endl;
    std::cout << "  Mixto: " << mixto << std::endl;
    std::cout << "\n";
}