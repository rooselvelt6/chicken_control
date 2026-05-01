#ifndef USUARIOS_H
#define USUARIOS_H

#include <string>
#include <vector>
#include <ctime>

struct Usuario {
    int id = 0;
    std::string username;
    std::string password_hash;  // SHA-256 hash
    std::string rol;             // "admin", "veterinario", "operador"
    std::string nombre_completo;
    std::string email;
    std::string telefono;
    bool activo = true;
    time_t ultimo_acceso = 0;
};

struct Sesion {
    int usuario_id = -1;
    std::string username;
    std::string rol;
    time_t inicio_sesion = 0;
    bool autenticado = false;
};

class Usuarios {
public:
    // Autenticación
    static int login(const std::string& username, const std::string& password);
    static void logout();
    static bool cambiarPassword(int user_id, const std::string& old_pass, const std::string& new_pass);
    static bool restablecerPassword(int admin_id, int target_user_id, const std::string& new_pass);
    
    // Gestión de usuarios
    static int agregar(const std::string& username, const std::string& password, 
                      const std::string& rol, const std::string& nombre_completo);
    static bool editar(int user_id, const std::string& email, const std::string& telefono);
    static bool desactivar(int user_id);
    static bool activar(int user_id);
    
    // Permisos
    static bool tienePermiso(int user_id, const std::string& comando);
    static std::vector<std::string> getPermisosRol(const std::string& rol);
    
    // Consultas
    static std::vector<Usuario> listar(bool solo_activos = true);
    static Usuario* obtenerPorId(int user_id);
    static Usuario* obtenerPorUsername(const std::string& username);
    
    // Utilidades
    static std::string hashPassword(const std::string& password);
    static bool verificarPassword(const std::string& password, const std::string& hash);
    
    // Sesión actual
    static Sesion sesion_actual;
    static bool sesionIniciada();
    static int getUsuarioActualId();
    static std::string getRolActual();
};

#endif // USUARIOS_H
