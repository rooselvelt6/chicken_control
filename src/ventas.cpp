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

std::vector<Venta> Ventas::listar() {
    auto* db = BaseDatos::getInstancia();
    std::vector<Venta> resultado;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->abrir(), 
        "SELECT v.id, v.lote_id, v.cliente_id, c.nombre, v.cantidad, v.pesos, v.precio_unitario, v.total, v.abonado, v.pendiente, v.pagada, v.metodo_pago, v.fecha "
        "FROM ventas v LEFT JOIN clientes c ON v.cliente_id = c.id ORDER BY v.id", -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Venta v;
            v.id = sqlite3_column_int(stmt, 0);
            v.lote_id = sqlite3_column_int(stmt, 1);
            v.cliente_id = sqlite3_column_int(stmt, 2);
            const char* cn = (const char*)sqlite3_column_text(stmt, 3);
            v.nombre_cliente = cn ? cn : "";
            v.cantidad = sqlite3_column_int(stmt, 4);
            std::string pesos_str = (const char*)sqlite3_column_text(stmt, 5);
            v.pesos = std::vector<double>(pesos_str.begin(), pesos_str.end());
            v.precio_unitario = sqlite3_column_double(stmt, 6);
            v.total = sqlite3_column_double(stmt, 7);
            v.abonado = sqlite3_column_double(stmt, 8);
            v.pendiente = sqlite3_column_double(stmt, 9);
            v.pagada = sqlite3_column_int(stmt, 10) == 1;
            const char* mp = (const char*)sqlite3_column_text(stmt, 11);
            std::string metodos = mp ? mp : "efectivo";
            v.metodos_pago = stringToStringList(metodos);
            const char* f = (const char*)sqlite3_column_text(stmt, 12);
            v.fecha = f ? f : "";
            resultado.push_back(v);
        }
    }
    sqlite3_finalize(stmt);
    return resultado;
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

Venta* Ventas::obtenerVenta(int id) {
    auto* db = BaseDatos::getInstancia();
    static Venta v;
    sqlite3_stmt* stmt;
    std::string sql = "SELECT v.id, v.lote_id, v.cliente_id, c.nombre, v.cantidad, v.pesos, v.precio_unitario, v.total, v.abonado, v.pendiente, v.pagada, v.metodo_pago, v.fecha "
                      "FROM ventas v LEFT JOIN clientes c ON v.cliente_id = c.id WHERE v.id = " + std::to_string(id);
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            v.id = sqlite3_column_int(stmt, 0);
            v.lote_id = sqlite3_column_int(stmt, 1);
            v.cliente_id = sqlite3_column_int(stmt, 2);
            const char* cn = (const char*)sqlite3_column_text(stmt, 3);
            v.nombre_cliente = cn ? cn : "";
            v.cantidad = sqlite3_column_int(stmt, 4);
            std::string pesos_str = (const char*)sqlite3_column_text(stmt, 5);
            v.pesos = std::vector<double>(pesos_str.begin(), pesos_str.end());
            v.precio_unitario = sqlite3_column_double(stmt, 6);
            v.total = sqlite3_column_double(stmt, 7);
            v.abonado = sqlite3_column_double(stmt, 8);
            v.pendiente = sqlite3_column_double(stmt, 9);
            v.pagada = sqlite3_column_int(stmt, 10) == 1;
            const char* mp = (const char*)sqlite3_column_text(stmt, 11);
            v.metodos_pago = stringToStringList(mp ? mp : "efectivo");
            const char* f = (const char*)sqlite3_column_text(stmt, 12);
            v.fecha = f ? f : "";
            sqlite3_finalize(stmt);
            return &v;
        }
    }
    sqlite3_finalize(stmt);
    return nullptr;
}

