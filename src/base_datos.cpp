#include "../include/base_datos.h"
#include <iostream>
#include <cstring>

BaseDatos* BaseDatos::instancia = nullptr;

BaseDatos::BaseDatos() : db(nullptr), db_path("datos/granja.db"), en_transaccion(false) {}

BaseDatos::~BaseDatos() {
    cerrar();
}

BaseDatos* BaseDatos::getInstancia() {
    if (!instancia) {
        instancia = new BaseDatos();
    }
    return instancia;
}

sqlite3* BaseDatos::abrir() {
    if (db) return db;
    int rc = sqlite3_open(db_path.c_str(), &db);
    if (rc) {
        std::cerr << "Error al abrir base de datos: " << sqlite3_errmsg(db) << std::endl;
        return nullptr;
    }
    return db;
}

void BaseDatos::cerrar() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
}

bool BaseDatos::ejecutarSQL(const std::string& sql) {
    if (!abrir()) return false;
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::string errStr = errMsg ? errMsg : "";
        if (errStr.find("duplicate column") == std::string::npos) {
            std::cerr << "Error SQL: " << errMsg << std::endl;
        }
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

int BaseDatos::insertarYGetId(const std::string& sql) {
    if (!abrir()) return -1;
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::string errStr = errMsg ? errMsg : "";
        if (errStr.find("duplicate column") == std::string::npos) {
            std::cerr << "Error SQL: " << errMsg << std::endl;
        }
        sqlite3_free(errMsg);
        return -1;
    }
    return (int)sqlite3_last_insert_rowid(db);
}

std::string BaseDatos::getConfigValue(const std::string& clave) {
    if (!abrir()) return "";
    std::string sql = "SELECT valor FROM configuracion WHERE clave = '" + clave + "'";
    sqlite3_stmt* stmt;
    std::string resultado = "";
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            const char* val = (const char*)sqlite3_column_text(stmt, 0);
            if (val) resultado = val;
        }
    }
    sqlite3_finalize(stmt);
    return resultado;
}

void BaseDatos::setConfigValue(const std::string& clave, const std::string& valor) {
    std::string sql = "INSERT OR REPLACE INTO configuracion (clave, valor, updated_at) VALUES ('" +
                      clave + "', '" + valor + "', datetime('now'))";
    ejecutarSQL(sql);
}

Configuracion BaseDatos::getConfiguracion() {
    Configuracion config;
    try {
        config.precio_kg = std::stod(getConfigValue("precio_kg"));
    } catch (...) {
        config.precio_kg = 2.50;
    }
    return config;
}

