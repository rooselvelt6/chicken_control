#include "../include/facturacion.h"
#include "../include/base_datos.h"
#include "../include/ventas.h"
#include "../include/utils.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <ctime>

std::string Facturacion::generarNumeroFactura() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    std::ostringstream oss;
    oss << "FAC-" << (1900 + ltm->tm_year);
    oss << std::setw(2) << std::setfill('0') << (1 + ltm->tm_mon);
    oss << std::setw(2) << std::setfill('0') << ltm->tm_mday;
    oss << "-" << std::rand() % 10000;
    return oss.str();
}

int Facturacion::generarFactura(int venta_id, const std::string& cliente_cedula, const std::string& cliente_direccion) {
    auto* db = BaseDatos::getInstancia();
    Venta* venta = Ventas::obtenerVenta(venta_id);
    if (!venta) {
        std::cerr << "Venta no encontrada" << std::endl;
        return -1;
    }
    
    std::string numero = generarNumeroFactura();
    std::string fecha = fechaActual();
    
    double subtotal = venta->total;
    double iva = subtotal * 0.16;
    double total = subtotal + iva;
    
    std::string cedula = cliente_cedula.empty() ? "N/A" : cliente_cedula;
    std::string direccion = cliente_direccion.empty() ? "N/A" : cliente_direccion;
    
    std::string sql = "INSERT INTO facturas (venta_id, numero_factura, fecha, cliente_nombre, cliente_cedula, cliente_telefono, cliente_direccion, cantidad_pollos, peso_total, precio_unitario, subtotal, iva, total) VALUES (" +
        std::to_string(venta_id) + ", '" + numero + "', '" + fecha + "', '" +
        venta->nombre_cliente + "', '" + cedula + "', '', '" + direccion + "', " +
        std::to_string(venta->cantidad) + ", " + std::to_string(venta->total / venta->precio_unitario) + ", " +
        std::to_string(venta->precio_unitario) + ", " + std::to_string(subtotal) + ", " +
        std::to_string(iva) + ", " + std::to_string(total) + ")";
    
    return db->insertarYGetId(sql);
}

Factura Facturacion::obtenerFactura(int id) {
    auto* db = BaseDatos::getInstancia();
    Factura f;
    sqlite3_stmt* stmt;
    std::string sql = "SELECT id, venta_id, numero_factura, fecha, cliente_nombre, cliente_cedula, cliente_telefono, cliente_direccion, cantidad_pollos, peso_total, precio_unitario, subtotal, iva, total, observaciones, anulada FROM facturas WHERE id = " + std::to_string(id);
    
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            f.id = sqlite3_column_int(stmt, 0);
            f.venta_id = sqlite3_column_int(stmt, 1);
            const char* nf = (const char*)sqlite3_column_text(stmt, 2);
            f.numero_factura = nf ? nf : "";
            const char* fe = (const char*)sqlite3_column_text(stmt, 3);
            f.fecha = fe ? fe : "";
            const char* cn = (const char*)sqlite3_column_text(stmt, 4);
            f.cliente_nombre = cn ? cn : "";
            const char* cc = (const char*)sqlite3_column_text(stmt, 5);
            f.cliente_cedula = cc ? cc : "";
            const char* ct = (const char*)sqlite3_column_text(stmt, 6);
            f.cliente_telefono = ct ? ct : "";
            const char* cd = (const char*)sqlite3_column_text(stmt, 7);
            f.cliente_direccion = cd ? cd : "";
            f.cantidad_pollos = sqlite3_column_int(stmt, 8);
            f.peso_total = sqlite3_column_double(stmt, 9);
            f.precio_unitario = sqlite3_column_double(stmt, 10);
            f.subtotal = sqlite3_column_double(stmt, 11);
            f.iva = sqlite3_column_double(stmt, 12);
            f.total = sqlite3_column_double(stmt, 13);
            const char* ob = (const char*)sqlite3_column_text(stmt, 14);
            f.observaciones = ob ? ob : "";
            f.anulada = sqlite3_column_int(stmt, 15) == 1;
        }
    }
    sqlite3_finalize(stmt);
    return f;
}

