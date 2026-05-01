#ifndef SERVICIOS_H
#define SERVICIOS_H

#include <string>
#include <vector>
#include "modelos.h"

enum class TipoServicio {
    Agua,
    Luz,
    Gas,
    Internet,
    Telefono,
    Otro
};

struct ConsumoServicio {
    int id = 0;
    int lote_id = 0;
    TipoServicio tipo;
    double cantidad = 0.0;
    double costo_unitario = 0.0;
    double costo_total = 0.0;
    std::string fecha;
    std::string descripcion;
};

class Servicios {
public:
    static int agregarConsumo(int lote_id, TipoServicio tipo, double cantidad, double costo_unitario, const std::string& descripcion = "");
    static std::vector<ConsumoServicio> listarConsumo(int lote_id = 0);
    static std::vector<ConsumoServicio> listarPorTipo(TipoServicio tipo);
    static double getTotalCosto(int lote_id);
    static double getTotalCostoPorTipo(TipoServicio tipo);
    static std::pair<double, double> getCostoYPesoAgua(int lote_id);
    static std::pair<double, double> getCostoYPesoLuz(int lote_id);
    static std::string tipoToString(TipoServicio t);
    static TipoServicio stringToTipo(const std::string& s);
};

#endif // SERVICIOS_H