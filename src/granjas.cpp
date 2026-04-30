#include "../include/granjas.h"
#include "../include/base_datos.h"
#include <sstream>
#include <iostream>

// ==================== TIPOS DE GRANJA ====================

int Granjas::crearTipoGranja(const std::string& nombre, const std::string& descripcion, 
                              int capacidad, double costo, int densidad) {
    auto* db = BaseDatos::getInstancia();
    std::string sql = "INSERT INTO tipos_granja (nombre, descripcion, capacidad_recomendada, costo_inicial, densidad_recomendada) VALUES ('" +
                      nombre + "', '" + descripcion + "', " + std::to_string(capacidad) + ", " +
                      std::to_string(costo) + ", " + std::to_string(densidad) + ")";
    return db->insertarYGetId(sql);
}

std::vector<TipoGranjaInfo> Granjas::listarTiposGranja() {
    auto* db = BaseDatos::getInstancia();
    std::vector<TipoGranjaInfo> resultado;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->abrir(), "SELECT id, nombre, descripcion, capacidad_recomendada, costo_inicial, densidad_recomendada FROM tipos_granja", -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            TipoGranjaInfo t;
            t.id = sqlite3_column_int(stmt, 0);
            const char* n = (const char*)sqlite3_column_text(stmt, 1);
            t.nombre = n ? n : "";
            const char* d = (const char*)sqlite3_column_text(stmt, 2);
            t.descripcion = d ? d : "";
            t.capacidad_recomendada = sqlite3_column_int(stmt, 3);
            t.costo_inicial = sqlite3_column_double(stmt, 4);
            t.densidad_recomendada = sqlite3_column_int(stmt, 5);
            resultado.push_back(t);
        }
    }
    sqlite3_finalize(stmt);
    return resultado;
}

TipoGranjaInfo* Granjas::obtenerTipoGranja(int id) {
    auto* db = BaseDatos::getInstancia();
    std::stringstream ss;
    ss << "SELECT id, nombre, descripcion, capacidad_recomendada, costo_inicial, densidad_recomendada FROM tipos_granja WHERE id = " << id;
    sqlite3_stmt* stmt;
    static TipoGranjaInfo t;
    if (sqlite3_prepare_v2(db->abrir(), ss.str().c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            t.id = sqlite3_column_int(stmt, 0);
            const char* n = (const char*)sqlite3_column_text(stmt, 1);
            t.nombre = n ? n : "";
            const char* d = (const char*)sqlite3_column_text(stmt, 2);
            t.descripcion = d ? d : "";
            t.capacidad_recomendada = sqlite3_column_int(stmt, 3);
            t.costo_inicial = sqlite3_column_double(stmt, 4);
            t.densidad_recomendada = sqlite3_column_int(stmt, 5);
            sqlite3_finalize(stmt);
            return &t;
        }
    }
    sqlite3_finalize(stmt);
    return nullptr;
}

// ==================== GRANJAS ====================

int Granjas::crear(const std::string& nombre, int tipo_granja_id, 
                   const std::string& ubicacion, const std::string& dimensiones, int densidad) {
    auto* db = BaseDatos::getInstancia();
    std::string sql = "INSERT INTO granjas (nombre, tipo_granja_id, ubicacion, dimensiones, densidad_maxima) VALUES ('" +
                      nombre + "', " + std::to_string(tipo_granja_id) + ", '" + ubicacion + "', '" +
                      dimensiones + "', " + std::to_string(densidad) + ")";
    return db->insertarYGetId(sql);
}

