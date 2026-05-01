#include "../include/backup.h"
#include "../include/base_datos.h"
#include <iostream>
#include <filesystem>
#include <cstring>
#include <fstream>

namespace fs = std::filesystem;

bool Backup::crearBackup(const std::string& ruta) {
    if (!BaseDatos::getInstancia()->abrir()) return false;
    
    std::string backup_path = ruta.empty() ? "datos/granja_backup.db" : ruta;
    
    // Crear directorio si no existe
    fs::path p(backup_path);
    if (p.has_parent_path()) {
        fs::create_directories(p.parent_path());
    }
    
    std::cout << "Creando backup: " << backup_path << "..." << std::endl;
    
    sqlite3* backup_db;
    if (sqlite3_open(backup_path.c_str(), &backup_db) != SQLITE_OK) {
        std::cout << "Error al crear archivo de backup" << std::endl;
        return false;
    }
    
    sqlite3_backup* backup = sqlite3_backup_init(backup_db, "main", BaseDatos::getInstancia()->abrir(), "main");
    if (!backup) {
        sqlite3_close(backup_db);
        return false;
    }
    
    int rc = sqlite3_backup_step(backup, -1);
    sqlite3_backup_finish(backup);
    
    if (rc == SQLITE_DONE) {
        std::cout << "Backup creado: " << backup_path << std::endl;
        sqlite3_close(backup_db);
        return true;
    }
    
    sqlite3_close(backup_db);
    std::cout << "Error al crear backup: " << rc << std::endl;
    return false;
}

bool Backup::restaurarBackup(const std::string& ruta) {
    if (!fs::exists(ruta)) {
        std::cout << "El archivo no existe: " << ruta << std::endl;
        return false;
    }
    
    std::cout << "Restaurando backup: " << ruta << "..." << std::endl;
    
    // Cerrar conexión actual
    BaseDatos::getInstancia()->cerrar();
    
    // Reemplazar base de datos actual
    try {
        fs::copy_file(ruta, "datos/granja.db", fs::copy_options::overwrite_existing);
        std::cout << "Backup restaurado desde: " << ruta << std::endl;
        
        // Reabrir base de datos
        BaseDatos::getInstancia()->abrir();
        return true;
    } catch (...) {
        std::cout << "Error al restaurar backup" << std::endl;
        return false;
    }
}

bool Backup::programarBackup(int intervalo_horas) {
    if (intervalo_horas <= 0) {
        std::cout << "Intervalo debe ser mayor a 0" << std::endl;
        return false;
    }
    
    std::cout << "Backup automático programado cada " << intervalo_horas << " horas" << std::endl;
    std::cout << "Nota: Esta función requiere implementación de demonio" << std::endl;
    return true;
}

void Backup::desprogramarBackup() {
    std::cout << "Backup automático desprogramado" << std::endl;
}

bool Backup::verificarBackupAutomatico() {
    std::cout << "Función de verificación automática requiere implementación" << std::endl;
    return false;
}

int Backup::getIntervaloBackup() {
    return 0;
}

std::vector<BackupInfo> Backup::listarBackups(const std::string& directorio) {
    std::vector<BackupInfo> backups;
    
    if (!fs::exists(directorio)) {
        return backups;
    }
    
    for (const auto& entry : fs::directory_iterator(directorio)) {
        if (fs::is_regular_file(entry.status())) {
            std::string nombre = entry.path().filename().string();
            if (nombre.find("backup_") == 0 && nombre.find(".db") != std::string::npos) {
                BackupInfo info;
                info.nombre_archivo = nombre;
                info.ruta_completa = entry.path().string();
                
                // Obtener fecha de modificación
                auto ftime = fs::last_write_time(entry.path());
                info.fecha_creacion = (time_t)std::chrono::duration_cast<std::chrono::seconds>(
                    ftime.time_since_epoch()).count();
                
                info.tamaño_bytes = fs::file_size(entry.path());
                info.tipo = "manual";
                
                backups.push_back(info);
            }
        }
    }
    
    return backups;
}

bool Backup::eliminarBackup(const std::string& ruta) {
    if (!fs::exists(ruta)) {
        std::cout << "El archivo no existe" << std::endl;
        return false;
    }
    
    try {
        fs::remove(ruta);
        std::cout << "Backup eliminado: " << ruta << std::endl;
        return true;
    } catch (...) {
        std::cout << "Error al eliminar backup" << std::endl;
        return false;
    }
}

void Backup::limpiarBackupsAntiguos(int dias) {
    std::string directorio = "backups";
    if (!fs::exists(directorio)) return;
    
    time_t now = time(nullptr);
    int eliminados = 0;
    
    for (const auto& entry : fs::directory_iterator(directorio)) {
        if (fs::is_regular_file(entry.status())) {
            std::string nombre = entry.path().filename().string();
            if (nombre.find("backup_") == 0) {
                auto ftime = fs::last_write_time(entry.path());
                time_t mtime = (time_t)std::chrono::duration_cast<std::chrono::seconds>(
                    ftime.time_since_epoch()).count();
                
                double dias_transcurridos = difftime(now, mtime) / (24 * 3600.0);
                if (dias_transcurridos > dias) {
                    fs::remove(entry.path());
                    eliminados++;
                }
            }
        }
    }
    
    if (eliminados > 0) {
        std::cout << "Eliminados " << eliminados << " backups antiguos (> " << dias << " días)" << std::endl;
    }
}

bool Backup::crearBackupComprimido(const std::string& ruta) {
    std::cout << "Función de compresión requiere biblioteca zlib" << std::endl;
    std::cout << "Usando backup normal en su lugar..." << std::endl;
    return crearBackup(ruta);
}

bool Backup::restaurarBackupComprimido(const std::string& ruta) {
    std::cout << "Función de descompresión requiere biblioteca zlib" << std::endl;
    return restaurarBackup(ruta);
}

std::string Backup::getDirectorioBackup() {
    return "backups";
}

long long Backup::getTamañoBaseDatos() {
    try {
        return fs::file_size("datos/granja.db");
    } catch (...) {
        return 0;
    }
}

bool Backup::verificarIntegridad(const std::string& ruta) {
    if (!fs::exists(ruta)) return false;
    
    // Verificar que sea una base de datos SQLite válida
    std::ifstream file(ruta.c_str(), std::ios::binary);
    if (!file) return false;
    
    char header[16];
    file.read(header, 16);
    
    if (file.gcount() < 16) return false;
    
    // SQLite header magic: "SQLite format 3\000"
    std::string header_str(header, 16);
    return header_str.find("SQLite format 3") == 0;
}
