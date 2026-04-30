#ifndef BASE_DATOS_H
#define BASE_DATOS_H

#include <string>
#include <sqlite3.h>
#include "modelos.h"

class BaseDatos {
private:
    sqlite3* db;
    static BaseDatos* instancia;
    std::string db_path;

    BaseDatos();
    ~BaseDatos();

public:
    static BaseDatos* getInstancia();
    sqlite3* abrir();
    void cerrar();
    bool ejecutarSQL(const std::string& sql);
    int insertarYGetId(const std::string& sql);
    std::string getConfigValue(const std::string& clave);
    void setConfigValue(const std::string& clave, const std::string& valor);
    Configuracion getConfiguracion();
    void inicializar();
    void actualizarSchema();
};

#endif // BASE_DATOS_H