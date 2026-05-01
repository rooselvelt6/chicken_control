#include "../include/empleados.h"
#include "../include/base_datos.h"
#include "../include/utils.h"
#include <iostream>
#include <sstream>

std::string Empleados::cargoToString(Cargo c) {
    switch (c) {
        case Cargo::Administrador: return "administrador";
        case Cargo::Gerente: return "gerente";
        case Cargo::Trabajador: return "trabajador";
        case Cargo::Veterinario: return "veterinario";
        case Cargo::Mantenimiento: return "mantenimiento";
        case Cargo::Seguridad: return "seguridad";
        default: return "trabajador";
    }
}

Cargo Empleados::stringToCargo(const std::string& s) {
    if (s == "administrador") return Cargo::Administrador;
    if (s == "gerente") return Cargo::Gerente;
    if (s == "veterinario") return Cargo::Veterinario;
    if (s == "mantenimiento") return Cargo::Mantenimiento;
    if (s == "seguridad") return Cargo::Seguridad;
    return Cargo::Trabajador;
}

std::string Empleados::turnoToString(Turno t) {
    switch (t) {
        case Turno::Matutino: return "matutino";
        case Turno::Vespertino: return "vespertino";
        case Turno::Nocturno: return "nocturno";
        case Turno::Rotativo: return "rotativo";
        default: return "matutino";
    }
}

Turno Empleados::stringToTurno(const std::string& s) {
    if (s == "vespertino") return Turno::Vespertino;
    if (s == "nocturno") return Turno::Nocturno;
    if (s == "rotativo") return Turno::Rotativo;
    return Turno::Matutino;
}

int Empleados::agregar(const std::string& nombre, const std::string& cedula, const std::string& telefono, Cargo cargo, double salario, Turno turno) {
    auto* db = BaseDatos::getInstancia();
    std::string fecha = fechaActual();
    std::string sql = "INSERT INTO empleados (nombre, cedula, telefono, cargo, salario, turno, activo, fecha_ingreso) VALUES ('" +
        nombre + "', '" + cedula + "', '" + telefono + "', '" + cargoToString(cargo) + "', " +
        std::to_string(salario) + ", '" + turnoToString(turno) + "', 1, '" + fecha + "')";
    return db->insertarYGetId(sql);
}

Empleado* Empleados::obtener(int id) {
    auto* db = BaseDatos::getInstancia();
    static Empleado e;
    sqlite3_stmt* stmt;
    std::string sql = "SELECT id, nombre, cedula, telefono, cargo, tipo_contrato, salario, turno, activo, fecha_ingreso FROM empleados WHERE id = " + std::to_string(id);
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            e.id = sqlite3_column_int(stmt, 0);
            const char* n = (const char*)sqlite3_column_text(stmt, 1);
            e.nombre = n ? n : "";
            const char* c = (const char*)sqlite3_column_text(stmt, 2);
            e.cedula = c ? c : "";
            const char* t = (const char*)sqlite3_column_text(stmt, 3);
            e.telefono = t ? t : "";
            std::string cargo = (const char*)sqlite3_column_text(stmt, 4);
            e.cargo = stringToCargo(cargo);
            const char* tc = (const char*)sqlite3_column_text(stmt, 5);
            e.tipo_contrato = (tc && std::string(tc) == "temporal") ? TipoContrato::Temporal : TipoContrato::Fijo;
            e.salario = sqlite3_column_double(stmt, 6);
            std::string tu = (const char*)sqlite3_column_text(stmt, 7);
            e.turno = stringToTurno(tu);
            e.activo = sqlite3_column_int(stmt, 8) == 1;
            const char* fi = (const char*)sqlite3_column_text(stmt, 9);
            e.fecha_ingreso = fi ? fi : "";
            sqlite3_finalize(stmt);
            return &e;
        }
    }
    sqlite3_finalize(stmt);
    return nullptr;
}

