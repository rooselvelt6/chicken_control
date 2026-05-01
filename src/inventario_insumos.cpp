#include "../include/inventario_insumos.h"
#include "../include/base_datos.h"
#include "../include/utils.h"
#include <iostream>
#include <iomanip>
#include <ctime>

// Categorías válidas
static std::vector<std::string> categorias_validas = {
    "medicamento", "vacuna", "limpieza", "equipo", "alimento", "otro"
};

bool InventarioInsumos::validarCategoria(const std::string& categoria) {
    for (const auto& c : categorias_validas) {
        if (c == categoria) return true;
    }
    return false;
}

std::vector<std::string> InventarioInsumos::getCategoriasValidas() {
    return categorias_validas;
}

std::string InventarioInsumos::categoriaToString(const std::string& categoria) {
    if (categoria == "medicamento") return "Medicamento";
    if (categoria == "vacuna") return "Vacuna";
    if (categoria == "limpieza") return "Limpieza";
    if (categoria == "equipo") return "Equipo";
    if (categoria == "alimento") return "Alimento";
    return "Otro";
}

int InventarioInsumos::agregar(const std::string& nombre, const std::string& categoria,
                              int cantidad, const std::string& unidad) {
    if (!validarCategoria(categoria)) {
        std::cout << "Categoría inválida. Use: medicamento, vacuna, limpieza, equipo" << std::endl;
        return -1;
    }
    
    std::string sql = std::string("INSERT INTO insumos (nombre, categoria, cantidad, unidad) VALUES ('") +
                    sanitizarSQL(nombre) + "', '" + sanitizarSQL(categoria) + "', " + 
                    std::to_string(cantidad) + ", '" + sanitizarSQL(unidad) + "')";
    
    if (BaseDatos::getInstancia()->ejecutarSQL(sql)) {
        int id = (int)sqlite3_last_insert_rowid(BaseDatos::getInstancia()->abrir());
        std::cout << "Insumo '" << nombre << "' agregado (ID: " << id << ")" << std::endl;
        return id;
    }
    return -1;
}

bool InventarioInsumos::editar(int insumo_id, const std::string& descripcion, 
                               int stock_minimo, double precio_unitario) {
    std::string sql = std::string("UPDATE insumos SET descripcion = '") + 
                    sanitizarSQL(descripcion) + "', stock_minimo = " + std::to_string(stock_minimo) + 
                    ", precio_unitario = " + std::to_string(precio_unitario) + 
                    " WHERE id = " + std::to_string(insumo_id);
    
    if (BaseDatos::getInstancia()->ejecutarSQL(sql)) {
        std::cout << "Insumo actualizado" << std::endl;
        return true;
    }
    return false;
}

bool InventarioInsumos::consumir(int insumo_id, int cantidad, const std::string& motivo) {
    Insumo* insumo = obtenerPorId(insumo_id);
    if (!insumo) {
        std::cout << "Insumo no encontrado" << std::endl;
        return false;
    }
    
    if (insumo->cantidad < cantidad) {
        std::cout << "Stock insuficiente. Disponible: " << insumo->cantidad << std::endl;
        return false;
    }
    
    // Registrar movimiento
    registrarMovimiento(insumo_id, "salida", cantidad, motivo, -1);
    
    // Actualizar stock
    std::string sql = std::string("UPDATE insumos SET cantidad = cantidad - ") + 
                    std::to_string(cantidad) + " WHERE id = " + std::to_string(insumo_id);
    
    if (BaseDatos::getInstancia()->ejecutarSQL(sql)) {
        std::cout << "Consumidos " << cantidad << " " << insumo->unidad << " de '" 
                  << insumo->nombre << "'" << std::endl;
        return true;
    }
    return false;
}

