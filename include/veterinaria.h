#ifndef VETERINARIA_H
#define VETERINARIA_H

#include <vector>
#include "modelos.h"

class Veterinaria {
public:
    // Vacunas
    static int agregarVacuna(const std::string& nombre, int aplicacion_dias, 
                             double costo, int proveedor_id, const std::string& observaciones);
    static Vacuna* obtenerVacuna(int id);
    static std::vector<Vacuna> listarVacunas();
    static std::vector<Vacuna> listarVacunasPorDia(int dia);
    static void eliminarVacuna(int id);
    
    // Aplicación de vacunas
    static int aplicarVacuna(int lote_id, int vacuna_id, const std::string& fecha, const std::string& obs);
    static std::vector<LoteVacuna> listarVacunasAplicadas(int lote_id);
    
    // Medicamentos
    static int agregarMedicamento(const std::string& nombre, const std::string& principio_activo,
                                  const std::string& dosis, double precio, int proveedor_id,
                                  const std::string& categoria, int stock);
    static Medicamento* obtenerMedicamento(int id);
    static std::vector<Medicamento> listarMedicamentos();
    static std::vector<Medicamento> listarMedicamentosPorCategoria(const std::string& categoria);
    static void actualizarStock(int id, int stock);
    static void eliminarMedicamento(int id);
    
    // Uso de medicamentos
    static int usarMedicamento(int lote_id, int medicamento_id, double cantidad,
                               const std::string& fecha, const std::string& motivo);
    static std::vector<LoteMedicamento> listarMedicamentosUsados(int lote_id);
    
    // Imprimir
    static void imprimirVacunas();
    static void imprimirMedicamentos();
};

#endif // VETERINARIA_H