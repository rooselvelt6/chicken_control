#include "../include/usuarios.h"
#include "../include/base_datos.h"
#include "../include/utils.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>
#include <ctime>

// Sesión actual
Sesion Usuarios::sesion_actual = Sesion();

std::string Usuarios::hashPassword(const std::string& password) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char*)password.c_str(), password.length(), hash);
    
    std::stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

bool Usuarios::verificarPassword(const std::string& password, const std::string& hash) {
    std::string hash_input = Usuarios::hashPassword(password);
    return hash_input == hash;
}

int Usuarios::login(const std::string& username, const std::string& password) {
    if (username.empty() || password.empty()) {
        std::cout << "Usuario y contraseña requeridos" << std::endl;
        return -1;
    }
    
    std::string username_sql = sanitizarSQL(username);
    std::string sql = "SELECT id, password_hash, rol, nombre_completo, activo FROM usuarios WHERE username = '" + username_sql + "'";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(BaseDatos::getInstancia()->abrir(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cout << "Error al verificar usuario" << std::endl;
        return -1;
    }
    
    int user_id = -1;
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        user_id = sqlite3_column_int(stmt, 0);
        std::string hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string rol = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string nombre = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        bool activo = sqlite3_column_int(stmt, 4) > 0;
        
        if (!activo) {
            std::cout << "Usuario inactivo. Contacte al administrador." << std::endl;
            sqlite3_finalize(stmt);
            return -1;
        }
        
        if (verificarPassword(password, hash)) {
            // Actualizar último acceso
            std::string update_sql = "UPDATE usuarios SET ultimo_acceso = datetime('now') WHERE id = " + std::to_string(user_id);
            BaseDatos::getInstancia()->ejecutarSQL(update_sql);
            
            // Crear sesión
            sesion_actual.usuario_id = user_id;
            sesion_actual.username = username;
            sesion_actual.rol = rol;
            sesion_actual.inicio_sesion = time(nullptr);
            sesion_actual.autenticado = true;
            
            std::cout << "Bienvenido, " << nombre << " (" << rol << ")" << std::endl;
        } else {
            std::cout << "Contraseña incorrecta" << std::endl;
            user_id = -1;
        }
    } else {
        std::cout << "Usuario no encontrado" << std::endl;
    }
    
    sqlite3_finalize(stmt);
    return user_id;
}

void Usuarios::logout() {
    if (sesion_actual.autenticado) {
        std::cout << "Sesión cerrada para: " << sesion_actual.username << std::endl;
    }
    sesion_actual = Sesion();
}