std::vector<Venta> Ventas::listarVentasPorLote(int lote_id) {
    auto* db = BaseDatos::getInstancia();
    std::vector<Venta> resultado;
    sqlite3_stmt* stmt;
    std::string sql = "SELECT v.id, v.lote_id, v.cliente_id, c.nombre, v.cantidad, v.pesos, v.precio_unitario, v.total, v.abonado, v.pendiente, v.pagada, v.metodo_pago, v.fecha "
                      "FROM ventas v LEFT JOIN clientes c ON v.cliente_id = c.id WHERE v.lote_id = " + std::to_string(lote_id);
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Venta v;
            v.id = sqlite3_column_int(stmt, 0);
            v.lote_id = sqlite3_column_int(stmt, 1);
            v.cliente_id = sqlite3_column_int(stmt, 2);
            const char* cn = (const char*)sqlite3_column_text(stmt, 3);
            v.nombre_cliente = cn ? cn : "";
            v.cantidad = sqlite3_column_int(stmt, 4);
            std::string pesos_str = (const char*)sqlite3_column_text(stmt, 5);
            v.pesos = std::vector<double>(pesos_str.begin(), pesos_str.end());
            v.precio_unitario = sqlite3_column_double(stmt, 6);
            v.total = sqlite3_column_double(stmt, 7);
            v.abonado = sqlite3_column_double(stmt, 8);
            v.pendiente = sqlite3_column_double(stmt, 9);
            v.pagada = sqlite3_column_int(stmt, 10) == 1;
            const char* mp = (const char*)sqlite3_column_text(stmt, 11);
            v.metodos_pago = stringToStringList(mp ? mp : "efectivo");
            const char* f = (const char*)sqlite3_column_text(stmt, 12);
            v.fecha = f ? f : "";
            resultado.push_back(v);
        }
    }
    sqlite3_finalize(stmt);
    return resultado;
}

std::vector<Venta> Ventas::listarVentasPorCliente(int cliente_id) {
    auto* db = BaseDatos::getInstancia();
    std::vector<Venta> resultado;
    sqlite3_stmt* stmt;
    std::string sql = "SELECT v.id, v.lote_id, v.cliente_id, c.nombre, v.cantidad, v.pesos, v.precio_unitario, v.total, v.abonado, v.pendiente, v.pagada, v.metodo_pago, v.fecha "
                      "FROM ventas v LEFT JOIN clientes c ON v.cliente_id = c.id WHERE v.cliente_id = " + std::to_string(cliente_id);
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Venta v;
            v.id = sqlite3_column_int(stmt, 0);
            v.lote_id = sqlite3_column_int(stmt, 1);
            v.cliente_id = sqlite3_column_int(stmt, 2);
            const char* cn = (const char*)sqlite3_column_text(stmt, 3);
            v.nombre_cliente = cn ? cn : "";
            v.cantidad = sqlite3_column_int(stmt, 4);
            std::string pesos_str = (const char*)sqlite3_column_text(stmt, 5);
            v.pesos = std::vector<double>(pesos_str.begin(), pesos_str.end());
            v.precio_unitario = sqlite3_column_double(stmt, 6);
            v.total = sqlite3_column_double(stmt, 7);
            v.abonado = sqlite3_column_double(stmt, 8);
            v.pendiente = sqlite3_column_double(stmt, 9);
            v.pagada = sqlite3_column_int(stmt, 10) == 1;
            const char* mp = (const char*)sqlite3_column_text(stmt, 11);
            v.metodos_pago = stringToStringList(mp ? mp : "efectivo");
            const char* f = (const char*)sqlite3_column_text(stmt, 12);
            v.fecha = f ? f : "";
            resultado.push_back(v);
        }
    }
    sqlite3_finalize(stmt);
    return resultado;
}

void Ventas::registrarPago(int venta_id, double monto) {
    auto* db = BaseDatos::getInstancia();
    std::string sql = "UPDATE ventas SET abonado = abonado + " + std::to_string(monto) + ", pendiente = total - abonado - " + std::to_string(monto) + ", pagada = CASE WHEN (total - abonado - " + std::to_string(monto) + ") <= 0 THEN 1 ELSE 0 END WHERE id = " + std::to_string(venta_id);
    db->ejecutarSQL(sql);
}

