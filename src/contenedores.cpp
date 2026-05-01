#include "../include/contenedores.h"
#include "../include/base_datos.h"
#include "../include/utils.h"
#include <iostream>
#include <sstream>
#include <iomanip>

int Contenedores::crear(const std::string& nombre, const std::string& ubicacion, int capacidad, double temperatura) {
    auto* db = BaseDatos::getInstancia();
    std::string sql = "INSERT INTO contenedores (nombre, ubicacion, capacidad_maxima, temperatura, estado) VALUES ('" +
        nombre + "', '" + ubicacion + "', " + std::to_string(capacidad) + ", " + std::to_string(temperatura) + ", 'disponible')";
    return db->insertarYGetId(sql);
}

Contenedor Contenedores::obtener(int id) {
    auto* db = BaseDatos::getInstancia();
    Contenedor c;
    sqlite3_stmt* stmt;
    std::string sql = "SELECT id, nombre, ubicacion, capacidad_maxima, cantidad_actual, temperatura, estado, created_at FROM contenedores WHERE id = " + std::to_string(id);
    
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            c.id = sqlite3_column_int(stmt, 0);
            const char* n = (const char*)sqlite3_column_text(stmt, 1);
            c.nombre = n ? n : "";
            const char* u = (const char*)sqlite3_column_text(stmt, 2);
            c.ubicacion = u ? u : "";
            c.capacidad_maxima = sqlite3_column_int(stmt, 3);
            c.cantidad_actual = sqlite3_column_int(stmt, 4);
            c.temperatura = sqlite3_column_double(stmt, 5);
            std::string e = (const char*)sqlite3_column_text(stmt, 6);
            c.estado = stringToEstadoContenedor(e);
            const char* ca = (const char*)sqlite3_column_text(stmt, 7);
            c.created_at = ca ? ca : "";
        }
    }
    sqlite3_finalize(stmt);
    return c;
}

std::vector<Contenedor> Contenedores::listar() {
    auto* db = BaseDatos::getInstancia();
    std::vector<Contenedor> resultado;
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db->abrir(), "SELECT id, nombre, ubicacion, capacidad_maxima, cantidad_actual, temperatura, estado, created_at FROM contenedores ORDER BY nombre", -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Contenedor c;
            c.id = sqlite3_column_int(stmt, 0);
            const char* n = (const char*)sqlite3_column_text(stmt, 1);
            c.nombre = n ? n : "";
            const char* u = (const char*)sqlite3_column_text(stmt, 2);
            c.ubicacion = u ? u : "";
            c.capacidad_maxima = sqlite3_column_int(stmt, 3);
            c.cantidad_actual = sqlite3_column_int(stmt, 4);
            c.temperatura = sqlite3_column_double(stmt, 5);
            std::string e = (const char*)sqlite3_column_text(stmt, 6);
            c.estado = stringToEstadoContenedor(e);
            const char* ca = (const char*)sqlite3_column_text(stmt, 7);
            c.created_at = ca ? ca : "";
            resultado.push_back(c);
        }
    }
    sqlite3_finalize(stmt);
    return resultado;
}

std::vector<Contenedor> Contenedores::listarDisponibles() {
    auto* db = BaseDatos::getInstancia();
    std::vector<Contenedor> resultado;
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db->abrir(), "SELECT id, nombre, ubicacion, capacidad_maxima, cantidad_actual, temperatura, estado, created_at FROM contenedores WHERE estado = 'disponible' ORDER BY nombre", -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Contenedor c;
            c.id = sqlite3_column_int(stmt, 0);
            const char* n = (const char*)sqlite3_column_text(stmt, 1);
            c.nombre = n ? n : "";
            const char* u = (const char*)sqlite3_column_text(stmt, 2);
            c.ubicacion = u ? u : "";
            c.capacidad_maxima = sqlite3_column_int(stmt, 3);
            c.cantidad_actual = sqlite3_column_int(stmt, 4);
            c.temperatura = sqlite3_column_double(stmt, 5);
            c.estado = EstadoContenedor::Disponible;
            const char* ca = (const char*)sqlite3_column_text(stmt, 7);
            c.created_at = ca ? ca : "";
            resultado.push_back(c);
        }
    }
    sqlite3_finalize(stmt);
    return resultado;
}

