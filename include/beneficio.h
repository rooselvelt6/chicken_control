#ifndef BENEFICIO_H
#define BENEFICIO_H

#include <string>
#include <vector>
#include "modelos.h"

class Beneficio {
public:
    static int registrar(int lote_id, int cantidad, double peso_total, TipoBeneficio tipo_beneficio, const std::string& operador_nombre, const std::string& operador_cedula, const std::string& observaciones = "");
    static RegistroBeneficio obtener(int id);
    static std::vector<RegistroBeneficio> listar();
    static std::vector<RegistroBeneficio> listarPorLote(int lote_id);
    static bool eliminar(int id);
    static std::pair<int, double> getTotalBeneficio(int lote_id);
    static void mostrarEstadisticas();
};

#endif // BENEFICIO_H