std::vector<Factura> Facturacion::listarFacturas() {
    auto* db = BaseDatos::getInstancia();
    std::vector<Factura> resultado;
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db->abrir(), "SELECT id, venta_id, numero_factura, fecha, cliente_nombre, cliente_cedula, cliente_telefono, cliente_direccion, cantidad_pollos, peso_total, precio_unitario, subtotal, iva, total, observaciones, anulada FROM facturas ORDER BY id DESC", -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Factura f;
            f.id = sqlite3_column_int(stmt, 0);
            f.venta_id = sqlite3_column_int(stmt, 1);
            const char* nf = (const char*)sqlite3_column_text(stmt, 2);
            f.numero_factura = nf ? nf : "";
            const char* fe = (const char*)sqlite3_column_text(stmt, 3);
            f.fecha = fe ? fe : "";
            const char* cn = (const char*)sqlite3_column_text(stmt, 4);
            f.cliente_nombre = cn ? cn : "";
            const char* cc = (const char*)sqlite3_column_text(stmt, 5);
            f.cliente_cedula = cc ? cc : "";
            const char* ct = (const char*)sqlite3_column_text(stmt, 6);
            f.cliente_telefono = ct ? ct : "";
            const char* cd = (const char*)sqlite3_column_text(stmt, 7);
            f.cliente_direccion = cd ? cd : "";
            f.cantidad_pollos = sqlite3_column_int(stmt, 8);
            f.peso_total = sqlite3_column_double(stmt, 9);
            f.precio_unitario = sqlite3_column_double(stmt, 10);
            f.subtotal = sqlite3_column_double(stmt, 11);
            f.iva = sqlite3_column_double(stmt, 12);
            f.total = sqlite3_column_double(stmt, 13);
            const char* ob = (const char*)sqlite3_column_text(stmt, 14);
            f.observaciones = ob ? ob : "";
            f.anulada = sqlite3_column_int(stmt, 15) == 1;
            resultado.push_back(f);
        }
    }
    sqlite3_finalize(stmt);
    return resultado;
}

std::vector<Factura> Facturacion::listarFacturasPorVenta(int venta_id) {
    auto* db = BaseDatos::getInstancia();
    std::vector<Factura> resultado;
    sqlite3_stmt* stmt;
    std::string sql = "SELECT id, venta_id, numero_factura, fecha, cliente_nombre, cliente_cedula, cliente_telefono, cliente_direccion, cantidad_pollos, peso_total, precio_unitario, subtotal, iva, total, observaciones, anulada FROM facturas WHERE venta_id = " + std::to_string(venta_id);
    
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Factura f;
            f.id = sqlite3_column_int(stmt, 0);
            f.venta_id = sqlite3_column_int(stmt, 1);
            const char* nf = (const char*)sqlite3_column_text(stmt, 2);
            f.numero_factura = nf ? nf : "";
            const char* fe = (const char*)sqlite3_column_text(stmt, 3);
            f.fecha = fe ? fe : "";
            const char* cn = (const char*)sqlite3_column_text(stmt, 4);
            f.cliente_nombre = cn ? cn : "";
            const char* cc = (const char*)sqlite3_column_text(stmt, 5);
            f.cliente_cedula = cc ? cc : "";
            const char* ct = (const char*)sqlite3_column_text(stmt, 6);
            f.cliente_telefono = ct ? ct : "";
            const char* cd = (const char*)sqlite3_column_text(stmt, 7);
            f.cliente_direccion = cd ? cd : "";
            f.cantidad_pollos = sqlite3_column_int(stmt, 8);
            f.peso_total = sqlite3_column_double(stmt, 9);
            f.precio_unitario = sqlite3_column_double(stmt, 10);
            f.subtotal = sqlite3_column_double(stmt, 11);
            f.iva = sqlite3_column_double(stmt, 12);
            f.total = sqlite3_column_double(stmt, 13);
            const char* ob = (const char*)sqlite3_column_text(stmt, 14);
            f.observaciones = ob ? ob : "";
            f.anulada = sqlite3_column_int(stmt, 15) == 1;
            resultado.push_back(f);
        }
    }
    sqlite3_finalize(stmt);
    return resultado;
}