bool Usuarios::cambiarPassword(int user_id, const std::string& old_pass, const std::string& new_pass) {
    if (!sesionIniciada() || (getUsuarioActualId() != user_id && getRolActual() != "admin")) {
        std::cout << "No tiene permiso para realizar esta acción" << std::endl;
        return false;
    }
    
    if (new_pass.length() < 6) {
        std::cout << "La contraseña debe tener al menos 6 caracteres" << std::endl;
        return false;
    }
    
    // Verificar contraseña actual
    std::string sql = "SELECT password_hash FROM usuarios WHERE id = " + std::to_string(user_id);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(BaseDatos::getInstancia()->abrir(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string hash_actual = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        if (!verificarPassword(old_pass, hash_actual) && getRolActual() != "admin") {
            std::cout << "Contraseña actual incorrecta" << std::endl;
            sqlite3_finalize(stmt);
            return false;
        }
    }
    sqlite3_finalize(stmt);
    
    // Actualizar contraseña
    std::string new_hash = hashPassword(new_pass);
    std::string update_sql = "UPDATE usuarios SET password_hash = '" + new_hash + "' WHERE id = " + std::to_string(user_id);
    
    if (BaseDatos::getInstancia()->ejecutarSQL(update_sql)) {
        std::cout << "Contraseña actualizada exitosamente" << std::endl;
        return true;
    }
    return false;
}

bool Usuarios::restablecerPassword(int admin_id, int target_user_id, const std::string& new_pass) {
    if (!sesionIniciada() || getRolActual() != "admin") {
        std::cout << "Solo administradores pueden restablecer contraseñas" << std::endl;
        return false;
    }
    
    if (new_pass.length() < 6) {
        std::cout << "La contraseña debe tener al menos 6 caracteres" << std::endl;
        return false;
    }
    
    std::string new_hash = hashPassword(new_pass);
    std::string sql = "UPDATE usuarios SET password_hash = '" + new_hash + "' WHERE id = " + std::to_string(target_user_id);
    
    if (BaseDatos::getInstancia()->ejecutarSQL(sql)) {
        std::cout << "Contraseña restablecida exitosamente" << std::endl;
        return true;
    }
    return false;
}

int Usuarios::agregar(const std::string& username, const std::string& password,
                      const std::string& rol, const std::string& nombre_completo) {
    if (!sesionIniciada() || getRolActual() != "admin") {
        std::cout << "Solo administradores pueden crear usuarios" << std::endl;
        return -1;
    }
    
    if (username.empty() || password.length() < 6) {
        std::cout << "Usuario y contraseña (min 6 chars) requeridos" << std::endl;
        return -1;
    }
    
    // Verificar que no exista el username
    std::string check_sql = "SELECT id FROM usuarios WHERE username = '" + sanitizarSQL(username) + "'";
    sqlite3_stmt* check_stmt;
    if (sqlite3_prepare_v2(BaseDatos::getInstancia()->abrir(), check_sql.c_str(), -1, &check_stmt, nullptr) != SQLITE_OK) {
        return -1;
    }
    
    if (sqlite3_step(check_stmt) == SQLITE_ROW) {
        std::cout << "El nombre de usuario ya existe" << std::endl;
        sqlite3_finalize(check_stmt);
        return -1;
    }
    sqlite3_finalize(check_stmt);
    
    std::string hash = hashPassword(password);
    std::string sql = "INSERT INTO usuarios (username, password_hash, rol, nombre_completo) VALUES ('" +
                    sanitizarSQL(username) + "', '" + hash + "', '" +
                    sanitizarSQL(rol) + "', '" + sanitizarSQL(nombre_completo) + "')";
    
    if (BaseDatos::getInstancia()->ejecutarSQL(sql)) {
        int id = (int)sqlite3_last_insert_rowid(BaseDatos::getInstancia()->abrir());
        std::cout << "Usuario '" << username << "' creado (ID: " << id << ")" << std::endl;
        return id;
    }
    return -1;
}

bool Usuarios::editar(int user_id, const std::string& email, const std::string& telefono) {
    if (!sesionIniciada()) {
        std::cout << "Debe iniciar sesión" << std::endl;
        return false;
    }
    
    if (getUsuarioActualId() != user_id && getRolActual() != "admin") {
        std::cout << "No tiene permiso para editar este usuario" << std::endl;
        return false;
    }
    
    std::string sql = "UPDATE usuarios SET email = '" + sanitizarSQL(email) + 
                    "', telefono = '" + sanitizarSQL(telefono) + 
                    "' WHERE id = " + std::to_string(user_id);
    
    if (BaseDatos::getInstancia()->ejecutarSQL(sql)) {
        std::cout << "Usuario actualizado exitosamente" << std::endl;
        return true;
    }
    return false;
}

bool Usuarios::desactivar(int user_id) {
    if (!sesionIniciada() || getRolActual() != "admin") {
        std::cout << "Solo administradores pueden desactivar usuarios" << std::endl;
        return false;
    }
    
    std::string sql = "UPDATE usuarios SET activo = 0 WHERE id = " + std::to_string(user_id);
    if (BaseDatos::getInstancia()->ejecutarSQL(sql)) {
        std::cout << "Usuario desactivado" << std::endl;
        return true;
    }
    return false;
}

bool Usuarios::activar(int user_id) {
    if (!sesionIniciada() || getRolActual() != "admin") {
        std::cout << "Solo administradores pueden activar usuarios" << std::endl;
        return false;
    }
    
    std::string sql = "UPDATE usuarios SET activo = 1 WHERE id = " + std::to_string(user_id);
    if (BaseDatos::getInstancia()->ejecutarSQL(sql)) {
        std::cout << "Usuario activado" << std::endl;
        return true;
    }
    return false;
}

bool Usuarios::tienePermiso(int user_id, const std::string& comando) {
    Usuario* u = obtenerPorId(user_id);
    if (!u) return false;
    
    // Administradores tienen todos los permisos
    if (u->rol == "admin") return true;
    
    // Veterinarios pueden: animales, vacunas, veterinaria, reportes
    if (u->rol == "veterinario") {
        std::vector<std::string> permisos = {"animal", "pesaje", "vacuna", "veterinaria", "reporte", "dashboard", "metricas"};
        for (const auto& p : permisos) {
            if (comando.find(p) != std::string::npos) return true;
        }
        return false;
    }
    
    // Operadores pueden: animales, alimentos, ventas
    if (u->rol == "operador") {
        std::vector<std::string> permisos = {"animal", "alimento", "venta", "cliente"};
        for (const auto& p : permisos) {
            if (comando.find(p) != std::string::npos) return true;
        }
        return false;
    }
    
    return false;
}

std::vector<std::string> Usuarios::getPermisosRol(const std::string& rol) {
    if (rol == "admin") {
        return {"all"};
    } else if (rol == "veterinario") {
        return {"animal", "pesaje", "vacuna", "veterinaria", "reporte", "dashboard", "metricas", "lote", "factura"};
    } else if (rol == "operador") {
        return {"animal", "alimento", "venta", "cliente", "lote", "proveedor"};
    }
    return {};
}

std::vector<Usuario> Usuarios::listar(bool solo_activos) {
    std::vector<Usuario> usuarios;
    
    std::string sql = "SELECT id, username, rol, nombre_completo, email, telefono, activo, ultimo_acceso FROM usuarios";
    if (solo_activos) sql += " WHERE activo = 1";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(BaseDatos::getInstancia()->abrir(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return usuarios;
    }
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Usuario u;
        u.id = sqlite3_column_int(stmt, 0);
        u.username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        u.rol = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        u.nombre_completo = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        u.email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        u.telefono = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        u.activo = sqlite3_column_int(stmt, 6) > 0;
        u.ultimo_acceso = sqlite3_column_int64(stmt, 7);
        usuarios.push_back(u);
    }
    
    sqlite3_finalize(stmt);
    return usuarios;
}

Usuario* Usuarios::obtenerPorId(int user_id) {
    static Usuario u;  // Static to avoid returning local variable
    std::string sql = "SELECT id, username, rol, nombre_completo, email, telefono, activo, ultimo_acceso FROM usuarios WHERE id = " + std::to_string(user_id);
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(BaseDatos::getInstancia()->abrir(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return nullptr;
    }
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        u.id = sqlite3_column_int(stmt, 0);
        u.username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        u.rol = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        u.nombre_completo = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        u.email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        u.telefono = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        u.activo = sqlite3_column_int(stmt, 6) > 0;
        u.ultimo_acceso = sqlite3_column_int64(stmt, 7);
        sqlite3_finalize(stmt);
        return &u;
    }
    
    sqlite3_finalize(stmt);
    return nullptr;
}

Usuario* Usuarios::obtenerPorUsername(const std::string& username) {
    static Usuario u;  // Static to avoid returning local variable
    std::string sql = "SELECT id, username, rol, nombre_completo, email, telefono, activo, ultimo_acceso FROM usuarios WHERE username = '" + sanitizarSQL(username) + "'";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(BaseDatos::getInstancia()->abrir(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return nullptr;
    }
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        u.id = sqlite3_column_int(stmt, 0);
        u.username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        u.rol = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        u.nombre_completo = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        u.email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        u.telefono = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        u.activo = sqlite3_column_int(stmt, 6) > 0;
        u.ultimo_acceso = sqlite3_column_int64(stmt, 7);
        sqlite3_finalize(stmt);
        return &u;
    }
    
    sqlite3_finalize(stmt);
    return nullptr;
}

bool Usuarios::sesionIniciada() {
    return sesion_actual.autenticado;
}

int Usuarios::getUsuarioActualId() {
    return sesion_actual.usuario_id;
}

std::string Usuarios::getRolActual() {
    return sesion_actual.rol;
}
