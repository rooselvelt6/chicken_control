#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <string>
#include <vector>
#include "modelos.h"

class Dashboard {
public:
    static void mostrarPanelPrincipal();
    static void mostrarPanelLotes();
    static void mostrarPanelFinanciero();
    static void mostrarPanelProduccion();
    static void mostrarGraficoCrecimiento(int lote_id);
    static void mostrarProyeccion(int lote_id);
    static std::string generarBarra(int valor, int max, int longitud = 20);
};

#endif // DASHBOARD_H