bool InventarioInsumos::agregarStock(int insumo_id, int cantidad, double precio_unitario,
                                    const std::string& proveedor, const std::string& lote) {
    Insumo* insumo = obtenerPorId(insumo_id);
    if (!insumo) {
        std::cout << "Insumo no encontrado" << std::endl;
        return false;
    }
    
    // Registrar movimiento
    registrarMovimiento(insumo_id, "entrada", cantidad, std::string("Compra: ") + lote, -1);
    
    // Actualizar stock y datos
    std::string sql = std::string("UPDATE insumos SET cantidad = cantidad + ") + 
                    std::to_string(cantidad) + ", precio_unitario = " + 
                    std::to_string(precio_unitario) + ", proveedor = '" + 
                    sanitizarSQL(proveedor) + "', lote_compra = '" + sanitizarSQL(lote) + "'" +
                    " WHERE id = " + std::to_string(insumo_id);
    
    if (BaseDatos::getInstancia()->ejecutarSQL(sql)) {
        std::cout << "Agregados " << cantidad << " " << insumo->unidad << " a '" 
                  << insumo->nombre << "'" << std::endl;
        return true;
    }
    return false;
}

bool InventarioInsumos::desactivar(int insumo_id) {
    std::string sql = std::string("UPDATE insumos SET activo = 0 WHERE id = ") + 
                    std::to_string(insumo_id);
    if (BaseDatos::getInstancia()->ejecutarSQL(sql)) {
        std::cout << "Insumo desactivado" << std::endl;
        return true;
    }
    return false;
}

Insumo* InventarioInsumos::obtenerPorId(int insumo_id) {
    static Insumo insumo;  // Static to avoid returning local variable
    std::string sql = std::string("SELECT id, nombre, categoria, descripcion, cantidad, stock_minimo, ") +
                    "unidad, precio_unitario, fecha_caducidad, proveedor, lote_compra, activo " +
                    "FROM insumos WHERE id = " + std::to_string(insumo_id);
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(BaseDatos::getInstancia()->abrir(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return nullptr;
    }
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        insumo.id = sqlite3_column_int(stmt, 0);
        insumo.nombre = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        insumo.categoria = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        insumo.descripcion = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        insumo.cantidad = sqlite3_column_int(stmt, 4);
        insumo.stock_minimo = sqlite3_column_int(stmt, 5);
        insumo.unidad = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        insumo.precio_unitario = sqlite3_column_double(stmt, 7);
        insumo.fecha_caducidad = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
        insumo.proveedor = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9));
        insumo.lote_compra = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 10));
        insumo.activo = sqlite3_column_int(stmt, 11) > 0;
        sqlite3_finalize(stmt);
        return &insumo;
    }
    
    sqlite3_finalize(stmt);
    return nullptr;
}

std::vector<Insumo> InventarioInsumos::listar(const std::string& categoria) {
    std::vector<Insumo> insumos;
    
    std::string sql = std::string("SELECT id, nombre, categoria, descripcion, cantidad, stock_minimo, ") +
                    "unidad, precio_unitario, fecha_caducidad, proveedor, lote_compra, activo " +
                    "FROM insumos WHERE activo = 1";
    
    if (!categoria.empty()) {
        sql += std::string(" AND categoria = '") + sanitizarSQL(categoria) + "'";
    }
    sql += " ORDER BY categoria, nombre";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(BaseDatos::getInstancia()->abrir(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return insumos;
    }
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Insumo i;
        i.id = sqlite3_column_int(stmt, 0);
        i.nombre = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        i.categoria = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        i.descripcion = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        i.cantidad = sqlite3_column_int(stmt, 4);
        i.stock_minimo = sqlite3_column_int(stmt, 5);
        i.unidad = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        i.precio_unitario = sqlite3_column_double(stmt, 7);
        i.fecha_caducidad = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
        i.proveedor = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9));
        i.lote_compra = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 10));
        i.activo = sqlite3_column_int(stmt, 11) > 0;
        insumos.push_back(i);
    }
    
    sqlite3_finalize(stmt);
    return insumos;
}

std::vector<Insumo> InventarioInsumos::listarBajoStock() {
    std::vector<Insumo> insumos;
    
    std::string sql = std::string("SELECT id, nombre, categoria, descripcion, cantidad, stock_minimo, ") +
                    "unidad, precio_unitario, fecha_caducidad, proveedor, lote_compra, activo " +
                    "FROM insumos WHERE activo = 1 AND cantidad <= stock_minimo ORDER BY cantidad";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(BaseDatos::getInstancia()->abrir(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return insumos;
    }
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Insumo i;
        i.id = sqlite3_column_int(stmt, 0);
        i.nombre = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        i.categoria = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        i.descripcion = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        i.cantidad = sqlite3_column_int(stmt, 4);
        i.stock_minimo = sqlite3_column_int(stmt, 5);
        i.unidad = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        i.precio_unitario = sqlite3_column_double(stmt, 7);
        i.fecha_caducidad = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
        i.proveedor = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9));
        i.lote_compra = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 10));
        i.activo = sqlite3_column_int(stmt, 11) > 0;
        insumos.push_back(i);
    }
    
    sqlite3_finalize(stmt);
    return insumos;
}

