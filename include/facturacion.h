#ifndef FACTURACION_H
#define FACTURACION_H

#include <string>
#include <vector>
#include "modelos.h"

class Facturacion {
public:
    static int generarFactura(int venta_id, const std::string& cliente_cedula = "", const std::string& cliente_direccion = "");
    static Factura obtenerFactura(int id);
    static std::vector<Factura> listarFacturas();
    static std::vector<Factura> listarFacturasPorVenta(int venta_id);
    static bool anularFactura(int id);
    static std::string generarNumeroFactura();
    static void mostrarFactura(int id);
    static void exportarFacturaPDF(int id, const std::string& filename);
};

#endif // FACTURACION_H