bool Contenedores::actualizar(int id, const std::string& nombre, const std::string& ubicacion, int capacidad, double temperatura, EstadoContenedor estado) {
    auto* db = BaseDatos::getInstancia();
    std::string sql = "UPDATE contenedores SET nombre = '" + nombre + "', ubicacion = '" + ubicacion + "', capacidad_maxima = " + std::to_string(capacidad) + ", temperatura = " + std::to_string(temperatura) + ", estado = '" + estadoContenedorToString(estado) + "' WHERE id = " + std::to_string(id);
    return db->ejecutarSQL(sql);
}

bool Contenedores::eliminar(int id) {
    auto* db = BaseDatos::getInstancia();
    db->ejecutarSQL("DELETE FROM productos_procesados WHERE contenedor_id = " + std::to_string(id));
    return db->ejecutarSQL("DELETE FROM contenedores WHERE id = " + std::to_string(id));
}

bool Contenedores::cambiarEstado(int id, EstadoContenedor estado) {
    auto* db = BaseDatos::getInstancia();
    std::string sql = "UPDATE contenedores SET estado = '" + estadoContenedorToString(estado) + "' WHERE id = " + std::to_string(id);
    return db->ejecutarSQL(sql);
}

int Contenedores::agregarProducto(int contenedor_id, int lote_id, int cantidad, double peso_total, const std::string& fecha_vencimiento) {
    auto* db = BaseDatos::getInstancia();
    std::string fecha = fechaActual();
    std::string vencimiento = fecha_vencimiento.empty() ? fecha : fecha_vencimiento;
    
    double peso_promedio = (cantidad > 0) ? (peso_total / cantidad) : 0.0;
    
    std::string sql = "INSERT INTO productos_procesados (contenedor_id, lote_id, cantidad, peso_total, peso_promedio, fecha_procesamiento, fecha_vencimiento, disponible) VALUES (" +
        std::to_string(contenedor_id) + ", " + std::to_string(lote_id) + ", " + std::to_string(cantidad) + ", " +
        std::to_string(peso_total) + ", " + std::to_string(peso_promedio) + ", '" + fecha + "', '" + vencimiento + "', 1)";
    
    int id = db->insertarYGetId(sql);
    
    db->ejecutarSQL("UPDATE contenedores SET cantidad_actual = cantidad_actual + " + std::to_string(cantidad) + ", estado = 'ocupado' WHERE id = " + std::to_string(contenedor_id));
    
    return id;
}

std::vector<ProductoProcesado> Contenedores::listarProductos(int contenedor_id) {
    auto* db = BaseDatos::getInstancia();
    std::vector<ProductoProcesado> resultado;
    sqlite3_stmt* stmt;
    std::string sql;
    
    if (contenedor_id > 0) {
        sql = "SELECT id, contenedor_id, lote_id, cantidad, peso_total, peso_promedio, fecha_procesamiento, fecha_vencimiento, disponible FROM productos_procesados WHERE contenedor_id = " + std::to_string(contenedor_id);
    } else {
        sql = "SELECT id, contenedor_id, lote_id, cantidad, peso_total, peso_promedio, fecha_procesamiento, fecha_vencimiento, disponible FROM productos_procesados ORDER BY id";
    }
    
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            ProductoProcesado p;
            p.id = sqlite3_column_int(stmt, 0);
            p.contenedor_id = sqlite3_column_int(stmt, 1);
            p.lote_id = sqlite3_column_int(stmt, 2);
            p.cantidad = sqlite3_column_int(stmt, 3);
            p.peso_total = sqlite3_column_double(stmt, 4);
            p.peso_promedio = sqlite3_column_double(stmt, 5);
            const char* fp = (const char*)sqlite3_column_text(stmt, 6);
            p.fecha_procesamiento = fp ? fp : "";
            const char* fv = (const char*)sqlite3_column_text(stmt, 7);
            p.fecha_vencimiento = fv ? fv : "";
            p.disponible = sqlite3_column_int(stmt, 8) == 1;
            resultado.push_back(p);
        }
    }
    sqlite3_finalize(stmt);
    return resultado;
}

