#include "../include/servicios.h"
#include "../include/base_datos.h"
#include "../include/utils.h"
#include <iostream>
#include <sstream>

std::string Servicios::tipoToString(TipoServicio t) {
    switch (t) {
        case TipoServicio::Agua: return "agua";
        case TipoServicio::Luz: return "luz";
        case TipoServicio::Gas: return "gas";
        case TipoServicio::Internet: return "internet";
        case TipoServicio::Telefono: return "telefono";
        default: return "otro";
    }
}

TipoServicio Servicios::stringToTipo(const std::string& s) {
    if (s == "agua") return TipoServicio::Agua;
    if (s == "luz") return TipoServicio::Luz;
    if (s == "gas") return TipoServicio::Gas;
    if (s == "internet") return TipoServicio::Internet;
    if (s == "telefono") return TipoServicio::Telefono;
    return TipoServicio::Otro;
}

int Servicios::agregarConsumo(int lote_id, TipoServicio tipo, double cantidad, double costo_unitario, const std::string& descripcion) {
    auto* db = BaseDatos::getInstancia();
    std::string fecha = fechaActual();
    double costo_total = cantidad * costo_unitario;
    std::string desc = descripcion.empty() ? "NULL" : "'" + descripcion + "'";
    std::string sql = "INSERT INTO servicios_consumo (lote_id, tipo, cantidad, costo_unitario, costo_total, fecha, descripcion) VALUES (" +
        std::to_string(lote_id) + ", '" + tipoToString(tipo) + "', " + std::to_string(cantidad) + ", " +
        std::to_string(costo_unitario) + ", " + std::to_string(costo_total) + ", '" + fecha + "', " + desc + ")";
    return db->insertarYGetId(sql);
}

std::vector<ConsumoServicio> Servicios::listarConsumo(int lote_id) {
    auto* db = BaseDatos::getInstancia();
    std::vector<ConsumoServicio> resultado;
    sqlite3_stmt* stmt;
    std::string sql;
    if (lote_id > 0) {
        sql = "SELECT id, lote_id, tipo, cantidad, costo_unitario, costo_total, fecha, descripcion FROM servicios_consumo WHERE lote_id = " + std::to_string(lote_id) + " ORDER BY fecha DESC";
    } else {
        sql = "SELECT id, lote_id, tipo, cantidad, costo_unitario, costo_total, fecha, descripcion FROM servicios_consumo ORDER BY fecha DESC";
    }
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            ConsumoServicio c;
            c.id = sqlite3_column_int(stmt, 0);
            c.lote_id = sqlite3_column_int(stmt, 1);
            std::string t = (const char*)sqlite3_column_text(stmt, 2);
            c.tipo = stringToTipo(t);
            c.cantidad = sqlite3_column_double(stmt, 3);
            c.costo_unitario = sqlite3_column_double(stmt, 4);
            c.costo_total = sqlite3_column_double(stmt, 5);
            const char* f = (const char*)sqlite3_column_text(stmt, 6);
            c.fecha = f ? f : "";
            const char* d = (const char*)sqlite3_column_text(stmt, 7);
            c.descripcion = d ? d : "";
            resultado.push_back(c);
        }
    }
    sqlite3_finalize(stmt);
    return resultado;
}

std::vector<ConsumoServicio> Servicios::listarPorTipo(TipoServicio tipo) {
    auto* db = BaseDatos::getInstancia();
    std::vector<ConsumoServicio> resultado;
    sqlite3_stmt* stmt;
    std::string sql = "SELECT id, lote_id, tipo, cantidad, costo_unitario, costo_total, fecha, descripcion FROM servicios_consumo WHERE tipo = '" + tipoToString(tipo) + "' ORDER BY fecha DESC";
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            ConsumoServicio c;
            c.id = sqlite3_column_int(stmt, 0);
            c.lote_id = sqlite3_column_int(stmt, 1);
            c.tipo = tipo;
            c.cantidad = sqlite3_column_double(stmt, 3);
            c.costo_unitario = sqlite3_column_double(stmt, 4);
            c.costo_total = sqlite3_column_double(stmt, 5);
            const char* f = (const char*)sqlite3_column_text(stmt, 6);
            c.fecha = f ? f : "";
            const char* d = (const char*)sqlite3_column_text(stmt, 7);
            c.descripcion = d ? d : "";
            resultado.push_back(c);
        }
    }
    sqlite3_finalize(stmt);
    return resultado;
}

double Servicios::getTotalCosto(int lote_id) {
    auto* db = BaseDatos::getInstancia();
    sqlite3_stmt* stmt;
    double total = 0;
    std::string sql = "SELECT COALESCE(SUM(costo_total), 0) FROM servicios_consumo WHERE lote_id = " + std::to_string(lote_id);
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            total = sqlite3_column_double(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);
    return total;
}

double Servicios::getTotalCostoPorTipo(TipoServicio tipo) {
    auto* db = BaseDatos::getInstancia();
    sqlite3_stmt* stmt;
    double total = 0;
    std::string sql = "SELECT COALESCE(SUM(costo_total), 0) FROM servicios_consumo WHERE tipo = '" + tipoToString(tipo) + "'";
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            total = sqlite3_column_double(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);
    return total;
}

std::pair<double, double> Servicios::getCostoYPesoAgua(int lote_id) {
    auto* db = BaseDatos::getInstancia();
    sqlite3_stmt* stmt;
    double cantidad = 0, costo = 0;
    std::string sql = "SELECT COALESCE(SUM(cantidad), 0), COALESCE(SUM(costo_total), 0) FROM servicios_consumo WHERE lote_id = " + std::to_string(lote_id) + " AND tipo = 'agua'";
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            cantidad = sqlite3_column_double(stmt, 0);
            costo = sqlite3_column_double(stmt, 1);
        }
    }
    sqlite3_finalize(stmt);
    return {costo, cantidad};
}

std::pair<double, double> Servicios::getCostoYPesoLuz(int lote_id) {
    auto* db = BaseDatos::getInstancia();
    sqlite3_stmt* stmt;
    double cantidad = 0, costo = 0;
    std::string sql = "SELECT COALESCE(SUM(cantidad), 0), COALESCE(SUM(costo_total), 0) FROM servicios_consumo WHERE lote_id = " + std::to_string(lote_id) + " AND tipo = 'luz'";
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            cantidad = sqlite3_column_double(stmt, 0);
            costo = sqlite3_column_double(stmt, 1);
        }
    }
    sqlite3_finalize(stmt);
    return {costo, cantidad};
}