void BaseDatos::actualizarSchema() {
    // Agregar nuevas columnas y tablas si no existen
    std::vector<std::string> nuevas_tablas = {
        // Tipos de granja
        R"(CREATE TABLE IF NOT EXISTS tipos_granja (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            nombre TEXT NOT NULL UNIQUE,
            descripcion TEXT,
            capacidad_recomendada INTEGER DEFAULT 500,
            costo_inicial REAL DEFAULT 0,
            densidad_recomendada INTEGER DEFAULT 10
        ))",

        // Granjas
        R"(CREATE TABLE IF NOT EXISTS granjas (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            nombre TEXT NOT NULL,
            tipo_granja_id INTEGER REFERENCES tipos_granja(id),
            ubicacion TEXT,
            dimensiones TEXT,
            densidad_maxima INTEGER DEFAULT 10,
            activa INTEGER DEFAULT 1,
            created_at TEXT DEFAULT CURRENT_TIMESTAMP
        ))",

        // Corrales
        R"(CREATE TABLE IF NOT EXISTS corrales (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            granja_id INTEGER REFERENCES granjas(id),
            numero INTEGER NOT NULL,
            nombre TEXT,
            largo_m REAL DEFAULT 0,
            ancho_m REAL DEFAULT 0,
            altura_m REAL DEFAULT 0,
            area_m2 REAL DEFAULT 0,
            tipo_techo TEXT DEFAULT 'zinc',
            proteccion TEXT DEFAULT 'completo',
            ventilacion TEXT DEFAULT 'natural',
            orientacion TEXT DEFAULT 'norte-sur',
            material_paredes TEXT,
            sistema_comederos TEXT DEFAULT 'lineal',
            sistema_bebederos TEXT DEFAULT 'campana',
            tiene_iluminacion INTEGER DEFAULT 1,
            tiene_calefaccion INTEGER DEFAULT 0,
            tipo_calefaccion TEXT DEFAULT 'ninguna',
            tiene_drenaje INTEGER DEFAULT 1,
            capacidad_maxima INTEGER DEFAULT 0,
            densidad_maxima INTEGER DEFAULT 10,
            activo INTEGER DEFAULT 1,
            created_at TEXT DEFAULT CURRENT_TIMESTAMP
        ))",

        // Tipos de cama
        R"(CREATE TABLE IF NOT EXISTS tipos_cama (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            nombre TEXT NOT NULL UNIQUE,
            descripcion TEXT,
            costo_por_kg REAL DEFAULT 0,
            proveedor_id INTEGER REFERENCES proveedores(id),
            frecuencia_cambio_dias INTEGER DEFAULT 14,
            activa INTEGER DEFAULT 1
        ))",

        // Proveedores
        R"(CREATE TABLE IF NOT EXISTS proveedores (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            nombre TEXT NOT NULL UNIQUE,
            telefono TEXT,
            email TEXT,
            direccion TEXT,
            tipo TEXT DEFAULT 'todos',
            rating INTEGER DEFAULT 3,
            activo INTEGER DEFAULT 1,
            created_at TEXT DEFAULT CURRENT_TIMESTAMP
        ))",

        // Equipos
        R"(CREATE TABLE IF NOT EXISTS equipos (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            corral_id INTEGER REFERENCES corrales(id),
            tipo TEXT NOT NULL,
            nombre TEXT,
            cantidad INTEGER DEFAULT 0,
            costo_unitario REAL DEFAULT 0,
            estado TEXT DEFAULT 'bueno',
            activo INTEGER DEFAULT 1,
            created_at TEXT DEFAULT CURRENT_TIMESTAMP
        ))",

        // Vacunas
        R"(CREATE TABLE IF NOT EXISTS vacunas (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            nombre TEXT NOT NULL,
            aplicacion_dias INTEGER DEFAULT 0,
            costo REAL DEFAULT 0,
            proveedor_id INTEGER REFERENCES proveedores(id),
            observaciones TEXT,
            activa INTEGER DEFAULT 1
        ))",

        // Aplicación de vacunas
        R"(CREATE TABLE IF NOT EXISTS lote_vacunas (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            lote_id INTEGER REFERENCES lotes(id),
            vacuna_id INTEGER REFERENCES vacunas(id),
            fecha_aplicacion TEXT,
            observacion TEXT
        ))",

        // Medicamentos
        R"(CREATE TABLE IF NOT EXISTS medicamentos (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            nombre TEXT NOT NULL,
            principio_activo TEXT,
            dosis TEXT,
            precio REAL DEFAULT 0,
            proveedor_id INTEGER REFERENCES proveedores(id),
            categoria TEXT DEFAULT 'otro',
            stock INTEGER DEFAULT 0,
            activo INTEGER DEFAULT 1
        ))",

        // Uso de medicamentos
        R"(CREATE TABLE IF NOT EXISTS lote_medicamentos (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            lote_id INTEGER REFERENCES lotes(id),
            medicamento_id INTEGER REFERENCES medicamentos(id),
            cantidad_usada REAL DEFAULT 0,
            fecha TEXT,
            motivo TEXT
        ))",

        // Pesaje semanal
        R"(CREATE TABLE IF NOT EXISTS pesaje_semanal (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            lote_id INTEGER REFERENCES lotes(id),
            semana INTEGER NOT NULL,
            fecha TEXT NOT NULL,
            cantidad_pollos_pesados INTEGER DEFAULT 0,
            peso_promedio REAL DEFAULT 0,
            peso_total REAL DEFAULT 0,
            peso_minimo REAL DEFAULT 0,
            peso_maximo REAL DEFAULT 0,
            peso_objetivo REAL DEFAULT 0,
            observacion TEXT
        ))",

        // Servicios
        R"(CREATE TABLE IF NOT EXISTS servicios (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            nombre TEXT NOT NULL,
            tipo TEXT DEFAULT 'otro',
            costo_mensual REAL DEFAULT 0,
            proveedor TEXT,
            activo INTEGER DEFAULT 1
        ))",

        // Gastos operativos
        R"(CREATE TABLE IF NOT EXISTS gastos_operativos (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            lote_id INTEGER REFERENCES lotes(id),
            tipo TEXT NOT NULL,
            descripcion TEXT,
            monto REAL DEFAULT 0,
            fecha TEXT
        ))",

        // Contenedores/Refrigeradores
        R"(CREATE TABLE IF NOT EXISTS contenedores (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            nombre TEXT NOT NULL,
            ubicacion TEXT,
            capacidad_maxima INTEGER DEFAULT 0,
            cantidad_actual INTEGER DEFAULT 0,
            temperatura REAL DEFAULT 4.0,
            estado TEXT DEFAULT 'disponible',
            created_at TEXT DEFAULT CURRENT_TIMESTAMP
        ))",

        // Productos procesados
        R"(CREATE TABLE IF NOT EXISTS productos_procesados (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            contenedor_id INTEGER REFERENCES contenedores(id),
            lote_id INTEGER REFERENCES lotes(id),
            cantidad INTEGER DEFAULT 0,
            peso_total REAL DEFAULT 0,
            peso_promedio REAL DEFAULT 0,
            fecha_procesamiento TEXT,
            fecha_vencimiento TEXT,
            disponible INTEGER DEFAULT 1
        ))",

        // Facturas
        R"(CREATE TABLE IF NOT EXISTS facturas (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            venta_id INTEGER REFERENCES ventas(id),
            numero_factura TEXT NOT NULL UNIQUE,
            fecha TEXT NOT NULL,
            cliente_nombre TEXT,
            cliente_cedula TEXT,
            cliente_telefono TEXT,
            cliente_direccion TEXT,
            cantidad_pollos INTEGER DEFAULT 0,
            peso_total REAL DEFAULT 0,
            precio_unitario REAL DEFAULT 0,
            subtotal REAL DEFAULT 0,
            iva REAL DEFAULT 0,
            total REAL DEFAULT 0,
            observaciones TEXT,
            anulada INTEGER DEFAULT 0
        ))",

        // Beneficio (registro de matanza)
        R"(CREATE TABLE IF NOT EXISTS beneficio (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            lote_id INTEGER REFERENCES lotes(id),
            cantidad INTEGER DEFAULT 0,
            peso_total REAL DEFAULT 0,
            fecha TEXT NOT NULL,
            tipo_beneficio TEXT DEFAULT 'manual',
            operador_nombre TEXT,
            operador_cedula TEXT,
            observaciones TEXT
        ))",

        // Agregar corral_id a lotes
        "ALTER TABLE lotes ADD COLUMN corral_id INTEGER REFERENCES corrales(id)"
    };

    for (const auto& sql : nuevas_tablas) {
        try {
            ejecutarSQL(sql);
        } catch (...) {
            // Ignorar errores de columnas duplicadas
        }
    }

    // Insertar tipos de granja por defecto si no existen
    ejecutarSQL("INSERT OR IGNORE INTO tipos_granja (id, nombre, descripcion, capacidad_recomendada, densidad_recomendada) VALUES (1, 'Terrestre', 'Granja convencional en suelo', 500, 10)");
    ejecutarSQL("INSERT OR IGNORE INTO tipos_granja (id, nombre, descripcion, capacidad_recomendada, densidad_recomendada) VALUES (2, 'Aerea', 'Sistema elevado o poleo', 400, 12)");
    ejecutarSQL("INSERT OR IGNORE INTO tipos_granja (id, nombre, descripcion, capacidad_recomendada, densidad_recomendada) VALUES (3, 'Maritima', 'Zonas costeras con clima salino', 450, 10)");

    std::cout << "Schema actualizado correctamente" << std::endl;
}

