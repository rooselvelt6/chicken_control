#ifndef INVENTARIO_INSUMOS_H
#define INVENTARIO_INSUMOS_H

#include <string>
#include <vector>
#include <ctime>

struct Insumo {
    int id = 0;
    std::string nombre;
    std::string categoria;  // "medicamento", "vacuna", "limpieza", "equipo", "otro"
    std::string descripcion;
    int cantidad = 0;
    int stock_minimo = 10;
    std::string unidad;      // "ml", "tabletas", "cajas", "piezas", "litros"
    double precio_unitario = 0.0;
    std::string fecha_caducidad;
    std::string proveedor;
    std::string lote_compra;
    bool activo = true;
};

struct MovimientoInsumo {
    int id = 0;
    int insumo_id = 0;
    std::string tipo;         // "entrada", "salida", "ajuste"
    int cantidad = 0;
    std::string motivo;
    std::string fecha;
    int usuario_id = 0;
    std::string observaciones;
};

class InventarioInsumos {
public:
    // Gestión de insumos
    static int agregar(const std::string& nombre, const std::string& categoria,
                      int cantidad, const std::string& unidad);
    static bool editar(int insumo_id, const std::string& descripcion, 
                      int stock_minimo, double precio_unitario);
    static bool consumir(int insumo_id, int cantidad, const std::string& motivo);
    static bool agregarStock(int insumo_id, int cantidad, double precio_unitario,
                           const std::string& proveedor, const std::string& lote);
    static bool desactivar(int insumo_id);
    
    // Consultas
    static Insumo* obtenerPorId(int insumo_id);
    static std::vector<Insumo> listar(const std::string& categoria = "");
    static std::vector<Insumo> listarBajoStock();
    static std::vector<Insumo> listarPorCaducar(int dias = 30);
    static std::vector<Insumo> listarCaducados();
    
    // Movimientos
    static int registrarMovimiento(int insumo_id, const std::string& tipo,
                                    int cantidad, const std::string& motivo,
                                    int usuario_id = -1);
    static std::vector<MovimientoInsumo> historialMovimientos(int insumo_id = -1);
    
    // Reportes
    static void reporteGeneral();
    static void reporteBajoStock();
    static void reporteCaducidad();
    static void reporteMovimientos(const std::string& fecha_ini, 
                                   const std::string& fecha_fin);
    static double valorTotalInventario();
    
    // Alertas
    static std::vector<std::string> generarAlertas();
    static bool verificarStockBajo(int insumo_id);
    static bool verificarCaducidad(int insumo_id, int dias = 30);
    
    // Utilidades
    static bool validarCategoria(const std::string& categoria);
    static std::vector<std::string> getCategoriasValidas();
    static std::string categoriaToString(const std::string& categoria);
};

#endif // INVENTARIO_INSUMOS_H
