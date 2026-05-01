#ifndef GRAFICOS_H
#define GRAFICOS_H

#include <string>
#include <vector>

namespace Graficos {
    void imprimirBarra(double valor, double maximo, int ancho, char caracter);
    void graficoBarras(const std::vector<std::string>& etiquetas, const std::vector<double>& valores, const std::string& titulo);
    void graficoVentasMensuales(int anio);
    void graficoCrecimientoLote(int lote_id);
    void graficoComparativoLotes();
    void graficoPizza(const std::vector<std::string>& etiquetas, const std::vector<double>& valores, const std::string& titulo);
    void histogramaPesos(const std::vector<double>& pesos);
    std::string formatearNumero(double numero);
}

#endif