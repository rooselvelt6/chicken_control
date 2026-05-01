#include "../include/animales.h"
#include "../include/base_datos.h"
#include "../include/utils.h"
#include <sstream>
#include <iostream>
#include <iomanip>

int Animales::agregar(int lote_id, int cantidad, double precio_unitario, double peso_promedio, const std::string& fecha_ingreso) {
    auto* db = BaseDatos::getInstancia();
    Fase fase = clasificarFaseDias(0);
    std::string sql = "INSERT INTO animales (lote_id, cantidad, precio_unitario, peso_promedio, fase, fecha_ingreso) VALUES (" +
                      std::to_string(lote_id) + ", " + std::to_string(cantidad) + ", " +
                      std::to_string(precio_unitario) + ", " + std::to_string(peso_promedio) +
                      ", '" + faseToString(fase) + "', '" + fecha_ingreso + "')";
    return db->insertarYGetId(sql);
}

Animal* Animales::obtener(int id) {
    auto* db = BaseDatos::getInstancia();
    std::string sql = "SELECT id, lote_id, cantidad, precio_unitario, peso_promedio, fase, fecha_ingreso FROM animales WHERE id = " + std::to_string(id);
    sqlite3_stmt* stmt;
    static Animal a;
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            a.id = sqlite3_column_int(stmt, 0);
            a.lote_id = sqlite3_column_int(stmt, 1);
            a.cantidad = sqlite3_column_int(stmt, 2);
            a.precio_unitario = sqlite3_column_double(stmt, 3);
            a.peso_promedio = sqlite3_column_double(stmt, 4);
            const char* f = (const char*)sqlite3_column_text(stmt, 5);
            a.fase = f ? stringToFase(f) : Fase::Desconocido;
            const char* fe = (const char*)sqlite3_column_text(stmt, 6);
            a.fecha_ingreso = fe ? fe : "";
            sqlite3_finalize(stmt);
            return &a;
        }
    }
    sqlite3_finalize(stmt);
    return nullptr;
}

std::vector<Animal> Animales::listarPorLote(int lote_id) {
    auto* db = BaseDatos::getInstancia();
    std::string sql = "SELECT id, lote_id, cantidad, precio_unitario, peso_promedio, fase, fecha_ingreso FROM animales WHERE lote_id = " + std::to_string(lote_id);
    std::vector<Animal> resultado;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Animal a;
            a.id = sqlite3_column_int(stmt, 0);
            a.lote_id = sqlite3_column_int(stmt, 1);
            a.cantidad = sqlite3_column_int(stmt, 2);
            a.precio_unitario = sqlite3_column_double(stmt, 3);
            a.peso_promedio = sqlite3_column_double(stmt, 4);
            const char* f = (const char*)sqlite3_column_text(stmt, 5);
            a.fase = f ? stringToFase(f) : Fase::Desconocido;
            const char* fe = (const char*)sqlite3_column_text(stmt, 6);
            a.fecha_ingreso = fe ? fe : "";
            resultado.push_back(a);
        }
    }
    sqlite3_finalize(stmt);
    return resultado;
}

int Animales::obtenerCantidadTotal(int lote_id) {
    auto* db = BaseDatos::getInstancia();
    std::string sql = "SELECT COALESCE(SUM(cantidad), 0) FROM animales WHERE lote_id = " + std::to_string(lote_id);
    sqlite3_stmt* stmt;
    int total = 0;
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) total = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return total;
}

int Animales::registrarMuerte(int lote_id, CausaMuerte causa, int cantidad, const std::string& fecha) {
    auto* db = BaseDatos::getInstancia();
    std::string sql = "INSERT INTO muertes (lote_id, causa, cantidad, fecha) VALUES (" +
                      std::to_string(lote_id) + ", '" + causaMuerteToString(causa) + "', " +
                      std::to_string(cantidad) + ", '" + fecha + "')";
    return db->insertarYGetId(sql);
}

std::vector<Muerte> Animales::listarMuertes(int lote_id) {
    auto* db = BaseDatos::getInstancia();
    std::string sql = "SELECT id, lote_id, causa, cantidad, fecha FROM muertes WHERE lote_id = " + std::to_string(lote_id);
    std::vector<Muerte> resultado;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Muerte m;
            m.id = sqlite3_column_int(stmt, 0);
            m.lote_id = sqlite3_column_int(stmt, 1);
            const char* c = (const char*)sqlite3_column_text(stmt, 2);
            m.causa = c ? stringToCausaMuerte(c) : CausaMuerte::Desconocida;
            m.cantidad = sqlite3_column_int(stmt, 3);
            const char* f = (const char*)sqlite3_column_text(stmt, 4);
            m.fecha = f ? f : "";
            resultado.push_back(m);
        }
    }
    sqlite3_finalize(stmt);
    return resultado;
}

