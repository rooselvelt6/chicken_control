#include "../include/inversores.h"
#include "../include/base_datos.h"
#include "../include/utils.h"
#include <iostream>

int Inversores::agregar(const std::string& nombre, double cantidad_invertida) {
    auto* db = BaseDatos::getInstancia();
    std::string nombre_seguro = sanitizarSQL(sanitizarInput(nombre, 100));
    return db->insertarYGetId("INSERT INTO inversores (nombre, cantidad_invertida) VALUES ('" + nombre_seguro + "', " + std::to_string(cantidad_invertida) + ")");
}

Inversor* Inversores::obtener(int id) {
    auto* db = BaseDatos::getInstancia();
    static Inversor i;
    sqlite3_stmt* stmt;
    std::string sql = "SELECT id, nombre, cantidad_invertida FROM inversores WHERE id = " + std::to_string(id);
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            i.id = sqlite3_column_int(stmt, 0);
            const char* n = (const char*)sqlite3_column_text(stmt, 1);
            i.nombre = n ? n : "";
            i.cantidad_invertida = sqlite3_column_double(stmt, 2);
            sqlite3_finalize(stmt);
            return &i;
        }
    }
    sqlite3_finalize(stmt);
    return nullptr;
}

Inversor* Inversores::obtenerPorNombre(const std::string& nombre) {
    auto* db = BaseDatos::getInstancia();
    static Inversor i;
    sqlite3_stmt* stmt;
    std::string nombre_seguro = sanitizarSQL(sanitizarInput(nombre, 100));
    std::string sql = "SELECT id, nombre, cantidad_invertida FROM inversores WHERE nombre = '" + nombre_seguro + "'";
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            i.id = sqlite3_column_int(stmt, 0);
            const char* n = (const char*)sqlite3_column_text(stmt, 1);
            i.nombre = n ? n : "";
            i.cantidad_invertida = sqlite3_column_double(stmt, 2);
            sqlite3_finalize(stmt);
            return &i;
        }
    }
    sqlite3_finalize(stmt);
    return nullptr;
}

std::vector<Inversor> Inversores::listar() {
    auto* db = BaseDatos::getInstancia();
    std::vector<Inversor> resultado;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->abrir(), "SELECT id, nombre, cantidad_invertida FROM inversores ORDER BY nombre", -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Inversor i;
            i.id = sqlite3_column_int(stmt, 0);
            const char* n = (const char*)sqlite3_column_text(stmt, 1);
            i.nombre = n ? n : "";
            i.cantidad_invertida = sqlite3_column_double(stmt, 2);
            resultado.push_back(i);
        }
    }
    sqlite3_finalize(stmt);
    return resultado;
}

void Inversores::actualizar(int id, const std::string& nombre, double cantidad_invertida) {
    auto* db = BaseDatos::getInstancia();
    std::string nombre_seguro = sanitizarSQL(sanitizarInput(nombre, 100));
    db->ejecutarSQL("UPDATE inversores SET nombre = '" + nombre_seguro + "', cantidad_invertida = " + std::to_string(cantidad_invertida) + " WHERE id = " + std::to_string(id));
}

void Inversores::eliminar(int id) {
    auto* db = BaseDatos::getInstancia();
    db->ejecutarSQL("DELETE FROM inversores WHERE id = " + std::to_string(id));
}

double Inversores::totalInvertido() {
    auto* db = BaseDatos::getInstancia();
    sqlite3_stmt* stmt;
    double total = 0.0;
    if (sqlite3_prepare_v2(db->abrir(), "SELECT COALESCE(SUM(cantidad_invertida), 0) FROM inversores", -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) total = sqlite3_column_double(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return total;
}

std::vector<std::pair<std::string, double>> Inversores::gananciasPorInversor(double total_ganancias) {
    auto inversores = listar();
    double total_invertido = totalInvertido();
    std::vector<std::pair<std::string, double>> resultado;
    
    for (const auto& i : inversores) {
        double porcentaje = (total_invertido > 0) ? (i.cantidad_invertida / total_invertido) : 0;
        double ganancia = total_ganancias * porcentaje;
        resultado.push_back({i.nombre, ganancia});
    }
    return resultado;
}

int Inversores::agregarEncargado(const std::string& nombre) {
    auto* db = BaseDatos::getInstancia();
    std::string nombre_seguro = sanitizarSQL(sanitizarInput(nombre, 100));
    return db->insertarYGetId("INSERT INTO encargado (nombre) VALUES ('" + nombre_seguro + "')");
}

std::vector<Encargado> Inversores::listarEncargados() {
    auto* db = BaseDatos::getInstancia();
    std::vector<Encargado> resultado;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->abrir(), "SELECT id, nombre FROM encargado ORDER BY nombre", -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Encargado e;
            e.id = sqlite3_column_int(stmt, 0);
            const char* n = (const char*)sqlite3_column_text(stmt, 1);
            e.nombre = n ? n : "";
            resultado.push_back(e);
        }
    }
    sqlite3_finalize(stmt);
    return resultado;
}

void Inversores::eliminarEncargado(int id) {
    auto* db = BaseDatos::getInstancia();
    db->ejecutarSQL("DELETE FROM encargado WHERE id = " + std::to_string(id));
}

void Inversores::cargarEjemplo() {
    std::cout << "Inversores y encargados de ejemplo cargados" << std::endl;
}