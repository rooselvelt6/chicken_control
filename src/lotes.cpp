#include "../include/lotes.h"
#include "../include/base_datos.h"
#include "../include/utils.h"
#include <sstream>
#include <iostream>

int Lotes::crear(int numero, const std::string& fecha_inicio, int duracion) {
    auto* db = BaseDatos::getInstancia();
    std::string fecha_segura = sanitizarSQL(sanitizarInput(fecha_inicio, 10));
    std::string sql = "INSERT INTO lotes (numero, fecha_inicio, duracion_dias, activo) VALUES (" +
                      std::to_string(numero) + ", '" + fecha_segura + "', " +
                      std::to_string(duracion) + ", 1)";
    return db->insertarYGetId(sql);
}

Lote* Lotes::obtener(int id) {
    auto* db = BaseDatos::getInstancia();
    std::string sql = "SELECT id, numero, fecha_inicio, fecha_fin, duracion_dias, activo FROM lotes WHERE id = " + std::to_string(id);
    sqlite3_stmt* stmt;
    static Lote lote;
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            lote.id = sqlite3_column_int(stmt, 0);
            lote.numero = sqlite3_column_int(stmt, 1);
            const char* f = (const char*)sqlite3_column_text(stmt, 2);
            lote.fecha_inicio = f ? f : "";
            const char* ff = (const char*)sqlite3_column_text(stmt, 3);
            lote.fecha_fin = ff ? ff : "";
            lote.duracion_dias = sqlite3_column_int(stmt, 4);
            lote.activo = sqlite3_column_int(stmt, 5) == 1;
            sqlite3_finalize(stmt);
            return &lote;
        }
    }
    sqlite3_finalize(stmt);
    return nullptr;
}

std::vector<Lote> Lotes::listar(bool activos) {
    auto* db = BaseDatos::getInstancia();
    std::string sql = "SELECT id, numero, fecha_inicio, fecha_fin, duracion_dias, activo FROM lotes";
    if (activos) sql += " WHERE activo = 1";
    sql += " ORDER BY numero DESC";
    std::vector<Lote> resultado;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Lote l;
            l.id = sqlite3_column_int(stmt, 0);
            l.numero = sqlite3_column_int(stmt, 1);
            const char* f = (const char*)sqlite3_column_text(stmt, 2);
            l.fecha_inicio = f ? f : "";
            const char* ff = (const char*)sqlite3_column_text(stmt, 3);
            l.fecha_fin = ff ? ff : "";
            l.duracion_dias = sqlite3_column_int(stmt, 4);
            l.activo = sqlite3_column_int(stmt, 5) == 1;
            resultado.push_back(l);
        }
    }
    sqlite3_finalize(stmt);
    return resultado;
}

void Lotes::cerrar(int id, const std::string& fecha_fin) {
    auto* db = BaseDatos::getInstancia();
    std::string fecha_segura = sanitizarSQL(sanitizarInput(fecha_fin, 10));
    std::string sql = "UPDATE lotes SET activo = 0, fecha_fin = '" + fecha_segura + "' WHERE id = " + std::to_string(id);
    db->ejecutarSQL(sql);
}

void Lotes::actualizar(int id, int numero, const std::string& fecha_inicio, int duracion) {
    auto* db = BaseDatos::getInstancia();
    std::string fecha_segura = sanitizarSQL(sanitizarInput(fecha_inicio, 10));
    std::string sql = "UPDATE lotes SET numero = " + std::to_string(numero) +
                      ", fecha_inicio = '" + fecha_segura +
                      "', duracion_dias = " + std::to_string(duracion) +
                      " WHERE id = " + std::to_string(id);
    db->ejecutarSQL(sql);
}

void Lotes::eliminar(int id) {
    auto* db = BaseDatos::getInstancia();
    db->ejecutarSQL("DELETE FROM lotes WHERE id = " + std::to_string(id));
}

Lote* Lotes::loteActivoActual() {
    auto* db = BaseDatos::getInstancia();
    std::string sql = "SELECT id, numero, fecha_inicio, fecha_fin, duracion_dias, activo FROM lotes WHERE activo = 1 ORDER BY id DESC LIMIT 1";
    sqlite3_stmt* stmt;
    static Lote lote;
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            lote.id = sqlite3_column_int(stmt, 0);
            lote.numero = sqlite3_column_int(stmt, 1);
            const char* f = (const char*)sqlite3_column_text(stmt, 2);
            lote.fecha_inicio = f ? f : "";
            const char* ff = (const char*)sqlite3_column_text(stmt, 3);
            lote.fecha_fin = ff ? ff : "";
            lote.duracion_dias = sqlite3_column_int(stmt, 4);
            lote.activo = sqlite3_column_int(stmt, 5) == 1;
            sqlite3_finalize(stmt);
            return &lote;
        }
    }
    sqlite3_finalize(stmt);
    return nullptr;
}

int Lotes::ultimoNumero() {
    auto* db = BaseDatos::getInstancia();
    std::string sql = "SELECT COALESCE(MAX(numero), 0) FROM lotes";
    sqlite3_stmt* stmt;
    int resultado = 0;
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            resultado = sqlite3_column_int(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);
    return resultado;
}

void Lotes::imprimirLote(const Lote& l) {
    std::cout << "Lote #" << l.numero << " - Inicio: " << l.fecha_inicio
              << " - Duración: " << l.duracion_dias << " días - "
              << (l.activo ? "ACTIVO" : "CERRADO") << std::endl;
}