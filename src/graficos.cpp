#include "../include/graficos.h"
#include "../include/ventas.h"
#include "../include/lotes.h"
#include "../include/animales.h"
#include "../include/pesaje.h"
#include "../include/alimentacion.h"
#include "../include/utils.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>

namespace Graficos {

std::string formatearNumero(double numero) {
    std::stringstream ss;
    if (numero >= 1000) {
        ss << std::fixed << std::setprecision(1) << (numero / 1000.0) << "K";
    } else if (numero >= 1) {
        ss << std::fixed << std::setprecision(0) << numero;
    } else {
        ss << std::fixed << std::setprecision(2) << numero;
    }
    return ss.str();
}

void imprimirBarra(double valor, double maximo, int ancho, const std::string& caracter) {
    if (maximo <= 0) maximo = 1;
    int barra_len = (int)((valor / maximo) * ancho);
    if (barra_len < 1 && valor > 0) barra_len = 1;
    for (int i = 0; i < barra_len; i++) {
        std::cout << caracter;
    }
    for (int i = barra_len; i < ancho; i++) {
        std::cout << " ";
    }
}

void graficoBarras(const std::vector<std::string>& etiquetas, const std::vector<double>& valores, const std::string& titulo) {
    if (etiquetas.empty() || valores.empty()) {
        std::cout << "No hay datos para graficar" << std::endl;
        return;
    }

    double maximo = *max_element(valores.begin(), valores.end());
    if (maximo == 0) maximo = 1;

    int ancho_grafico = 40;

    std::cout << "\n╔";
    for (size_t i = 0; i < titulo.length() + 2; i++) std::cout << "═";
    std::cout << "╗" << std::endl;
    std::cout << "║ " << titulo << " ║" << std::endl;
    std::cout << "╠";
    for (size_t i = 0; i < ancho_grafico + 20; i++) std::cout << "═";
    std::cout << "╣" << std::endl;

    for (size_t i = 0; i < etiquetas.size() && i < valores.size(); i++) {
        std::cout << "║ " << std::setw(15) << std::left << etiquetas[i] << " │ ";
        imprimirBarra(valores[i], maximo, ancho_grafico, "█");
        std::cout << " │ " << formatearNumero(valores[i]) << std::endl;
    }

    std::cout << "╚";
    for (size_t i = 0; i < ancho_grafico + 20; i++) std::cout << "═";
    std::cout << "╝" << std::endl;
}

void graficoVentasMensuales(int anio) {
    std::vector<std::string> meses = {"Ene", "Feb", "Mar", "Abr", "May", "Jun", "Jul", "Ago", "Sep", "Oct", "Nov", "Dic"};
    std::vector<double> ventas_mes(12, 0.0);

    auto ventas = Ventas::listar();
    for (const auto& v : ventas) {
        if (v.fecha.length() >= 4) {
            int anio_venta = std::stoi(v.fecha.substr(0, 4));
            if (anio_venta == anio && v.fecha.length() >= 7) {
                int mes = std::stoi(v.fecha.substr(5, 2)) - 1;
                if (mes >= 0 && mes < 12) {
                    ventas_mes[mes] += v.total;
                }
            }
        }
    }

    graficoBarras(meses, ventas_mes, "Ventas Mensuales " + std::to_string(anio) + " (USD)");
}

void graficoCrecimientoLote(int lote_id) {
    auto pesajes = Pesaje::listarPorLote(lote_id);
    if (pesajes.empty()) {
        std::cout << "No hay datos de pesaje para el lote " << lote_id << std::endl;
        return;
    }

    std::vector<std::string> etiquetas;
    std::vector<double> pesos;

    for (const auto& p : pesajes) {
        etiquetas.push_back("Sem " + std::to_string(p.semana));
        pesos.push_back(p.peso_promedio);
    }

    graficoBarras(etiquetas, pesos, "Crecimiento Lote #" + std::to_string(lote_id) + " (kg)");
}

void graficoComparativoLotes() {
    auto lotes = Lotes::listar(false);
    std::vector<std::string> etiquetas;
    std::vector<double> valores;

    for (const auto& l : lotes) {
        double total = Ventas::totalVentas(l.id);
        etiquetas.push_back("Lote " + std::to_string(l.numero));
        valores.push_back(total);
    }

    if (!etiquetas.empty()) {
        graficoBarras(etiquetas, valores, "Comparativo de Ventas por Lote (USD)");
    } else {
        std::cout << "No hay lotes para comparar" << std::endl;
    }
}

void graficoPizza(const std::vector<std::string>& etiquetas, const std::vector<double>& valores, const std::string& titulo) {
    if (etiquetas.empty() || valores.empty()) {
        std::cout << "No hay datos para graficar" << std::endl;
        return;
    }

    double total = 0;
    for (double v : valores) total += v;
    if (total == 0) total = 1;

    std::vector<std::string> caracteres = {"●", "◐", "○", "◑", "◉", "◈", "◆", "◇", "★", "☆"};

    std::cout << "\n╔═══════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║ " << std::setw(50) << std::left << titulo << " ║" << std::endl;
    std::cout << "╠═══════════════════════════════════════════════════╣" << std::endl;

    for (size_t i = 0; i < etiquetas.size() && i < valores.size(); i++) {
        double pct = (valores[i] / total) * 100;
        std::string c = caracteres[i % caracteres.size()];
        std::cout << "║ " << c << " " << std::setw(20) << std::left << etiquetas[i]
                  << ": " << std::setw(8) << std::fixed << std::setprecision(1) << std::right << pct << "%  ║" << std::endl;
    }

    std::cout << "╚═══════════════════════════════════════════════════╝" << std::endl;
}

void histogramaPesos(const std::vector<double>& pesos) {
    if (pesos.empty()) {
        std::cout << "No hay pesos para graficar" << std::endl;
        return;
    }

    double min_p = *min_element(pesos.begin(), pesos.end());
    double max_p = *max_element(pesos.begin(), pesos.end());
    int num_intervalos = 8;
    double intervalo = (max_p - min_p) / num_intervalos;
    if (intervalo == 0) intervalo = 1;

    std::vector<int> conteo(num_intervalos, 0);
    for (double p : pesos) {
        int idx = (int)((p - min_p) / intervalo);
        if (idx >= num_intervalos) idx = num_intervalos - 1;
        if (idx < 0) idx = 0;
        conteo[idx]++;
    }

    int max_conteo = *max_element(conteo.begin(), conteo.end());
    if (max_conteo == 0) max_conteo = 1;

    std::cout << "\n╔═══════════════════════════════════════════════╗" << std::endl;
    std::cout << "║        Distribución de Pesos (kg)            ║" << std::endl;
    std::cout << "╠═══════════════════════════════════════════════╣" << std::endl;

    for (int i = 0; i < num_intervalos; i++) {
        double rango_inicio = min_p + (i * intervalo);
        double rango_fin = rango_inicio + intervalo;
        std::cout << "║ " << std::fixed << std::setprecision(1) << std::setw(5) << rango_inicio << "-"
                  << std::setw(5) << rango_fin << " kg │ ";
        imprimirBarra(conteo[i], max_conteo, 20, "█");
        std::cout << " " << conteo[i] << " ║" << std::endl;
    }

    std::cout << "╚═══════════════════════════════════════════════╝" << std::endl;
}

}