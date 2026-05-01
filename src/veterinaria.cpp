#include "../include/veterinaria.h"
#include "../include/base_datos.h"
#include "../include/modelos.h"
#include <iostream>
#include <iomanip>
#include <sstream>

// Vacunas
int Veterinaria::agregarVacuna(const std::string& nombre, int aplicacion_dias, 
                                 double costo, int proveedor_id, const std::string& observaciones) {
    auto* db = BaseDatos::getInstancia();
    std::string prov = (proveedor_id > 0) ? std::to_string(proveedor_id) : "NULL";
    std::string obs = observaciones.empty() ? "NULL" : "'" + observaciones + "'";
    std::stringstream ss;
    ss << "INSERT INTO vacunas (nombre, aplicacion_dias, costo, proveedor_id, observaciones) VALUES ('"
       << nombre << "', " << aplicacion_dias << ", " << costo << ", " << prov << ", " << obs << ")";
    return db->insertarYGetId(ss.str());
}

Vacuna* Veterinaria::obtenerVacuna(int id) {
    auto* db = BaseDatos::getInstancia();
    std::stringstream ss;
    ss << "SELECT v.id, v.nombre, v.aplicacion_dias, v.costo, v.proveedor_id, v.observaciones, p.nombre "
       << "FROM vacunas v LEFT JOIN proveedores p ON v.proveedor_id = p.id WHERE v.id = " << id;
    sqlite3_stmt* stmt;
    static Vacuna v;
    if (sqlite3_prepare_v2(db->abrir(), ss.str().c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            v.id = sqlite3_column_int(stmt, 0);
            v.nombre = (const char*)sqlite3_column_text(stmt, 1);
            v.aplicacion_dias = sqlite3_column_int(stmt, 2);
            v.costo = sqlite3_column_double(stmt, 3);
            v.proveedor_id = sqlite3_column_int(stmt, 4);
            const char* o = (const char*)sqlite3_column_text(stmt, 5);
            v.observaciones = o ? o : "";
            const char* pn = (const char*)sqlite3_column_text(stmt, 6);
            v.nombre_proveedor = pn ? pn : "";
            sqlite3_finalize(stmt);
            return &v;
        }
    }
    sqlite3_finalize(stmt);
    return nullptr;
}

std::vector<Vacuna> Veterinaria::listarVacunas() {
    auto* db = BaseDatos::getInstancia();
    std::vector<Vacuna> resultado;
    sqlite3_stmt* stmt;
    std::string sql = "SELECT v.id, v.nombre, v.aplicacion_dias, v.costo, v.proveedor_id, v.observaciones, p.nombre "
                      "FROM vacunas v LEFT JOIN proveedores p ON v.proveedor_id = p.id WHERE v.activa = 1 ORDER BY v.aplicacion_dias";
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Vacuna v;
            v.id = sqlite3_column_int(stmt, 0);
            v.nombre = (const char*)sqlite3_column_text(stmt, 1);
            v.aplicacion_dias = sqlite3_column_int(stmt, 2);
            v.costo = sqlite3_column_double(stmt, 3);
            v.proveedor_id = sqlite3_column_int(stmt, 4);
            const char* o = (const char*)sqlite3_column_text(stmt, 5);
            v.observaciones = o ? o : "";
            const char* pn = (const char*)sqlite3_column_text(stmt, 6);
            v.nombre_proveedor = pn ? pn : "";
            resultado.push_back(v);
        }
    }
    sqlite3_finalize(stmt);
    return resultado;
}

std::vector<Vacuna> Veterinaria::listarVacunasPorDia(int dia) {
    auto* db = BaseDatos::getInstancia();
    std::vector<Vacuna> resultado;
    sqlite3_stmt* stmt;
    std::stringstream ss;
    ss << "SELECT v.id, v.nombre, v.aplicacion_dias, v.costo, v.proveedor_id, v.observaciones, p.nombre "
       << "FROM vacunas v LEFT JOIN proveedores p ON v.proveedor_id = p.id WHERE v.activa = 1 AND v.aplicacion_dias = " << dia;
    if (sqlite3_prepare_v2(db->abrir(), ss.str().c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Vacuna v;
            v.id = sqlite3_column_int(stmt, 0);
            v.nombre = (const char*)sqlite3_column_text(stmt, 1);
            v.aplicacion_dias = sqlite3_column_int(stmt, 2);
            v.costo = sqlite3_column_double(stmt, 3);
            v.proveedor_id = sqlite3_column_int(stmt, 4);
            const char* o = (const char*)sqlite3_column_text(stmt, 5);
            v.observaciones = o ? o : "";
            const char* pn = (const char*)sqlite3_column_text(stmt, 6);
            v.nombre_proveedor = pn ? pn : "";
            resultado.push_back(v);
        }
    }
    sqlite3_finalize(stmt);
    return resultado;
}

