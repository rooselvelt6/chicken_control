#ifndef VENTAS_H
#define VENTAS_H

#include <vector>
#include "modelos.h"

class Ventas {
public:
    static int agregarCliente(const std::string& nombre, const std::string& telefono, const std::string& referencia);
    static Cliente* obtenerCliente(int id);
    static Cliente* obtenerClientePorNombre(const std::string& nombre);
    static std::vector<Cliente> listarClientes();
    static std::vector<Venta> listar();
    static int registrarVenta(int lote_id, int cliente_id, const std::vector<double>& pesos,
                              double precio_unitario, const std::vector<std::string>& metodos_pago,
                              const std::string& fecha);
    static Venta* obtenerVenta(int id);
    static std::vector<Venta> listarVentasPorLote(int lote_id);
    static std::vector<Venta> listarVentasPorCliente(int cliente_id);
    static void registrarPago(int venta_id, double monto);
    static std::vector<Venta> ventasPendientes();
    static double totalDeudasPendientes();
    static double totalVentas(int lote_id);
    static double totalCapitalRecuperado(int lote_id);
    static std::pair<int, double> totalVendidos(int lote_id);
    static void imprimirVentas(const std::vector<Venta>& ventas);
    static void cargarClientesEjemplo();
};

#endif // VENTAS_H