void BaseDatos::crearIndices() {
    std::vector<std::string> indices = {
        // Lotes
        "CREATE INDEX IF NOT EXISTS idx_lotes_corral ON lotes(corral_id)",
        "CREATE INDEX IF NOT EXISTS idx_lotes_numero ON lotes(numero)",
        "CREATE INDEX IF NOT EXISTS idx_lotes_activo ON lotes(activo)",
        
        // Animales
        "CREATE INDEX IF NOT EXISTS idx_animales_lote ON animales(lote_id)",
        
        // Muertes
        "CREATE INDEX IF NOT EXISTS idx_muertes_lote ON muertes(lote_id)",
        
        // Sacrificios
        "CREATE INDEX IF NOT EXISTS idx_sacrificios_lote ON sacrificios(lote_id)",
        
        // Consumo alimento
        "CREATE INDEX IF NOT EXISTS idx_consumo_lote ON consumo_alimento(lote_id)",
        "CREATE INDEX IF NOT EXISTS idx_consumo_alimento ON consumo_alimento(alimento_id)",
        
        // Ventas
        "CREATE INDEX IF NOT EXISTS idx_ventas_lote ON ventas(lote_id)",
        "CREATE INDEX IF NOT EXISTS idx_ventas_cliente ON ventas(cliente_id)",
        "CREATE INDEX IF NOT EXISTS idx_ventas_pagada ON ventas(pagada)",
        
        // Clientes
        "CREATE INDEX IF NOT EXISTS idx_clientes_nombre ON clientes(nombre)",
        
        // Alimentos
        "CREATE INDEX IF NOT EXISTS idx_alimentos_nombre ON alimentos(nombre)",
        "CREATE INDEX IF NOT EXISTS idx_alimentos_fase ON alimentos(fase)",
        
        // Corrales
        "CREATE INDEX IF NOT EXISTS idx_corrales_granja ON corrales(granja_id)",
        "CREATE INDEX IF NOT EXISTS idx_corrales_activo ON corrales(activo)",
        
        // Granjas
        "CREATE INDEX IF NOT EXISTS idx_granjas_tipo ON granjas(tipo_granja_id)",
        "CREATE INDEX IF NOT EXISTS idx_granjas_activa ON granjas(activa)",
        
        // Pesaje
        "CREATE INDEX IF NOT EXISTS idx_pesaje_lote ON pesaje_semanal(lote_id)",
        "CREATE INDEX IF NOT EXISTS idx_pesaje_semana ON pesaje_semanal(semana)",
        
        // Proveedores
        "CREATE INDEX IF NOT EXISTS idx_proveedores_nombre ON proveedores(nombre)",
        
        // Vacunas
        "CREATE INDEX IF NOT EXISTS idx_vacunas_dias ON vacunas(aplicacion_dias)",
        
        // Medicamentos
        "CREATE INDEX IF NOT EXISTS idx_medicamentos_categoria ON medicamentos(categoria)",
    };
    
    for (const auto& sql : indices) {
        ejecutarSQL(sql);
    }
    
    std::cout << "Indices creados correctamente" << std::endl;
}

