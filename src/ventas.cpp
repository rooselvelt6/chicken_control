#include "../include/ventas.h"
#include "../include/base_datos.h"
#include "../include/utils.h"
#include <sstream>
#include <iostream>
#include <iomanip>

int Ventas::agregarCliente(const std::string& nombre, const std::string& telefono, const std::string& referencia) {
    auto* db = BaseDatos::getInstancia();
    std::string tel = telefono.empty() ? "NULL" : "'" + telefono + "'";
    std::string ref = referencia.empty() ? "NULL" : "'" + referencia + "'";
    std::string sql = "INSERT INTO clientes (nombre, telefono, referencia) VALUES ('" + nombre + "', " + tel + ", " + ref + ")";
    return db->insertarYGetId(sql);
}

Cliente* Ventas::obtenerCliente(int id) { return nullptr; }

Cliente* Ventas::obtenerClientePorNombre(const std::string& nombre) {
    auto* db = BaseDatos::getInstancia();
    std::string sql = "SELECT id, nombre, telefono, referencia FROM clientes WHERE nombre = '" + nombre + "'";
    sqlite3_stmt* stmt;
    static Cliente c;
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            c.id = sqlite3_column_int(stmt, 0);
            c.nombre = (const char*)sqlite3_column_text(stmt, 1);
            const char* t = (const char*)sqlite3_column_text(stmt, 2);
            c.telefono = t ? t : "";
            const char* r = (const char*)sqlite3_column_text(stmt, 3);
            c.referencia = r ? r : "";
            sqlite3_finalize(stmt);
            return &c;
        }
    }
    sqlite3_finalize(stmt);
    return nullptr;
}

std::vector<Cliente> Ventas::listarClientes() {
    auto* db = BaseDatos::getInstancia();
    std::vector<Cliente> resultado;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->abrir(), "SELECT id, nombre, telefono, referencia FROM clientes ORDER BY nombre", -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Cliente c;
            c.id = sqlite3_column_int(stmt, 0);
            c.nombre = (const char*)sqlite3_column_text(stmt, 1);
            const char* t = (const char*)sqlite3_column_text(stmt, 2);
            c.telefono = t ? t : "";
            const char* r = (const char*)sqlite3_column_text(stmt, 3);
            c.referencia = r ? r : "";
            resultado.push_back(c);
        }
    }
    sqlite3_finalize(stmt);
    return resultado;
}

int Ventas::registrarVenta(int lote_id, int cliente_id, const std::vector<double>& pesos,
                          double precio_unitario, const std::vector<std::string>& metodos_pago,
                          const std::string& fecha) {
    auto* db = BaseDatos::getInstancia();
    int cantidad = pesos.size();
    double total = cantidad * precio_unitario;
    std::string pesos_str = floatListToString(std::vector<float>(pesos.begin(), pesos.end()));
    std::string metodos_str = stringListToString(metodos_pago);
    std::string sql = "INSERT INTO ventas (lote_id, cliente_id, cantidad, pesos, precio_unitario, total, abonado, pendiente, pagada, metodo_pago, fecha) VALUES (" +
                      std::to_string(lote_id) + ", " + std::to_string(cliente_id) + ", " +
                      std::to_string(cantidad) + ", '" + pesos_str + "', " +
                      std::to_string(precio_unitario) + ", " + std::to_string(total) + ", 0, " +
                      std::to_string(total) + ", 0, '" + metodos_str + "', '" + fecha + "')";
    return db->insertarYGetId(sql);
}

Venta* Ventas::obtenerVenta(int id) { return nullptr; }

std::vector<Venta> Ventas::listarVentasPorLote(int lote_id) { return {}; }

std::vector<Venta> Ventas::listarVentasPorCliente(int cliente_id) { return {}; }

void Ventas::registrarPago(int venta_id, double monto) {
    auto* db = BaseDatos::getInstancia();
    std::string sql = "UPDATE ventas SET abonado = abonado + " + std::to_string(monto) + ", pendiente = total - abonado - " + std::to_string(monto) + ", pagada = CASE WHEN (total - abonado - " + std::to_string(monto) + ") <= 0 THEN 1 ELSE 0 END WHERE id = " + std::to_string(venta_id);
    db->ejecutarSQL(sql);
}

std::vector<Venta> Ventas::ventasPendientes() { return {}; }

double Ventas::totalDeudasPendientes() { return 0.0; }

double Ventas::totalVentas(int lote_id) { return 0.0; }

double Ventas::totalCapitalRecuperado(int lote_id) { return 0.0; }

std::pair<int, double> Ventas::totalVendidos(int lote_id) { return {0, 0.0}; }

void Ventas::imprimirVentas(const std::vector<Venta>& ventas) {}

void Ventas::cargarClientesEjemplo() {
    agregarCliente("Alfredo", "04121234567", "Felix");
    agregarCliente("Xiomara", "04141234567", "Nelo");
    agregarCliente("Julio Picha", "04121234568", "Felix");
    agregarCliente("Danilo", "04121234569", "Felix");
    agregarCliente("Frank", "04121234570", "Felix");
    agregarCliente("Lorenzo Cocina", "04121234571", "Felix");
    std::cout << "Clientes de ejemplo cargados" << std::endl;
}