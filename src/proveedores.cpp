#include "../include/proveedores.h"
#include "../include/base_datos.h"
#include <iostream>
#include <iomanip>

int Proveedores::agregar(const std::string& nombre, const std::string& telefono,
                         const std::string& email, const std::string& direccion,
                         const std::string& tipo, int rating) {
    auto* db = BaseDatos::getInstancia();
    std::string tel = telefono.empty() ? "NULL" : "'" + telefono + "'";
    std::string em = email.empty() ? "NULL" : "'" + email + "'";
    std::string dir = direccion.empty() ? "NULL" : "'" + direccion + "'";
    std::string sql = "INSERT INTO proveedores (nombre, telefono, email, direccion, tipo, rating) VALUES ('" +
                      nombre + "', " + tel + ", " + em + ", " + dir + ", '" + tipo + "', " + std::to_string(rating) + ")";
    return db->insertarYGetId(sql);
}

Proveedor* Proveedores::obtener(int id) {
    auto* db = BaseDatos::getInstancia();
    std::string sql = "SELECT id, nombre, telefono, email, direccion, tipo, rating FROM proveedores WHERE id = " + std::to_string(id);
    sqlite3_stmt* stmt;
    static Proveedor p;
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            p.id = sqlite3_column_int(stmt, 0);
            p.nombre = (const char*)sqlite3_column_text(stmt, 1);
            const char* t = (const char*)sqlite3_column_text(stmt, 2);
            p.telefono = t ? t : "";
            const char* e = (const char*)sqlite3_column_text(stmt, 3);
            p.email = e ? e : "";
            const char* d = (const char*)sqlite3_column_text(stmt, 4);
            p.direccion = d ? d : "";
            p.tipo = (const char*)sqlite3_column_text(stmt, 5);
            p.rating = sqlite3_column_int(stmt, 6);
            sqlite3_finalize(stmt);
            return &p;
        }
    }
    sqlite3_finalize(stmt);
    return nullptr;
}

Proveedor* Proveedores::obtenerPorNombre(const std::string& nombre) {
    auto* db = BaseDatos::getInstancia();
    std::string sql = "SELECT id, nombre, telefono, email, direccion, tipo, rating FROM proveedores WHERE nombre = '" + nombre + "'";
    sqlite3_stmt* stmt;
    static Proveedor p;
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            p.id = sqlite3_column_int(stmt, 0);
            p.nombre = (const char*)sqlite3_column_text(stmt, 1);
            const char* t = (const char*)sqlite3_column_text(stmt, 2);
            p.telefono = t ? t : "";
            const char* e = (const char*)sqlite3_column_text(stmt, 3);
            p.email = e ? e : "";
            const char* d = (const char*)sqlite3_column_text(stmt, 4);
            p.direccion = d ? d : "";
            p.tipo = (const char*)sqlite3_column_text(stmt, 5);
            p.rating = sqlite3_column_int(stmt, 6);
            sqlite3_finalize(stmt);
            return &p;
        }
    }
    sqlite3_finalize(stmt);
    return nullptr;
}

std::vector<Proveedor> Proveedores::listar(const std::string& filtro_tipo) {
    auto* db = BaseDatos::getInstancia();
    std::string sql = "SELECT id, nombre, telefono, email, direccion, tipo, rating FROM proveedores WHERE activo = 1";
    if (!filtro_tipo.empty()) sql += " AND tipo = '" + filtro_tipo + "'";
    sql += " ORDER BY nombre";
    
    std::vector<Proveedor> resultado;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Proveedor p;
            p.id = sqlite3_column_int(stmt, 0);
            p.nombre = (const char*)sqlite3_column_text(stmt, 1);
            const char* t = (const char*)sqlite3_column_text(stmt, 2);
            p.telefono = t ? t : "";
            const char* e = (const char*)sqlite3_column_text(stmt, 3);
            p.email = e ? e : "";
            const char* d = (const char*)sqlite3_column_text(stmt, 4);
            p.direccion = d ? d : "";
            p.tipo = (const char*)sqlite3_column_text(stmt, 5);
            p.rating = sqlite3_column_int(stmt, 6);
            resultado.push_back(p);
        }
    }
    sqlite3_finalize(stmt);
    return resultado;
}

std::vector<Proveedor> Proveedores::listarPorCategoria(const std::string& categoria) {
    return listar(categoria);
}

void Proveedores::actualizar(int id, const std::string& nombre, const std::string& telefono,
                             const std::string& email, const std::string& tipo, int rating) {
    auto* db = BaseDatos::getInstancia();
    std::string sql = "UPDATE proveedores SET nombre = '" + nombre + "', telefono = '" + telefono +
                      "', email = '" + email + "', tipo = '" + tipo + "', rating = " + 
                      std::to_string(rating) + " WHERE id = " + std::to_string(id);
    db->ejecutarSQL(sql);
}

void Proveedores::eliminar(int id) {
    auto* db = BaseDatos::getInstancia();
    db->ejecutarSQL("UPDATE proveedores SET activo = 0 WHERE id = " + std::to_string(id));
}

void Proveedores::imprimirProveedor(const Proveedor& p) {
    std::cout << p.id << ": " << p.nombre << " (" << p.telefono << ") - " << p.tipo;
    std::cout << " | Rating: ";
    for (int i = 0; i < p.rating; i++) std::cout << "★";
    for (int i = p.rating; i < 5; i++) std::cout << "☆";
    std::cout << std::endl;
}