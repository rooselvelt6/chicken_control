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
        config.precio_dolar = std::stod(getConfigValue("precio_dolar"));
    } catch (...) {
        config.precio_dolar = 4340.0;
    }
    try {
        config.precio_kg_ves = std::stod(getConfigValue("precio_kg_ves"));
    } catch (...) {
        config.precio_kg_ves = 15000.0;
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
    
    // Insertar configuración inicial
    ejecutarSQL("INSERT OR IGNORE INTO configuracion (clave, valor) VALUES ('precio_dolar', '4340')");
    ejecutarSQL("INSERT OR IGNORE INTO configuracion (clave, valor) VALUES ('precio_kg_ves', '15000')");
    
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