std::vector<Insumo> InventarioInsumos::listarPorCaducar(int dias) {
    std::vector<Insumo> insumos;
    
    // Fecha límite
    time_t now = time(nullptr);
    now += dias * 24 * 3600;
    std::tm* t = std::localtime(&now);
    char fecha_lim[11];
    std::strftime(fecha_lim, sizeof(fecha_lim), "%Y-%m-%d", t);
    
    std::string sql = std::string("SELECT id, nombre, categoria, descripcion, cantidad, stock_minimo, ") +
                    "unidad, precio_unitario, fecha_caducidad, proveedor, lote_compra, activo " +
                    "FROM insumos WHERE activo = 1 AND fecha_caducidad <= '" + std::string(fecha_lim) + 
                    "' AND fecha_caducidad != '' ORDER BY fecha_caducidad";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(BaseDatos::getInstancia()->abrir(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return insumos;
    }
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Insumo i;
        i.id = sqlite3_column_int(stmt, 0);
        i.nombre = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        i.categoria = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        i.descripcion = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        i.cantidad = sqlite3_column_int(stmt, 4);
        i.stock_minimo = sqlite3_column_int(stmt, 5);
        i.unidad = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        i.precio_unitario = sqlite3_column_double(stmt, 7);
        i.fecha_caducidad = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
        i.proveedor = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9));
        i.lote_compra = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 10));
        i.activo = sqlite3_column_int(stmt, 11) > 0;
        insumos.push_back(i);
    }
    
    sqlite3_finalize(stmt);
    return insumos;
}

std::vector<Insumo> InventarioInsumos::listarCaducados() {
    std::vector<Insumo> insumos;
    
    // Fecha actual
    time_t now = time(nullptr);
    std::tm* t = std::localtime(&now);
    char fecha_actual[11];
    std::strftime(fecha_actual, sizeof(fecha_actual), "%Y-%m-%d", t);
    
    std::string sql = std::string("SELECT id, nombre, categoria, descripcion, cantidad, stock_minimo, ") +
                    "unidad, precio_unitario, fecha_caducidad, proveedor, lote_compra, activo " +
                    "FROM insumos WHERE activo = 1 AND fecha_caducidad < '" + std::string(fecha_actual) + 
                    "' AND fecha_caducidad != '' ORDER BY fecha_caducidad";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(BaseDatos::getInstancia()->abrir(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return insumos;
    }
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Insumo i;
        i.id = sqlite3_column_int(stmt, 0);
        i.nombre = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        i.categoria = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        i.descripcion = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        i.cantidad = sqlite3_column_int(stmt, 4);
        i.stock_minimo = sqlite3_column_int(stmt, 5);
        i.unidad = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        i.precio_unitario = sqlite3_column_double(stmt, 7);
        i.fecha_caducidad = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
        i.proveedor = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9));
        i.lote_compra = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 10));
        i.activo = sqlite3_column_int(stmt, 11) > 0;
        insumos.push_back(i);
    }
    
    sqlite3_finalize(stmt);
    return insumos;
}

int InventarioInsumos::registrarMovimiento(int insumo_id, const std::string& tipo,
                                         int cantidad, const std::string& motivo,
                                         int usuario_id) {
    std::string fecha = BaseDatos::getInstancia()->getConfigValue("current_date");
    if (fecha.empty()) {
        time_t now = time(nullptr);
        std::tm* t = std::localtime(&now);
        char fecha_buf[11];
        std::strftime(fecha_buf, sizeof(fecha_buf), "%Y-%m-%d", t);
        fecha = std::string(fecha_buf);
    }
    
    std::string sql = std::string("INSERT INTO movimientos_insumos (insumo_id, tipo, cantidad, motivo, fecha, usuario_id) ") +
                    "VALUES (" + std::to_string(insumo_id) + ", '" + sanitizarSQL(tipo) + "', " + 
                    std::to_string(cantidad) + ", '" + sanitizarSQL(motivo) + "', '" + fecha + "', " + 
                    std::to_string(usuario_id) + ")";
    
    if (BaseDatos::getInstancia()->ejecutarSQL(sql)) {
        return (int)sqlite3_last_insert_rowid(BaseDatos::getInstancia()->abrir());
    }
    return -1;
}

