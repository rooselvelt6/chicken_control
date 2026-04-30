#ifndef CORRALES_H
#define CORRALES_H

#include <vector>
#include "modelos.h"

class Corrales {
public:
    // Crear corral con todos los detalles
    static int crear(int granja_id, int numero, const std::string& nombre,
                     double largo, double ancho, double altura,
                     const std::string& tipo_techo, const std::string& proteccion,
                     const std::string& ventilacion, const std::string& orientacion,
                     const std::string& material_paredes,
                     const std::string& sistema_comederos, const std::string& sistema_bebederos,
                     bool tiene_iluminacion, bool tiene_calefaccion, const std::string& tipo_calefaccion,
                     bool tiene_drenaje, int capacidad_maxima, int densidad_maxima);
    
    static Corral* obtener(int id);
    static std::vector<Corral> listarPorGranja(int granja_id, bool activos = true);
    static std::vector<Corral> listarTodos(bool activos = true);
    
    static void actualizar(int id, const std::string& nombre, int capacidad, int densidad);
    static void cerrar(int id);
    static void eliminar(int id);
    
    // Calcular capacidad basada en dimensiones
    static int calcularCapacidad(double area_m2, int densidad);
    static double calcularArea(double largo, double ancho);
    
    // Información
    static void imprimirCorral(const Corral& c);
    static void imprimirCompleto(const Corral& c);
};

#endif // CORRALES_H