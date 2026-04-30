#include "../include/inversores.h"
#include "../include/base_datos.h"
#include "../include/utils.h"
#include <iostream>

int Inversores::agregar(const std::string& nombre, double cantidad_invertida) {
    auto* db = BaseDatos::getInstancia();
    return db->insertarYGetId("INSERT INTO inversores (nombre, cantidad_invertida) VALUES ('" + nombre + "', " + std::to_string(cantidad_invertida) + ")");
}

Inversor* Inversores::obtener(int id) { return nullptr; }
Inversor* Inversores::obtenerPorNombre(const std::string& nombre) { return nullptr; }

std::vector<Inversor> Inversores::listar() {
    auto* db = BaseDatos::getInstancia();
    std::vector<Inversor> resultado;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->abrir(), "SELECT id, nombre, cantidad_invertida FROM inversores ORDER BY nombre", -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Inversor i;
            i.id = sqlite3_column_int(stmt, 0);
            i.nombre = (const char*)sqlite3_column_text(stmt, 1);
            i.cantidad_invertida = sqlite3_column_double(stmt, 2);
            resultado.push_back(i);
        }
    }
    sqlite3_finalize(stmt);
    return resultado;
}

void Inversores::actualizar(int id, const std::string& nombre, double cantidad_invertida) {}
void Inversores::eliminar(int id) {}

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
    std::vector<std::pair<std::string, double>> resultado;
    return resultado;
}

int Inversores::agregarEncargado(const std::string& nombre) {
    auto* db = BaseDatos::getInstancia();
    return db->insertarYGetId("INSERT INTO encargado (nombre) VALUES ('" + nombre + "')");
}

std::vector<Encargado> Inversores::listarEncargados() { return {}; }
void Inversores::eliminarEncargado(int id) {}

void Inversores::cargarEjemplo() {
    agregar("Leomarlys Bejarano", 210.0);
    agregar("Felix Peroza", 1.0);
    agregarEncargado("Rooselvelt Angulo");
    agregarEncargado("Milagros Lezama");
    std::cout << "Inversores y encargados de ejemplo cargados" << std::endl;
}