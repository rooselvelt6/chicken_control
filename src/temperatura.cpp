#include "../include/temperatura.h"
#include "../include/base_datos.h"
#include "../include/utils.h"
#include <iostream>
#include <sstream>
#include <iomanip>

int Temperatura::registrar(int corral_id, double temperatura, double humedad, const std::string& observaciones) {
    auto* db = BaseDatos::getInstancia();
    std::string fecha = fechaActual();
    std::string obs = observaciones.empty() ? "NULL" : "'" + observaciones + "'";
    std::string sql = "INSERT INTO temperatura_registros (corral_id, temperatura, humedad, fecha, observaciones) VALUES (" +
        std::to_string(corral_id) + ", " + std::to_string(temperatura) + ", " + std::to_string(humedad) + ", '" + fecha + "', " + obs + ")";
    return db->insertarYGetId(sql);
}

std::vector<RegistroTemperatura> Temperatura::listar(int corral_id) {
    auto* db = BaseDatos::getInstancia();
    std::vector<RegistroTemperatura> resultado;
    sqlite3_stmt* stmt;
    std::string sql;
    if (corral_id > 0) {
        sql = "SELECT id, corral_id, temperatura, humedad, fecha, observaciones FROM temperatura_registros WHERE corral_id = " + std::to_string(corral_id) + " ORDER BY fecha DESC";
    } else {
        sql = "SELECT id, corral_id, temperatura, humedad, fecha, observaciones FROM temperatura_registros ORDER BY fecha DESC";
    }
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            RegistroTemperatura r;
            r.id = sqlite3_column_int(stmt, 0);
            r.corral_id = sqlite3_column_int(stmt, 1);
            r.temperatura = sqlite3_column_double(stmt, 2);
            r.humedad = sqlite3_column_double(stmt, 3);
            const char* f = (const char*)sqlite3_column_text(stmt, 4);
            r.fecha = f ? f : "";
            const char* o = (const char*)sqlite3_column_text(stmt, 5);
            r.observaciones = o ? o : "";
            resultado.push_back(r);
        }
    }
    sqlite3_finalize(stmt);
    return resultado;
}

std::vector<RegistroTemperatura> Temperatura::listarPorFecha(const std::string& fecha) {
    auto* db = BaseDatos::getInstancia();
    std::vector<RegistroTemperatura> resultado;
    sqlite3_stmt* stmt;
    std::string sql = "SELECT id, corral_id, temperatura, humedad, fecha, observaciones FROM temperatura_registros WHERE fecha = '" + fecha + "'";
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            RegistroTemperatura r;
            r.id = sqlite3_column_int(stmt, 0);
            r.corral_id = sqlite3_column_int(stmt, 1);
            r.temperatura = sqlite3_column_double(stmt, 2);
            r.humedad = sqlite3_column_double(stmt, 3);
            const char* f = (const char*)sqlite3_column_text(stmt, 4);
            r.fecha = f ? f : "";
            const char* o = (const char*)sqlite3_column_text(stmt, 5);
            r.observaciones = o ? o : "";
            resultado.push_back(r);
        }
    }
    sqlite3_finalize(stmt);
    return resultado;
}

std::vector<RegistroTemperatura> Temperatura::listarRango(int corral_id, const std::string& fecha_inicio, const std::string& fecha_fin) {
    auto* db = BaseDatos::getInstancia();
    std::vector<RegistroTemperatura> resultado;
    sqlite3_stmt* stmt;
    std::string sql = "SELECT id, corral_id, temperatura, humedad, fecha, observaciones FROM temperatura_registros WHERE corral_id = " + std::to_string(corral_id) + " AND fecha BETWEEN '" + fecha_inicio + "' AND '" + fecha_fin + "' ORDER BY fecha";
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            RegistroTemperatura r;
            r.id = sqlite3_column_int(stmt, 0);
            r.corral_id = sqlite3_column_int(stmt, 1);
            r.temperatura = sqlite3_column_double(stmt, 2);
            r.humedad = sqlite3_column_double(stmt, 3);
            const char* f = (const char*)sqlite3_column_text(stmt, 4);
            r.fecha = f ? f : "";
            const char* o = (const char*)sqlite3_column_text(stmt, 5);
            r.observaciones = o ? o : "";
            resultado.push_back(r);
        }
    }
    sqlite3_finalize(stmt);
    return resultado;
}

double Temperatura::getTemperaturaPromedio(int corral_id, const std::string& fecha) {
    auto* db = BaseDatos::getInstancia();
    sqlite3_stmt* stmt;
    double temp = 0;
    std::string sql = "SELECT COALESCE(AVG(temperatura), 0) FROM temperatura_registros WHERE corral_id = " + std::to_string(corral_id) + " AND fecha = '" + fecha + "'";
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            temp = sqlite3_column_double(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);
    return temp;
}

double Temperatura::getTemperaturaMinima(int corral_id) {
    auto* db = BaseDatos::getInstancia();
    sqlite3_stmt* stmt;
    double temp = 0;
    std::string sql = "SELECT COALESCE(MIN(temperatura), 0) FROM temperatura_registros WHERE corral_id = " + std::to_string(corral_id);
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            temp = sqlite3_column_double(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);
    return temp;
}

double Temperatura::getTemperaturaMaxima(int corral_id) {
    auto* db = BaseDatos::getInstancia();
    sqlite3_stmt* stmt;
    double temp = 0;
    std::string sql = "SELECT COALESCE(MAX(temperatura), 0) FROM temperatura_registros WHERE corral_id = " + std::to_string(corral_id);
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            temp = sqlite3_column_double(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);
    return temp;
}

bool Temperatura::verificarAlertas(int corral_id, double temp_min, double temp_max) {
    double temp_actual = getTemperaturaPromedio(corral_id, fechaActual());
    return (temp_actual < temp_min || temp_actual > temp_max);
}

void Temperatura::mostrarHistorico(int corral_id, int dias) {
    auto registros = listar(corral_id);
    if (registros.empty()) {
        std::cout << "No hay registros de temperatura." << std::endl;
        return;
    }
    
    std::cout << "\n╔══════════════════════════════════════════════════════════════════╗";
    std::cout << "\n║              HISTORICO DE TEMPERATURA - CORRAL " << corral_id << "             ║";
    std::cout << "\n╠════════╦═══════════════╦═══════════╦═══════════════════════════════╣";
    std::cout << "\n║  Fecha ║ Temperatura   ║ Humedad  ║ Estado                        ║";
    std::cout << "\n╠════════╬═══════════════╬═══════════╬═══════════════════════════════╣";
    
    for (size_t i = 0; i < std::min((size_t)dias, registros.size()); i++) {
        const auto& r = registros[i];
        std::string estado;
        if (r.temperatura < 18) estado = "FRRIO";
        else if (r.temperatura > 28) estado = "CALOR";
        else estado = "OK";
        
        std::cout << "\n║ " << std::setw(8) << r.fecha;
        std::cout << " ║ " << std::fixed << std::setprecision(1) << std::setw(9) << r.temperatura << "C";
        std::cout << " ║ " << std::fixed << std::setprecision(1) << std::setw(7) << r.humedad << "%";
        std::cout << " ║ " << std::setw(16) << estado << "    ║";
    }
    std::cout << "\n╚════════╩═══════════════╩═══════════╩═══════════════════════════════╝\n";
}