std::vector<MovimientoInsumo> InventarioInsumos::historialMovimientos(int insumo_id) {
    std::vector<MovimientoInsumo> movimientos;
    
    std::string sql = std::string("SELECT id, insumo_id, tipo, cantidad, motivo, fecha, usuario_id, observaciones ") +
                    "FROM movimientos_insumos";
    
    if (insumo_id > 0) {
        sql += " WHERE insumo_id = " + std::to_string(insumo_id);
    }
    sql += " ORDER BY fecha DESC, id DESC LIMIT 100";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(BaseDatos::getInstancia()->abrir(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return movimientos;
    }
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        MovimientoInsumo m;
        m.id = sqlite3_column_int(stmt, 0);
        m.insumo_id = sqlite3_column_int(stmt, 1);
        m.tipo = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        m.cantidad = sqlite3_column_int(stmt, 3);
        m.motivo = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        m.fecha = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        m.usuario_id = sqlite3_column_int(stmt, 6);
        m.observaciones = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        movimientos.push_back(m);
    }
    
    sqlite3_finalize(stmt);
    return movimientos;
}

void InventarioInsumos::reporteGeneral() {
    std::cout << "\n==========================================" << std::endl;
    std::cout << "         INVENTARIO DE INSUMOS" << std::endl;
    std::cout << "==========================================" << std::endl;
    
    auto insumos = listar();
    if (insumos.empty()) {
        std::cout << "No hay insumos registrados" << std::endl;
        return;
    }
    
    std::string categoria_actual = "";
    double valor_total = 0.0;
    
    for (const auto& i : insumos) {
        if (categoria_actual != i.categoria) {
            if (!categoria_actual.empty()) std::cout << std::endl;
            categoria_actual = i.categoria;
            std::cout << "\n--- " << categoriaToString(categoria_actual) << " ---" << std::endl;
        }
        
        std::cout << "  [" << i.id << "] " << i.nombre << std::endl;
        std::cout << "      Stock: " << i.cantidad << " " << i.unidad;
        if (i.cantidad <= i.stock_minimo) {
            std::cout << " ⚠️  BAJO STOCK";
        }
        std::cout << std::endl;
        std::cout << "      Valor: USD " << std::fixed << std::setprecision(2) 
                  << (i.cantidad * i.precio_unitario) << std::endl;
        
        valor_total += i.cantidad * i.precio_unitario;
    }
    
    std::cout << "\n==========================================" << std::endl;
    std::cout << "VALOR TOTAL INVENTARIO: USD " << std::fixed << std::setprecision(2) 
              << valor_total << std::endl;
}

void InventarioInsumos::reporteBajoStock() {
    std::cout << "\n==========================================" << std::endl;
    std::cout << "       ALERTA: BAJO STOCK" << std::endl;
    std::cout << "==========================================" << std::endl;
    
    auto insumos = listarBajoStock();
    if (insumos.empty()) {
        std::cout << "✅ Todos los insumos tienen stock suficiente" << std::endl;
        return;
    }
    
    for (const auto& i : insumos) {
        std::cout << "⚠️  [" << i.id << "] " << i.nombre << " - Stock: " 
                  << i.cantidad << "/" << i.stock_minimo << " " << i.unidad << std::endl;
    }
}

