#include "../include/alimentacion.h"
#include "../include/base_datos.h"
#include "../include/utils.h"
#include <sstream>
#include <iostream>
#include <iomanip>

int Alimentacion::agregar(const std::string& nombre, const std::string& marca, Fase fase,
                          const AnalisisNutricional& analisis, int contenido_kg,
                          double precio_unitario, int inventario, int inicio_dias, int final_dias) {
    auto* db = BaseDatos::getInstancia();
    std::string marca_sql = marca.empty() ? "NULL" : "'" + marca + "'";
    std::string sql = "INSERT INTO alimentos (nombre, marca, fase, proteina, humedad, grasa, ceniza, calcio, fibra, fosforo, contenido_kg, precio_unitario, inventario, inicio_dias, final_dias) VALUES ('" +
                      nombre + "', " + marca_sql + ", '" + faseToString(fase) + "', " +
                      std::to_string(analisis.proteina) + ", " + std::to_string(analisis.humedad) + ", " +
                      std::to_string(analisis.grasa) + ", " + std::to_string(analisis.ceniza) + ", " +
                      std::to_string(analisis.calcio) + ", " + std::to_string(analisis.fibra) + ", " +
                      std::to_string(analisis.fosforo) + ", " + std::to_string(contenido_kg) + ", " +
                      std::to_string(precio_unitario) + ", " + std::to_string(inventario) + ", " +
                      std::to_string(inicio_dias) + ", " + std::to_string(final_dias) + ")";
    return db->insertarYGetId(sql);
}

Alimento* Alimentacion::obtener(int id) { return nullptr; }

Alimento* Alimentacion::obtenerPorNombre(const std::string& nombre) {
    auto* db = BaseDatos::getInstancia();
    std::string sql = "SELECT id, nombre, marca, fase, proteina, contenido_kg, precio_unitario, inventario FROM alimentos WHERE nombre = '" + nombre + "'";
    sqlite3_stmt* stmt;
    static Alimento a;
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            a.id = sqlite3_column_int(stmt, 0);
            a.nombre = (const char*)sqlite3_column_text(stmt, 1);
            const char* m = (const char*)sqlite3_column_text(stmt, 2);
            a.marca = m ? m : "";
            const char* f = (const char*)sqlite3_column_text(stmt, 3);
            a.fase = f ? stringToFase(f) : Fase::Desconocido;
            a.analisis.proteina = sqlite3_column_double(stmt, 4);
            a.contenido_kg = sqlite3_column_int(stmt, 5);
            a.precio_unitario = sqlite3_column_double(stmt, 6);
            a.inventario = sqlite3_column_int(stmt, 7);
            sqlite3_finalize(stmt);
            return &a;
        }
    }
    sqlite3_finalize(stmt);
    return nullptr;
}

std::vector<Alimento> Alimentacion::listar() {
    auto* db = BaseDatos::getInstancia();
    std::vector<Alimento> resultado;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->abrir(), "SELECT id, nombre, marca, fase, proteina, contenido_kg, precio_unitario, inventario FROM alimentos ORDER BY nombre", -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Alimento a;
            a.id = sqlite3_column_int(stmt, 0);
            a.nombre = (const char*)sqlite3_column_text(stmt, 1);
            const char* m = (const char*)sqlite3_column_text(stmt, 2);
            a.marca = m ? m : "";
            const char* f = (const char*)sqlite3_column_text(stmt, 3);
            a.fase = f ? stringToFase(f) : Fase::Desconocido;
            a.analisis.proteina = sqlite3_column_double(stmt, 4);
            a.contenido_kg = sqlite3_column_int(stmt, 5);
            a.precio_unitario = sqlite3_column_double(stmt, 6);
            a.inventario = sqlite3_column_int(stmt, 7);
            resultado.push_back(a);
        }
    }
    sqlite3_finalize(stmt);
    return resultado;
}

std::vector<Alimento> Alimentacion::listarPorFase(Fase fase) { return listar(); }

void Alimentacion::actualizarInventario(int alimento_id, int cantidad) {
    auto* db = BaseDatos::getInstancia();
    db->ejecutarSQL("UPDATE alimentos SET inventario = " + std::to_string(cantidad) + " WHERE id = " + std::to_string(alimento_id));
}

int Alimentacion::registrarConsumo(int lote_id, int alimento_id, int cantidad_sacos) {
    auto* db = BaseDatos::getInstancia();
    std::string sql = "INSERT INTO consumo_alimento (lote_id, alimento_id, cantidad_sacos) VALUES (" +
                      std::to_string(lote_id) + ", " + std::to_string(alimento_id) + ", " + std::to_string(cantidad_sacos) + ")";
    int id = db->insertarYGetId(sql);
    db->ejecutarSQL("UPDATE alimentos SET inventario = inventario - " + std::to_string(cantidad_sacos) + " WHERE id = " + std::to_string(alimento_id));
    return id;
}

std::vector<ConsumoAlimento> Alimentacion::listarConsumo(int lote_id) {
    std::vector<ConsumoAlimento> resultado;
    return resultado;
}

std::pair<int, double> Alimentacion::totalConsumo(int lote_id) {
    auto* db = BaseDatos::getInstancia();
    std::string sql = "SELECT COALESCE(SUM(c.cantidad_sacos), 0), COALESCE(SUM(c.cantidad_sacos * a.precio_unitario), 0) FROM consumo_alimento c JOIN alimentos a ON c.alimento_id = a.id WHERE c.lote_id = " + std::to_string(lote_id);
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

double Alimentacion::totalInvertido(int lote_id) { return totalConsumo(lote_id).second; }

void Alimentacion::imprimirInventario() {
    std::cout << "=== Inventario de Alimentos ===" << std::endl;
    for (auto& a : listar()) {
        std::cout << "  " << a.nombre << " (" << faseToString(a.fase) << ") - Inventario: " << a.inventario << " sacks - Bs. " << std::fixed << std::setprecision(2) << a.precio_unitario << "/saco" << std::endl;
    }
}

void Alimentacion::cargarEjemplo() {
    AnalisisNutricional a;
    agregar("Preinicial 1", "SUPER S", Fase::Bebe, a, 40, 37.0, 2, 0, 26);
    agregar("Preinicial 3", "SUPER S", Fase::Intermedio, a, 40, 37.0, 3, 26, 45);
    agregar("Engorde", "SUPER S", Fase::Grande, a, 40, 40.0, 0, 45, 999);
    std::cout << "Alimentos de ejemplo cargados" << std::endl;
}