// Medicamentos
int Veterinaria::agregarMedicamento(const std::string& nombre, const std::string& principio_activo,
                                     const std::string& dosis, double precio, int proveedor_id,
                                     const std::string& categoria, int stock) {
    auto* db = BaseDatos::getInstancia();
    std::string prov = (proveedor_id > 0) ? std::to_string(proveedor_id) : "NULL";
    std::string principio = principio_activo.empty() ? "NULL" : "'" + principio_activo + "'";
    std::stringstream ss;
    ss << "INSERT INTO medicamentos (nombre, principio_activo, dosis, precio, proveedor_id, categoria, stock) VALUES ('"
       << nombre << "', " << principio << ", '" << dosis << "', " << precio << ", " 
       << prov << ", '" << categoria << "', " << stock << ")";
    return db->insertarYGetId(ss.str());
}

Medicamento* Veterinaria::obtenerMedicamento(int id) {
    auto* db = BaseDatos::getInstancia();
    std::stringstream ss;
    ss << "SELECT m.id, m.nombre, m.principio_activo, m.dosis, m.precio, m.proveedor_id, m.categoria, m.stock, p.nombre "
       << "FROM medicamentos m LEFT JOIN proveedores p ON m.proveedor_id = p.id WHERE m.id = " << id;
    sqlite3_stmt* stmt;
    static Medicamento m;
    if (sqlite3_prepare_v2(db->abrir(), ss.str().c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            m.id = sqlite3_column_int(stmt, 0);
            m.nombre = (const char*)sqlite3_column_text(stmt, 1);
            const char* pa = (const char*)sqlite3_column_text(stmt, 2);
            m.principio_activo = pa ? pa : "";
            m.dosis = (const char*)sqlite3_column_text(stmt, 3);
            m.precio = sqlite3_column_double(stmt, 4);
            m.proveedor_id = sqlite3_column_int(stmt, 5);
            m.categoria = (const char*)sqlite3_column_text(stmt, 6);
            m.stock = sqlite3_column_int(stmt, 7);
            const char* pn = (const char*)sqlite3_column_text(stmt, 8);
            m.nombre_proveedor = pn ? pn : "";
            sqlite3_finalize(stmt);
            return &m;
        }
    }
    sqlite3_finalize(stmt);
    return nullptr;
}

std::vector<Medicamento> Veterinaria::listarMedicamentos() {
    auto* db = BaseDatos::getInstancia();
    std::vector<Medicamento> resultado;
    sqlite3_stmt* stmt;
    std::string sql = "SELECT m.id, m.nombre, m.principio_activo, m.dosis, m.precio, m.proveedor_id, m.categoria, m.stock, p.nombre "
                      "FROM medicamentos m LEFT JOIN proveedores p ON m.proveedor_id = p.id WHERE m.activo = 1 ORDER BY m.nombre";
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Medicamento m;
            m.id = sqlite3_column_int(stmt, 0);
            m.nombre = (const char*)sqlite3_column_text(stmt, 1);
            const char* pa = (const char*)sqlite3_column_text(stmt, 2);
            m.principio_activo = pa ? pa : "";
            m.dosis = (const char*)sqlite3_column_text(stmt, 3);
            m.precio = sqlite3_column_double(stmt, 4);
            m.proveedor_id = sqlite3_column_int(stmt, 5);
            m.categoria = (const char*)sqlite3_column_text(stmt, 6);
            m.stock = sqlite3_column_int(stmt, 7);
            const char* pn = (const char*)sqlite3_column_text(stmt, 8);
            m.nombre_proveedor = pn ? pn : "";
            resultado.push_back(m);
        }
    }
    sqlite3_finalize(stmt);
    return resultado;
}

void Veterinaria::imprimirVacunas() {
    auto listas = listarVacunas();
    std::cout << "=== VACUNAS REGISTRADAS ===" << std::endl;
    for (auto& v : listas) {
        std::cout << v.id << ": " << v.nombre << " | Día: " << v.aplicacion_dias 
                  << " | Costo: USD " << std::fixed << std::setprecision(2) << v.costo;
        if (!v.nombre_proveedor.empty()) std::cout << " | Proveedor: " << v.nombre_proveedor;
        std::cout << std::endl;
    }
}

void Veterinaria::imprimirMedicamentos() {
    auto listas = listarMedicamentos();
    std::cout << "=== MEDICAMENTOS REGISTRADOS ===" << std::endl;
    for (auto& m : listas) {
        std::cout << m.id << ": " << m.nombre << " | Categoría: " << m.categoria 
                  << " | Stock: " << m.stock << " | Precio: USD " << std::fixed << std::setprecision(2) << m.precio;
        std::cout << std::endl;
    }
}