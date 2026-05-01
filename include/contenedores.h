#ifndef CONTENEDORES_H
#define CONTENEDORES_H

#include <string>
#include <vector>
#include "modelos.h"

class Contenedores {
public:
    static int crear(const std::string& nombre, const std::string& ubicacion, int capacidad, double temperatura = 4.0);
    static Contenedor obtener(int id);
    static std::vector<Contenedor> listar();
    static std::vector<Contenedor> listarDisponibles();
    static bool actualizar(int id, const std::string& nombre, const std::string& ubicacion, int capacidad, double temperatura, EstadoContenedor estado);
    static bool eliminar(int id);
    static bool cambiarEstado(int id, EstadoContenedor estado);
    
    // Productos en contenedores
    static int agregarProducto(int contenedor_id, int lote_id, int cantidad, double peso_total, const std::string& fecha_vencimiento = "");
    static std::vector<ProductoProcesado> listarProductos(int contenedor_id = 0);
    static bool retirerProducto(int producto_id, int cantidad);
    static bool eliminarProducto(int producto_id);
    static int getCantidadProductos(int contenedor_id);
};

#endif // CONTENEDORES_H