int Animales::totalMuertes(int lote_id) {
    auto* db = BaseDatos::getInstancia();
    std::string sql = "SELECT COALESCE(SUM(cantidad), 0) FROM muertes WHERE lote_id = " + std::to_string(lote_id);
    sqlite3_stmt* stmt;
    int total = 0;
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) total = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return total;
}

int Animales::registrarSacrificio(int lote_id, int cantidad, double peso_total, const std::string& fecha) {
    auto* db = BaseDatos::getInstancia();
    double peso_promedio = (cantidad > 0) ? peso_total / cantidad : 0.0;
    std::string sql = "INSERT INTO sacrificios (lote_id, cantidad, peso_total, peso_promedio, fecha) VALUES (" +
                      std::to_string(lote_id) + ", " + std::to_string(cantidad) + ", " +
                      std::to_string(peso_total) + ", " + std::to_string(peso_promedio) + ", '" + fecha + "')";
    return db->insertarYGetId(sql);
}

std::vector<Sacrificio> Animales::listarSacrificios(int lote_id) {
    auto* db = BaseDatos::getInstancia();
    std::string sql = "SELECT id, lote_id, cantidad, peso_total, peso_promedio, fecha FROM sacrificios WHERE lote_id = " + std::to_string(lote_id);
    std::vector<Sacrificio> resultado;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Sacrificio s;
            s.id = sqlite3_column_int(stmt, 0);
            s.lote_id = sqlite3_column_int(stmt, 1);
            s.cantidad = sqlite3_column_int(stmt, 2);
            s.peso_total = sqlite3_column_double(stmt, 3);
            s.peso_promedio = sqlite3_column_double(stmt, 4);
            const char* f = (const char*)sqlite3_column_text(stmt, 5);
            s.fecha = f ? f : "";
            resultado.push_back(s);
        }
    }
    sqlite3_finalize(stmt);
    return resultado;
}

std::pair<int, double> Animales::totalSacrificios(int lote_id) {
    auto* db = BaseDatos::getInstancia();
    std::string sql = "SELECT COALESCE(SUM(cantidad), 0), COALESCE(SUM(peso_total), 0) FROM sacrificios WHERE lote_id = " + std::to_string(lote_id);
    sqlite3_stmt* stmt;
    auto par = std::make_pair(0, 0.0);
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            par.first = sqlite3_column_int(stmt, 0);
            par.second = sqlite3_column_double(stmt, 1);
        }
    }
    sqlite3_finalize(stmt);
    return par;
}

double Animales::totalInvertido(int lote_id) {
    auto* db = BaseDatos::getInstancia();
    std::string sql = "SELECT COALESCE(SUM(cantidad * precio_unitario), 0) FROM animales WHERE lote_id = " + std::to_string(lote_id);
    sqlite3_stmt* stmt;
    double total = 0.0;
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) total = sqlite3_column_double(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return total;
}

double Animales::calcularPerdidasMuertes(int lote_id) {
    int muertes = totalMuertes(lote_id);
    double total_invertido = totalInvertido(lote_id);
    int total_animales = obtenerCantidadTotal(lote_id);
    if (total_animales > 0) return (double)muertes * (total_invertido / total_animales);
    return 0.0;
}

std::pair<int, double> Animales::animalesVivos(int lote_id) {
    int cantidad = obtenerCantidadTotal(lote_id) - totalMuertes(lote_id);
    return std::make_pair(cantidad, (double)cantidad * 2.0);
}

void Animales::imprimirResumen(int lote_id) {
    int total = obtenerCantidadTotal(lote_id);
    int muertes = totalMuertes(lote_id);
    auto [sacrificados, kg_sacrificados] = totalSacrificios(lote_id);
    double inversion = totalInvertido(lote_id);
    double perdidas = calcularPerdidasMuertes(lote_id);
    std::cout << "  Total comprados: " << total << std::endl;
    std::cout << "  Muertes: " << muertes << " (USD " << std::fixed << std::setprecision(2) << perdidas << ")" << std::endl;
    std::cout << "  Sacrificados: " << sacrificados << " (" << std::fixed << std::setprecision(2) << kg_sacrificados << " kg)" << std::endl;
    std::cout << "  Inversión en animales: USD " << std::fixed << std::setprecision(2) << inversion << std::endl;
}