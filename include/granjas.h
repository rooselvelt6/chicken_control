#ifndef GRANJAS_H
#define GRANJAS_H

#include <vector>
#include "modelos.h"

class Granjas {
public:
    // Tipos de granja
    static int crearTipoGranja(const std::string& nombre, const std::string& descripcion, 
                                int capacidad, double costo, int densidad);
    static std::vector<TipoGranjaInfo> listarTiposGranja();
    static TipoGranjaInfo* obtenerTipoGranja(int id);
    
    // Granjas
    static int crear(const std::string& nombre, int tipo_granja_id, 
                     const std::string& ubicacion, const std::string& dimensiones, int densidad);
    static Granja* obtener(int id);
    static std::vector<Granja> listar(bool activos = true);
    static void actualizar(int id, const std::string& nombre, int tipo_granja_id, 
                          const std::string& ubicacion, int densidad);
    static void eliminar(int id);
    static void imprimirGranja(const Granja& g);
};

#endif // GRANJAS_H