bool Facturacion::anularFactura(int id) {
    auto* db = BaseDatos::getInstancia();
    std::string sql = "UPDATE facturas SET anulada = 1 WHERE id = " + std::to_string(id);
    return db->ejecutarSQL(sql);
}

void Facturacion::mostrarFactura(int id) {
    Factura f = obtenerFactura(id);
    if (f.id == 0) {
        std::cout << "Factura no encontrada" << std::endl;
        return;
    }
    
    std::cout << "\n";
    std::cout << "========================================" << std::endl;
    std::cout << "          FACTURA DE VENTA             " << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "No. Factura: " << f.numero_factura << std::endl;
    std::cout << "Fecha: " << f.fecha << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "CLIENTE:" << std::endl;
    std::cout << "  Nombre: " << f.cliente_nombre << std::endl;
    std::cout << "  Cedula: " << f.cliente_cedula << std::endl;
    std::cout << "  Telefono: " << f.cliente_telefono << std::endl;
    std::cout << "  Direccion: " << f.cliente_direccion << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "DETALLE:" << std::endl;
    std::cout << "  Cantidad pollos: " << f.cantidad_pollos << std::endl;
    std::cout << "  Peso total: " << std::fixed << std::setprecision(2) << f.peso_total << " kg" << std::endl;
    std::cout << "  Precio unitario: USD " << f.precio_unitario << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "  Subtotal:    USD " << std::fixed << std::setprecision(2) << f.subtotal << std::endl;
    std::cout << "  IVA (16%):   USD " << std::fixed << std::setprecision(2) << f.iva << std::endl;
    std::cout << "  TOTAL:       USD " << std::fixed << std::setprecision(2) << f.total << std::endl;
    std::cout << "========================================" << std::endl;
    if (f.anulada) {
        std::cout << "           *** ANULADA ***" << std::endl;
    }
    std::cout << "\n";
}

void Facturacion::exportarFacturaPDF(int id, const std::string& filename) {
    Factura f = obtenerFactura(id);
    if (f.id == 0) {
        std::cout << "Factura no encontrada" << std::endl;
        return;
    }
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error al crear archivo" << std::endl;
        return;
    }
    
    file << "========================================\n";
    file << "          FACTURA DE VENTA             \n";
    file << "========================================\n";
    file << "No. Factura: " << f.numero_factura << "\n";
    file << "Fecha: " << f.fecha << "\n";
    file << "----------------------------------------\n";
    file << "CLIENTE:\n";
    file << "  Nombre: " << f.cliente_nombre << "\n";
    file << "  Cedula: " << f.cliente_cedula << "\n";
    file << "  Telefono: " << f.cliente_telefono << "\n";
    file << "  Direccion: " << f.cliente_direccion << "\n";
    file << "----------------------------------------\n";
    file << "DETALLE:\n";
    file << "  Cantidad pollos: " << f.cantidad_pollos << "\n";
    file << "  Peso total: " << std::fixed << std::setprecision(2) << f.peso_total << " kg\n";
    file << "  Precio unitario: USD " << f.precio_unitario << "\n";
    file << "----------------------------------------\n";
    file << "  Subtotal:    USD " << std::fixed << std::setprecision(2) << f.subtotal << "\n";
    file << "  IVA (16%):   USD " << std::fixed << std::setprecision(2) << f.iva << "\n";
    file << "  TOTAL:       USD " << std::fixed << std::setprecision(2) << f.total << "\n";
    file << "========================================\n";
    file.close();
    
    std::cout << "Factura exportada a: " << filename << std::endl;
}