Granja* Granjas::obtener(int id) {
    auto* db = BaseDatos::getInstancia();
    std::stringstream ss;
    ss << "SELECT g.id, g.nombre, g.tipo_granja_id, tg.nombre, g.ubicacion, g.dimensiones, g.densidad_maxima, g.activa "
       << "FROM granjas g JOIN tipos_granja tg ON g.tipo_granja_id = tg.id WHERE g.id = " << id;
    sqlite3_stmt* stmt;
    static Granja gr;
    if (sqlite3_prepare_v2(db->abrir(), ss.str().c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            gr.id = sqlite3_column_int(stmt, 0);
            const char* n = (const char*)sqlite3_column_text(stmt, 1);
            gr.nombre = n ? n : "";
            gr.tipo_granja_id = sqlite3_column_int(stmt, 2);
            const char* tn = (const char*)sqlite3_column_text(stmt, 3);
            gr.nombre_tipo = tn ? tn : "";
            const char* ub = (const char*)sqlite3_column_text(stmt, 4);
            gr.ubicacion = ub ? ub : "";
            const char* dim = (const char*)sqlite3_column_text(stmt, 5);
            gr.dimensiones = dim ? dim : "";
            gr.densidad_maxima = sqlite3_column_int(stmt, 6);
            gr.activa = sqlite3_column_int(stmt, 7) == 1;
            sqlite3_finalize(stmt);
            return &gr;
        }
    }
    sqlite3_finalize(stmt);
    return nullptr;
}

std::vector<Granja> Granjas::listar(bool activos) {
    auto* db = BaseDatos::getInstancia();
    std::string sql = "SELECT g.id, g.nombre, g.tipo_granja_id, tg.nombre, g.ubicacion, g.dimensiones, g.densidad_maxima, g.activa "
                      "FROM granjas g JOIN tipos_granja tg ON g.tipo_granja_id = tg.id";
    if (activos) sql += " WHERE g.activa = 1";
    sql += " ORDER BY g.nombre";
    
    std::vector<Granja> resultado;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Granja gr;
            gr.id = sqlite3_column_int(stmt, 0);
            const char* n = (const char*)sqlite3_column_text(stmt, 1);
            gr.nombre = n ? n : "";
            gr.tipo_granja_id = sqlite3_column_int(stmt, 2);
            const char* tn = (const char*)sqlite3_column_text(stmt, 3);
            gr.nombre_tipo = tn ? tn : "";
            const char* ub = (const char*)sqlite3_column_text(stmt, 4);
            gr.ubicacion = ub ? ub : "";
            const char* dim = (const char*)sqlite3_column_text(stmt, 5);
            gr.dimensiones = dim ? dim : "";
            gr.densidad_maxima = sqlite3_column_int(stmt, 6);
            gr.activa = sqlite3_column_int(stmt, 7) == 1;
            resultado.push_back(gr);
        }
    }
    sqlite3_finalize(stmt);
    return resultado;
}

void Granjas::actualizar(int id, const std::string& nombre, int tipo_granja_id, 
                          const std::string& ubicacion, int densidad) {
    auto* db = BaseDatos::getInstancia();
    std::stringstream ss;
    ss << "UPDATE granjas SET nombre = '" << nombre << "', tipo_granja_id = " << tipo_granja_id 
       << ", ubicacion = '" << ubicacion << "', densidad_maxima = " << densidad << " WHERE id = " << id;
    db->ejecutarSQL(ss.str());
}

void Granjas::eliminar(int id) {
    auto* db = BaseDatos::getInstancia();
    std::stringstream ss;
    ss << "UPDATE granjas SET activa = 0 WHERE id = " << id;
    db->ejecutarSQL(ss.str());
}

void Granjas::imprimirGranja(const Granja& g) {
    std::cout << "Granja: " << g.nombre << " (ID: " << g.id << ")" << std::endl;
    std::cout << "  Tipo: " << g.nombre_tipo << std::endl;
    std::cout << "  Ubicación: " << g.ubicacion << std::endl;
    std::cout << "  Dimensiones: " << g.dimensiones << std::endl;
    std::cout << "  Densidad máxima: " << g.densidad_maxima << " pollos/m2" << std::endl;
    std::cout << "  Estado: " << (g.activa ? "ACTIVA" : "INACTIVA") << std::endl;
}