std::vector<Venta> Ventas::ventasPendientes() {
    auto* db = BaseDatos::getInstancia();
    std::vector<Venta> resultado;
    sqlite3_stmt* stmt;
    std::string sql = "SELECT v.id, v.lote_id, v.cliente_id, c.nombre, v.cantidad, v.pesos, v.precio_unitario, v.total, v.abonado, v.pendiente, v.pagada, v.metodo_pago, v.fecha "
                      "FROM ventas v LEFT JOIN clientes c ON v.cliente_id = c.id WHERE v.pagada = 0 ORDER BY v.fecha";
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Venta v;
            v.id = sqlite3_column_int(stmt, 0);
            v.lote_id = sqlite3_column_int(stmt, 1);
            v.cliente_id = sqlite3_column_int(stmt, 2);
            const char* cn = (const char*)sqlite3_column_text(stmt, 3);
            v.nombre_cliente = cn ? cn : "";
            v.cantidad = sqlite3_column_int(stmt, 4);
            std::string pesos_str = (const char*)sqlite3_column_text(stmt, 5);
            v.pesos = std::vector<double>(pesos_str.begin(), pesos_str.end());
            v.precio_unitario = sqlite3_column_double(stmt, 6);
            v.total = sqlite3_column_double(stmt, 7);
            v.abonado = sqlite3_column_double(stmt, 8);
            v.pendiente = sqlite3_column_double(stmt, 9);
            v.pagada = sqlite3_column_int(stmt, 10) == 1;
            const char* mp = (const char*)sqlite3_column_text(stmt, 11);
            v.metodos_pago = stringToStringList(mp ? mp : "efectivo");
            const char* f = (const char*)sqlite3_column_text(stmt, 12);
            v.fecha = f ? f : "";
            resultado.push_back(v);
        }
    }
    sqlite3_finalize(stmt);
    return resultado;
}

double Ventas::totalDeudasPendientes() {
    auto* db = BaseDatos::getInstancia();
    sqlite3_stmt* stmt;
    double total = 0.0;
    if (sqlite3_prepare_v2(db->abrir(), "SELECT COALESCE(SUM(pendiente), 0) FROM ventas WHERE pagada = 0", -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            total = sqlite3_column_double(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);
    return total;
}

double Ventas::totalVentas(int lote_id) {
    auto* db = BaseDatos::getInstancia();
    sqlite3_stmt* stmt;
    double total = 0.0;
    std::string sql = "SELECT COALESCE(SUM(total), 0) FROM ventas WHERE lote_id = " + std::to_string(lote_id);
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            total = sqlite3_column_double(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);
    return total;
}

double Ventas::totalCapitalRecuperado(int lote_id) {
    auto* db = BaseDatos::getInstancia();
    sqlite3_stmt* stmt;
    double total = 0.0;
    std::string sql = "SELECT COALESCE(SUM(abonado), 0) FROM ventas WHERE lote_id = " + std::to_string(lote_id);
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            total = sqlite3_column_double(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);
    return total;
}

std::pair<int, double> Ventas::totalVendidos(int lote_id) {
    auto* db = BaseDatos::getInstancia();
    sqlite3_stmt* stmt;
    int cantidad = 0;
    double peso = 0.0;
    std::string sql = "SELECT COALESCE(SUM(cantidad), 0), COALESCE(SUM(peso_total), 0) FROM sacrificios WHERE lote_id = " + std::to_string(lote_id);
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            cantidad = sqlite3_column_int(stmt, 0);
            peso = sqlite3_column_double(stmt, 1);
        }
    }
    sqlite3_finalize(stmt);
    return {cantidad, peso};
}

void Ventas::imprimirVentas(const std::vector<Venta>& ventas) {
    if (ventas.empty()) {
        std::cout << "No hay ventas para mostrar" << std::endl;
        return;
    }
    std::cout << "\n===== LISTA DE VENTAS =====" << std::endl;
    for (const auto& v : ventas) {
        std::cout << "ID: " << v.id << " | Cliente: " << v.nombre_cliente 
                  << " | Cant: " << v.cantidad << " | Total: " << std::fixed << std::setprecision(2) << v.total
                  << " | Estado: " << (v.pagada ? "Pagada" : "Pendiente") << std::endl;
    }
}

void Ventas::cargarClientesEjemplo() {
    std::cout << "Clientes de ejemplo cargados" << std::endl;
}