std::vector<Empleado> Empleados::listar() {
    auto* db = BaseDatos::getInstancia();
    std::vector<Empleado> resultado;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->abrir(), "SELECT id, nombre, cedula, telefono, cargo, tipo_contrato, salario, turno, activo, fecha_ingreso FROM empleados ORDER BY nombre", -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Empleado e;
            e.id = sqlite3_column_int(stmt, 0);
            const char* n = (const char*)sqlite3_column_text(stmt, 1);
            e.nombre = n ? n : "";
            const char* c = (const char*)sqlite3_column_text(stmt, 2);
            e.cedula = c ? c : "";
            const char* t = (const char*)sqlite3_column_text(stmt, 3);
            e.telefono = t ? t : "";
            std::string cargo = (const char*)sqlite3_column_text(stmt, 4);
            e.cargo = stringToCargo(cargo);
            const char* tc = (const char*)sqlite3_column_text(stmt, 5);
            e.tipo_contrato = (tc && std::string(tc) == "temporal") ? TipoContrato::Temporal : TipoContrato::Fijo;
            e.salario = sqlite3_column_double(stmt, 6);
            std::string tu = (const char*)sqlite3_column_text(stmt, 7);
            e.turno = stringToTurno(tu);
            e.activo = sqlite3_column_int(stmt, 8) == 1;
            const char* fi = (const char*)sqlite3_column_text(stmt, 9);
            e.fecha_ingreso = fi ? fi : "";
            resultado.push_back(e);
        }
    }
    sqlite3_finalize(stmt);
    return resultado;
}

std::vector<Empleado> Empleados::listarActivos() {
    auto* db = BaseDatos::getInstancia();
    std::vector<Empleado> resultado;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->abrir(), "SELECT id, nombre, cedula, telefono, cargo, tipo_contrato, salario, turno, activo, fecha_ingreso FROM empleados WHERE activo = 1 ORDER BY nombre", -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Empleado e;
            e.id = sqlite3_column_int(stmt, 0);
            const char* n = (const char*)sqlite3_column_text(stmt, 1);
            e.nombre = n ? n : "";
            const char* c = (const char*)sqlite3_column_text(stmt, 2);
            e.cedula = c ? c : "";
            const char* t = (const char*)sqlite3_column_text(stmt, 3);
            e.telefono = t ? t : "";
            std::string cargo = (const char*)sqlite3_column_text(stmt, 4);
            e.cargo = stringToCargo(cargo);
            const char* tc = (const char*)sqlite3_column_text(stmt, 5);
            e.tipo_contrato = (tc && std::string(tc) == "temporal") ? TipoContrato::Temporal : TipoContrato::Fijo;
            e.salario = sqlite3_column_double(stmt, 6);
            std::string tu = (const char*)sqlite3_column_text(stmt, 7);
            e.turno = stringToTurno(tu);
            e.activo = true;
            const char* fi = (const char*)sqlite3_column_text(stmt, 9);
            e.fecha_ingreso = fi ? fi : "";
            resultado.push_back(e);
        }
    }
    sqlite3_finalize(stmt);
    return resultado;
}

bool Empleados::actualizar(int id, const std::string& nombre, const std::string& telefono, Cargo cargo, double salario) {
    auto* db = BaseDatos::getInstancia();
    std::string sql = "UPDATE empleados SET nombre = '" + nombre + "', telefono = '" + telefono + "', cargo = '" + cargoToString(cargo) + "', salario = " + std::to_string(salario) + " WHERE id = " + std::to_string(id);
    return db->ejecutarSQL(sql);
}

bool Empleados::eliminar(int id) {
    auto* db = BaseDatos::getInstancia();
    return db->ejecutarSQL("DELETE FROM empleados WHERE id = " + std::to_string(id));
}

bool Empleados::cambiarEstado(int id, bool activo) {
    auto* db = BaseDatos::getInstancia();
    return db->ejecutarSQL("UPDATE empleados SET activo = " + std::to_string(activo ? 1 : 0) + " WHERE id = " + std::to_string(id));
}

int Empleados::registrarAsistencia(int empleado_id, bool presente, const std::string& hora_entrada, const std::string& hora_salida, int horas_extras) {
    auto* db = BaseDatos::getInstancia();
    std::string fecha = fechaActual();
    std::string sql = "INSERT INTO asistencia_empleados (empleado_id, fecha, presente, hora_entrada, hora_salida, horas_extras) VALUES (" +
        std::to_string(empleado_id) + ", '" + fecha + "', " + std::to_string(presente ? 1 : 0) + ", '" + hora_entrada + "', '" + hora_salida + "', " + std::to_string(horas_extras) + ")";
    return db->insertarYGetId(sql);
}