void InventarioInsumos::reporteCaducidad() {
    std::cout << "\n==========================================" << std::endl;
    std::cout << "      ALERTA: CADUCIDAD" << std::endl;
    std::cout << "==========================================" << std::endl;
    
    auto por_caducar = listarPorCaducar(30);
    if (!por_caducar.empty()) {
        std::cout << "\nPor caducar (30 días):" << std::endl;
        for (const auto& i : por_caducar) {
            std::cout << "  ⚠️  " << i.nombre << " - Caduca: " << i.fecha_caducidad << std::endl;
        }
    }
    
    auto caducados = listarCaducados();
    if (!caducados.empty()) {
        std::cout << "\n¡CADUCADOS!" << std::endl;
        for (const auto& i : caducados) {
            std::cout << "  ❌ " << i.nombre << " - Caducó: " << i.fecha_caducidad << std::endl;
        }
    }
    
    if (por_caducar.empty() && caducados.empty()) {
        std::cout << "✅ No hay alertas de caducidad" << std::endl;
    }
}

void InventarioInsumos::reporteMovimientos(const std::string& fecha_ini, 
                                         const std::string& fecha_fin) {
    std::cout << "\n==========================================" << std::endl;
    std::cout << "      MOVIMIENTOS DE INSUMOS" << std::endl;
    std::cout << "==========================================" << std::endl;
    
    std::string sql = std::string("SELECT m.id, m.insumo_id, i.nombre, m.tipo, m.cantidad, m.motivo, m.fecha ") +
                    "FROM movimientos_insumos m " +
                    "JOIN insumos i ON m.insumo_id = i.id " +
                    "WHERE m.fecha BETWEEN '" + sanitizarSQL(fecha_ini) + "' AND '" + sanitizarSQL(fecha_fin) + "' " +
                    "ORDER BY m.fecha DESC LIMIT 100";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(BaseDatos::getInstancia()->abrir(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return;
    }
    
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        std::string nombre = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string tipo = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        int cantidad = sqlite3_column_int(stmt, 4);
        std::string motivo = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        std::string fecha = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        
        std::cout << "[" << id << "] " << fecha << " - " << nombre << " - " 
                  << tipo << " " << cantidad << " (" << motivo << ")" << std::endl;
        count++;
    }
    
    sqlite3_finalize(stmt);
    
    if (count == 0) {
        std::cout << "No hay movimientos en el periodo" << std::endl;
    }
}

double InventarioInsumos::valorTotalInventario() {
    std::string sql = std::string("SELECT SUM(cantidad * precio_unitario) FROM insumos WHERE activo = 1");
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(BaseDatos::getInstancia()->abrir(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return 0.0;
    }
    
    double total = 0.0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        total = sqlite3_column_double(stmt, 0);
    }
    
    sqlite3_finalize(stmt);
    return total;
}

std::vector<std::string> InventarioInsumos::generarAlertas() {
    std::vector<std::string> alertas;
    
    auto bajo_stock = listarBajoStock();
    for (const auto& i : bajo_stock) {
        alertas.push_back("⚠️  BAJO STOCK: " + i.nombre + " (Stock: " + 
                         std::to_string(i.cantidad) + ", Mín: " + 
                         std::to_string(i.stock_minimo) + ")");
    }
    
    auto caducados = listarCaducados();
    for (const auto& i : caducados) {
        alertas.push_back("❌ CADUCADO: " + i.nombre + " (Caducó: " + i.fecha_caducidad + ")");
    }
    
    auto por_caducar = listarPorCaducar(30);
    for (const auto& i : por_caducar) {
        alertas.push_back("⚠️  POR CADUCAR: " + i.nombre + " (Caduca: " + i.fecha_caducidad + ")");
    }
    
    return alertas;
}

bool InventarioInsumos::verificarStockBajo(int insumo_id) {
    Insumo* i = obtenerPorId(insumo_id);
    if (!i) return false;
    return i->cantidad <= i->stock_minimo;
}

bool InventarioInsumos::verificarCaducidad(int insumo_id, int dias) {
    Insumo* i = obtenerPorId(insumo_id);
    if (!i || i->fecha_caducidad.empty()) return false;
    
    time_t now = time(nullptr);
    std::tm tm_fecha{};
    std::istringstream ss(i->fecha_caducidad);
    ss >> std::get_time(&tm_fecha, "%Y-%m-%d");
    time_t fecha_cad = std::mktime(&tm_fecha);
    
    double dias_restantes = difftime(fecha_cad, now) / (24 * 3600);
    return dias_restantes <= dias;
}
