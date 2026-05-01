#include "../include/herramientas.h"
#include "../include/base_datos.h"
#include "../include/utils.h"
#include <iostream>
#include <iomanip>

int Herramientas::agregar(const std::string& nombre, int cantidad, double precio_unitario, const std::string& tipo) {
    auto* db = BaseDatos::getInstancia();
    std::string tipo_sql = tipo.empty() ? "NULL" : "'" + tipo + "'";
    return db->insertarYGetId("INSERT INTO herramientas (nombre, cantidad, precio_unitario, tipo) VALUES ('" + nombre + "', " + std::to_string(cantidad) + ", " + std::to_string(precio_unitario) + ", " + tipo_sql + ")");
}

Herramienta* Herramientas::obtener(int id) {
    auto* db = BaseDatos::getInstancia();
    static Herramienta h;
    sqlite3_stmt* stmt;
    std::string sql = "SELECT id, nombre, cantidad, precio_unitario, tipo FROM herramientas WHERE id = " + std::to_string(id);
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            h.id = sqlite3_column_int(stmt, 0);
            h.nombre = (const char*)sqlite3_column_text(stmt, 1);
            h.cantidad = sqlite3_column_int(stmt, 2);
            h.precio_unitario = sqlite3_column_double(stmt, 3);
            const char* t = (const char*)sqlite3_column_text(stmt, 4);
            h.tipo = t ? t : "";
            sqlite3_finalize(stmt);
            return &h;
        }
    }
    sqlite3_finalize(stmt);
    return nullptr;
}

std::vector<Herramienta> Herramientas::listar() {
    auto* db = BaseDatos::getInstancia();
    std::vector<Herramienta> resultado;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->abrir(), "SELECT id, nombre, cantidad, precio_unitario, tipo FROM herramientas ORDER BY nombre", -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Herramienta h;
            h.id = sqlite3_column_int(stmt, 0);
            h.nombre = (const char*)sqlite3_column_text(stmt, 1);
            h.cantidad = sqlite3_column_int(stmt, 2);
            h.precio_unitario = sqlite3_column_double(stmt, 3);
            const char* t = (const char*)sqlite3_column_text(stmt, 4);
            h.tipo = t ? t : "";
            resultado.push_back(h);
        }
    }
    sqlite3_finalize(stmt);
    return resultado;
}

void Herramientas::actualizar(int id, const std::string& nombre, int cantidad, double precio_unitario) {
    auto* db = BaseDatos::getInstancia();
    db->ejecutarSQL("UPDATE herramientas SET nombre = '" + nombre + "', cantidad = " + std::to_string(cantidad) + ", precio_unitario = " + std::to_string(precio_unitario) + " WHERE id = " + std::to_string(id));
}

void Herramientas::eliminar(int id) {
    auto* db = BaseDatos::getInstancia();
    db->ejecutarSQL("DELETE FROM herramientas WHERE id = " + std::to_string(id));
}

double Herramientas::totalInvertido() {
    auto* db = BaseDatos::getInstancia();
    sqlite3_stmt* stmt;
    double total = 0.0;
    if (sqlite3_prepare_v2(db->abrir(), "SELECT COALESCE(SUM(cantidad * precio_unitario), 0) FROM herramientas", -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) total = sqlite3_column_double(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return total;
}

void Herramientas::cargarEjemplo() {
    agregar("Tornillos", 20, 0.10, "ferreteria");
    agregar("Madera", 2, 0.0, "materiales");
    agregar("Socates", 6, 0.4, "electricidad");
    agregar("Bombillos", 6, 0.56, "electricidad");
    agregar("Corral", 2, 0.0, "infraestructura");
    agregar("Cama", 2, 0.0, "materiales");
    agregar("Comedero", 2, 4.0, "equipamiento");
    agregar("Medicinas", 4, 4.0, "veterinaria");
    agregar("Cable", 4, 1.0, "electricidad");
    agregar("Transporte", 4, 2.0, "servicios");
    agregar("Mano de Obra", 10, 0.0, "servicios");
    agregar("Saldo Teléfono", 0, 2.0, "comunicacion");
    std::cout << "Herramientas de ejemplo cargadas" << std::endl;
}