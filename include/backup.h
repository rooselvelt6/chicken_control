#ifndef BACKUP_H
#define BACKUP_H

#include <string>
#include <vector>
#include <ctime>

struct BackupInfo {
    std::string nombre_archivo;
    std::string ruta_completa;
    time_t fecha_creacion;
    long long tamaño_bytes;
    std::string tipo;  // "manual", "automatico"
};

class Backup {
public:
    // Crear y restaurar
    static bool crearBackup(const std::string& ruta = "");
    static bool restaurarBackup(const std::string& ruta);
    
    // Backup automático
    static bool programarBackup(int intervalo_horas);
    static void desprogramarBackup();
    static bool verificarBackupAutomatico();
    static int getIntervaloBackup();
    
    // Listar y limpiar
    static std::vector<BackupInfo> listarBackups(const std::string& directorio = "backups/");
    static bool eliminarBackup(const std::string& ruta);
    static void limpiarBackupsAntiguos(int dias = 30);
    
    // Compresión
    static bool crearBackupComprimido(const std::string& ruta = "");
    static bool restaurarBackupComprimido(const std::string& ruta);
    
    // Utilidades
    static std::string getDirectorioBackup();
    static long long getTamañoBaseDatos();
    static bool verificarIntegridad(const std::string& ruta);
};

#endif // BACKUP_H
