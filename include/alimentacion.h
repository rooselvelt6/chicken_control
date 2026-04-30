#ifndef ALIMENTACION_H
#define ALIMENTACION_H

#include <vector>
#include "modelos.h"

class Alimentacion {
public:
    static int agregar(const std::string& nombre, const std::string& marca, Fase fase,
                       const AnalisisNutricional& analisis, int contenido_kg,
                       double precio_unitario, int inventario, int inicio_dias, int final_dias);
    static Alimento* obtener(int id);
    static Alimento* obtenerPorNombre(const std::string& nombre);
    static std::vector<Alimento> listar();
    static std::vector<Alimento> listarPorFase(Fase fase);
    static void actualizarInventario(int alimento_id, int cantidad);
    static int registrarConsumo(int lote_id, int alimento_id, int cantidad_sacos);
    static std::vector<ConsumoAlimento> listarConsumo(int lote_id);
    static std::pair<int, double> totalConsumo(int lote_id);
    static double totalInvertido(int lote_id);
    static void imprimirInventario();
    static void cargarEjemplo();
};

#endif // ALIMENTACION_H