std::vector<Asistencia> Empleados::listarAsistencia(int empleado_id) {
    auto* db = BaseDatos::getInstancia();
    std::vector<Asistencia> resultado;
    sqlite3_stmt* stmt;
    std::string sql;
    if (empleado_id > 0) {
        sql = "SELECT id, empleado_id, fecha, presente, hora_entrada, hora_salida, horas_extras FROM asistencia_empleados WHERE empleado_id = " + std::to_string(empleado_id) + " ORDER BY fecha DESC";
    } else {
        sql = "SELECT id, empleado_id, fecha, presente, hora_entrada, hora_salida, horas_extras FROM asistencia_empleados ORDER BY fecha DESC";
    }
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Asistencia a;
            a.id = sqlite3_column_int(stmt, 0);
            a.empleado_id = sqlite3_column_int(stmt, 1);
            const char* f = (const char*)sqlite3_column_text(stmt, 2);
            a.fecha = f ? f : "";
            a.presente = sqlite3_column_int(stmt, 3) == 1;
            const char* he = (const char*)sqlite3_column_text(stmt, 4);
            a.hora_entrada = he ? he : "";
            const char* hs = (const char*)sqlite3_column_text(stmt, 5);
            a.hora_salida = hs ? hs : "";
            a.horas_extras = sqlite3_column_int(stmt, 6);
            resultado.push_back(a);
        }
    }
    sqlite3_finalize(stmt);
    return resultado;
}

std::vector<Asistencia> Empleados::listarAsistenciaPorFecha(const std::string& fecha) {
    auto* db = BaseDatos::getInstancia();
    std::vector<Asistencia> resultado;
    sqlite3_stmt* stmt;
    std::string sql = "SELECT id, empleado_id, fecha, presente, hora_entrada, hora_salida, horas_extras FROM asistencia_empleados WHERE fecha = '" + fecha + "'";
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Asistencia a;
            a.id = sqlite3_column_int(stmt, 0);
            a.empleado_id = sqlite3_column_int(stmt, 1);
            const char* f = (const char*)sqlite3_column_text(stmt, 2);
            a.fecha = f ? f : "";
            a.presente = sqlite3_column_int(stmt, 3) == 1;
            const char* he = (const char*)sqlite3_column_text(stmt, 4);
            a.hora_entrada = he ? he : "";
            const char* hs = (const char*)sqlite3_column_text(stmt, 5);
            a.hora_salida = hs ? hs : "";
            a.horas_extras = sqlite3_column_int(stmt, 6);
            resultado.push_back(a);
        }
    }
    sqlite3_finalize(stmt);
    return resultado;
}

double Empleados::getHorasExtrasMes(int empleado_id, int mes, int anio) {
    auto* db = BaseDatos::getInstancia();
    sqlite3_stmt* stmt;
    double total = 0;
    std::string sql = "SELECT COALESCE(SUM(horas_extras), 0) FROM asistencia_empleados WHERE empleado_id = " + std::to_string(empleado_id) + " AND strftime('%m', fecha) = '" + std::to_string(mes) + "' AND strftime('%Y', fecha) = '" + std::to_string(anio) + "'";
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            total = sqlite3_column_double(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);
    return total;
}

double Empleados::getTotalSalarios() {
    auto* db = BaseDatos::getInstancia();
    sqlite3_stmt* stmt;
    double total = 0;
    if (sqlite3_prepare_v2(db->abrir(), "SELECT COALESCE(SUM(salario), 0) FROM empleados", -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            total = sqlite3_column_double(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);
    return total;
}

double Empleados::getTotalSalariosActivos() {
    auto* db = BaseDatos::getInstancia();
    sqlite3_stmt* stmt;
    double total = 0;
    if (sqlite3_prepare_v2(db->abrir(), "SELECT COALESCE(SUM(salario), 0) FROM empleados WHERE activo = 1", -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            total = sqlite3_column_double(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);
    return total;
}