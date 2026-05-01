#ifndef UI_H
#define UI_H

#include <string>
#include <vector>
#include <ncurses.h>

namespace UI {

struct OpcionMenu {
    std::string titulo;
    std::string descripcion;
    int (*funcion)();
};

struct MenuItem {
    std::string titulo;
    int id;
};

void inicializar();
void finalizar();
void dibujarMarco(const std::string& titulo);
int menuPrincipal();
int menuLotes();
int menuAnimales();
int menuAlimentacion();
int menuVentas();
int menuGranjas();
int menuVeterinaria();
int menuReportes();
int menuConfiguracion();
void esperarTecla(const std::string& mensaje);
std::string inputString(const std::string& etiqueta, int maxLen = 50);
int inputInt(const std::string& etiqueta, int min = 0, int max = 999999);
double inputDouble(const std::string& etiqueta, double min = 0.0, double max = 999999.0);
bool confirmar(const std::string& mensaje);
bool validarNumeroPositivo(const std::string& s);
bool validarDecimal(const std::string& s);
bool validarTelefono(const std::string& s);
bool validarFecha(const std::string& s);
std::string inputFecha(const std::string& etiqueta);
void mostrarMensaje(const std::string& titulo, const std::string& mensaje);
void mostrarTabla(const std::vector<std::string>& headers, const std::vector<std::vector<std::string>>& filas, const std::string& titulo);
int selectorLista(const std::vector<std::string>& opciones, const std::string& titulo);
std::string selectorOpcion(const std::vector<std::pair<std::string, std::string>>& opciones, const std::string& titulo);

}

#endif // UI_H