bool Contenedores::retirerProducto(int producto_id, int cantidad) {
    auto* db = BaseDatos::getInstancia();
    
    sqlite3_stmt* stmt;
    int contenedor_id = 0;
    int cantidad_actual = 0;
    
    std::string sqlProd = "SELECT contenedor_id, cantidad FROM productos_procesados WHERE id = " + std::to_string(producto_id);
    if (sqlite3_prepare_v2(db->abrir(), sqlProd.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            contenedor_id = sqlite3_column_int(stmt, 0);
            cantidad_actual = sqlite3_column_int(stmt, 1);
        }
    }
    sqlite3_finalize(stmt);
    
    if (cantidad > cantidad_actual) cantidad = cantidad_actual;
    
    if (cantidad_actual == cantidad) {
        db->ejecutarSQL("DELETE FROM productos_procesados WHERE id = " + std::to_string(producto_id));
    } else {
        db->ejecutarSQL("UPDATE productos_procesados SET cantidad = cantidad - " + std::to_string(cantidad) + " WHERE id = " + std::to_string(producto_id));
    }
    
    db->ejecutarSQL("UPDATE contenedores SET cantidad_actual = cantidad_actual - " + std::to_string(cantidad) + " WHERE id = " + std::to_string(contenedor_id));
    
    int total = 0;
    std::string sqlTotal = "SELECT COALESCE(SUM(cantidad), 0) FROM productos_procesados WHERE contenedor_id = " + std::to_string(contenedor_id);
    if (sqlite3_prepare_v2(db->abrir(), sqlTotal.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            total = sqlite3_column_int(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);
    
    if (total == 0) {
        db->ejecutarSQL("UPDATE contenedores SET estado = 'disponible' WHERE id = " + std::to_string(contenedor_id));
    }
    
    return true;
}

bool Contenedores::eliminarProducto(int producto_id) {
    auto* db = BaseDatos::getInstancia();
    
    sqlite3_stmt* stmt;
    int contenedor_id = 0;
    int cantidad = 0;
    
    std::string sqlDel = "SELECT contenedor_id, cantidad FROM productos_procesados WHERE id = " + std::to_string(producto_id);
    if (sqlite3_prepare_v2(db->abrir(), sqlDel.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            contenedor_id = sqlite3_column_int(stmt, 0);
            cantidad = sqlite3_column_int(stmt, 1);
        }
    }
    sqlite3_finalize(stmt);
    
    db->ejecutarSQL("DELETE FROM productos_procesados WHERE id = " + std::to_string(producto_id));
    db->ejecutarSQL("UPDATE contenedores SET cantidad_actual = cantidad_actual - " + std::to_string(cantidad) + " WHERE id = " + std::to_string(contenedor_id));
    
    return true;
}

int Contenedores::getCantidadProductos(int contenedor_id) {
    auto* db = BaseDatos::getInstancia();
    sqlite3_stmt* stmt;
    int total = 0;
    std::string sqlCount = "SELECT COALESCE(SUM(cantidad), 0) FROM productos_procesados WHERE contenedor_id = " + std::to_string(contenedor_id);
    if (sqlite3_prepare_v2(db->abrir(), sqlCount.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            total = sqlite3_column_int(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);
    return total;
}