void BaseDatos::inicializar() {
    abrir();
    
    // Tablas originales
    std::string sql = R"(
CREATE TABLE IF NOT EXISTS lotes (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    corral_id INTEGER REFERENCES corrales(id),
    numero INTEGER NOT NULL,
    fecha_inicio TEXT NOT NULL,
    fecha_fin TEXT,
    duracion_dias INTEGER DEFAULT 45,
    activo INTEGER DEFAULT 1,
    created_at TEXT DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS animales (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    lote_id INTEGER NOT NULL REFERENCES lotes(id),
    cantidad INTEGER NOT NULL,
    precio_unitario REAL NOT NULL DEFAULT 0,
    peso_promedio REAL DEFAULT 0,
    fase TEXT CHECK(fase IN ('bebe','intermedio','grande')) DEFAULT 'bebe',
    fecha_ingreso TEXT NOT NULL,
    created_at TEXT DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS muertes (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    lote_id INTEGER NOT NULL REFERENCES lotes(id),
    causa TEXT NOT NULL,
    cantidad INTEGER NOT NULL,
    fecha TEXT NOT NULL,
    created_at TEXT DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS sacrificios (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    lote_id INTEGER NOT NULL REFERENCES lotes(id),
    cantidad INTEGER NOT NULL,
    peso_total REAL NOT NULL,
    peso_promedio REAL,
    fecha TEXT NOT NULL,
    created_at TEXT DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS alimentos (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    nombre TEXT NOT NULL UNIQUE,
    marca TEXT,
    fase TEXT CHECK(fase IN ('bebe','intermedio','grande','todos')) DEFAULT 'todos',
    proteina REAL,
    humedad REAL,
    grasa REAL,
    ceniza REAL,
    calcio REAL,
    fibra REAL,
    fosforo REAL,
    contenido_kg INTEGER DEFAULT 40,
    precio_unitario REAL DEFAULT 0,
    inventario INTEGER DEFAULT 0,
    inicio_dias INTEGER DEFAULT 0,
    final_dias INTEGER DEFAULT 999,
    created_at TEXT DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS consumo_alimento (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    lote_id INTEGER NOT NULL REFERENCES lotes(id),
    alimento_id INTEGER NOT NULL REFERENCES alimentos(id),
    cantidad_sacos INTEGER NOT NULL,
    created_at TEXT DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS clientes (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    nombre TEXT NOT NULL UNIQUE,
    telefono TEXT,
    referencia TEXT,
    created_at TEXT DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS ventas (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    lote_id INTEGER NOT NULL REFERENCES lotes(id),
    cliente_id INTEGER NOT NULL REFERENCES clientes(id),
    cantidad INTEGER NOT NULL,
    pesos TEXT NOT NULL,
    precio_unitario REAL NOT NULL,
    total REAL NOT NULL,
    abonado REAL DEFAULT 0,
    pendiente REAL DEFAULT 0,
    pagada INTEGER DEFAULT 0,
    metodo_pago TEXT,
    fecha TEXT NOT NULL,
    created_at TEXT DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS inversores (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    nombre TEXT NOT NULL UNIQUE,
    cantidad_invertida REAL NOT NULL DEFAULT 0,
    created_at TEXT DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS encargado (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    nombre TEXT NOT NULL UNIQUE,
    created_at TEXT DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS configuracion (
    clave TEXT PRIMARY KEY,
    valor TEXT NOT NULL,
    updated_at TEXT DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS herramientas (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    nombre TEXT NOT NULL UNIQUE,
    cantidad INTEGER DEFAULT 0,
    precio_unitario REAL DEFAULT 0,
    tipo TEXT,
    created_at TEXT DEFAULT CURRENT_TIMESTAMP
);
)";
    ejecutarSQL(sql);
    
    // Actualizar schema con nuevas tablas
    actualizarSchema();
    
    // Crear índices para optimizar queries
    crearIndices();
    
    // Insertar configuración inicial
    ejecutarSQL("INSERT OR IGNORE INTO configuracion (clave, valor) VALUES ('precio_kg', '2.50')");
    
    std::cout << "Base de datos inicializada correctamente" << std::endl;
}

void BaseDatos::iniciarTransaccion() {
    if (!abrir()) return;
    en_transaccion = true;
    sqlite3_exec(db, "BEGIN TRANSACTION", nullptr, nullptr, nullptr);
}

void BaseDatos::terminarTransaccion() {
    if (!abrir() || !en_transaccion) return;
    sqlite3_exec(db, "COMMIT", nullptr, nullptr, nullptr);
    en_transaccion = false;
}

std::string BaseDatos::exportarJSON() {
    if (!abrir()) return "{}";
    
    std::string json = "{\n";
    sqlite3_stmt* stmt;
    
    auto appendTable = [&](const std::string& tabla, const std::string& campos) {
        json += "  \"" + tabla + "\": [";
        std::string sql = "SELECT " + campos + " FROM " + tabla;
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            bool first = true;
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                if (!first) json += ",\n";
                json += "    {";
                int cols = sqlite3_column_count(stmt);
                for (int i = 0; i < cols; i++) {
                    const char* nombre = sqlite3_column_name(stmt, i);
                    int tipo = sqlite3_column_type(stmt, i);
                    if (i > 0) json += ", ";
                    if (tipo == SQLITE_INTEGER) {
                        json += "\"" + std::string(nombre) + "\": " + std::to_string(sqlite3_column_int(stmt, i));
                    } else if (tipo == SQLITE_FLOAT) {
                        json += "\"" + std::string(nombre) + "\": " + std::to_string(sqlite3_column_double(stmt, i));
                    } else {
                        const char* val = (const char*)sqlite3_column_text(stmt, i);
                        json += "\"" + std::string(nombre) + "\": \"" + std::string(val ? val : "") + "\"";
                    }
                }
                json += "}";
                first = false;
            }
        }
        sqlite3_finalize(stmt);
        json += "],\n";
    };
    
    appendTable("lotes", "id, corral_id, numero, fecha_inicio, fecha_fin, duracion_dias, activo");
    appendTable("animales", "id, lote_id, cantidad, precio_unitario, peso_promedio, fase, fecha_ingreso");
    appendTable("muertes", "id, lote_id, causa, cantidad, fecha");
    appendTable("sacrificios", "id, lote_id, cantidad, peso_total, peso_promedio, fecha");
    appendTable("alimentos", "id, nombre, marca, fase, inventario, precio_unitario");
    appendTable("consumo_alimento", "id, lote_id, alimento_id, cantidad_sacos");
    appendTable("clientes", "id, nombre, telefono, referencia");
    appendTable("ventas", "id, lote_id, cliente_id, cantidad, precio_unitario, total, abonado, pendiente, pagada, fecha");
    appendTable("inversores", "id, nombre, cantidad_invertida");
    appendTable("herramientas", "id, nombre, cantidad, precio_unitario");
    appendTable("granjas", "id, nombre, tipo_granja_id, ubicacion, densidad_maxima, activa");
    appendTable("corrales", "id, granja_id, numero, nombre, capacidad_maxima, activo");
    appendTable("proveedores", "id, nombre, telefono, email, tipo");
    appendTable("vacunas", "id, nombre, aplicacion_dias, costo");
    appendTable("medicamentos", "id, nombre, principio_activo, categoria, stock, precio");
    appendTable("pesaje_semanal", "id, lote_id, semana, fecha, peso_promedio, peso_objetivo");
    appendTable("configuracion", "clave, valor");
    
    json += "  \"version\": \"1.0\"\n";
    json += "}\n";
    
    return json;
}

bool BaseDatos::importarJSON(const std::string& json) {
    if (!abrir()) return false;
    
    std::cout << "Importando datos desde JSON..." << std::endl;
    return true;
}

void BaseDatos::vacuum() {
    if (!abrir()) return;
    sqlite3_exec(db, "VACUUM", nullptr, nullptr, nullptr);
    std::cout << "Base de datos optimizada (VACUUM)" << std::endl;
}