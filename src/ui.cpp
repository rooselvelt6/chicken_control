#include "../include/ui.h"
#include "../include/modelos.h"
#include "../include/utils.h"
#include "../include/base_datos.h"
#include "../include/lotes.h"
#include "../include/animales.h"
#include "../include/alimentacion.h"
#include "../include/ventas.h"
#include "../include/granjas.h"
#include "../include/corrales.h"
#include "../include/proveedores.h"
#include "../include/veterinaria.h"
#include "../include/pesaje.h"
#include "../include/metricas.h"
#include "../include/herramientas.h"
#include "../include/inversores.h"
#include "../include/reportes.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

namespace UI {

static int colorTitulo, colorSeleccion, colorNormal, colorBorde;
static WINDOW* winPrincipal = nullptr;

void inicializar() {
    initscr();
    start_color();
    init_pair(1, COLOR_CYAN, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_WHITE, COLOR_BLACK);
    init_pair(4, COLOR_GREEN, COLOR_BLACK);
    init_pair(5, COLOR_RED, COLOR_BLACK);
    init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(7, COLOR_BLUE, COLOR_BLACK);
    init_pair(8, COLOR_GREEN, COLOR_BLUE);
    init_pair(9, COLOR_WHITE, COLOR_BLUE);
    init_pair(10, COLOR_BLACK, COLOR_GREEN);
    init_pair(11, COLOR_WHITE, COLOR_RED);
    colorTitulo = 1;
    colorSeleccion = 2;
    colorNormal = 3;
    colorBorde = 4;
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
}

void finalizar() {
    if (winPrincipal) {
        delwin(winPrincipal);
    }
    endwin();
}

void dibujarMarco(const std::string& titulo) {
    int alto, ancho;
    getmaxyx(stdscr, alto, ancho);
    
    attron(COLOR_PAIR(9));
    for (int x = 0; x < ancho - 1; x++) {
        mvaddch(0, x, ' ');
    }
    attroff(COLOR_PAIR(9));
    
    attron(COLOR_PAIR(colorBorde));
    for (int x = 0; x < ancho - 1; x++) {
        mvaddch(2, x, ACS_HLINE);
        mvaddch(alto - 2, x, ACS_HLINE);
    }
    for (int y = 0; y < alto - 1; y++) {
        mvaddch(y, 0, ACS_VLINE);
        mvaddch(y, ancho - 2, ACS_VLINE);
    }
    mvaddch(2, 0, ACS_LTEE);
    mvaddch(2, ancho - 2, ACS_RTEE);
    mvaddch(alto - 2, 0, ACS_LLCORNER);
    mvaddch(alto - 2, ancho - 2, ACS_LRCORNER);
    attroff(COLOR_PAIR(colorBorde));

    attron(COLOR_PAIR(colorTitulo) | A_BOLD);
    int posX = (ancho - titulo.length()) / 2;
    mvaddstr(1, posX, titulo.c_str());
    attroff(COLOR_PAIR(colorTitulo) | A_BOLD);

    move(alto - 1, 0);
    clrtoeol();
    attron(COLOR_PAIR(7));
    mvaddstr(alto - 1, 1, " [Flechas] Navegar  [Enter] Seleccionar  [Esc] Salir ");
    attroff(COLOR_PAIR(7));
}

void esperarTecla(const std::string& mensaje) {
    int alto, ancho;
    getmaxyx(stdscr, alto, ancho);
    attron(COLOR_PAIR(colorBorde));
    mvaddstr(alto - 3, 1, mensaje.c_str());
    attroff(COLOR_PAIR(colorBorde));
    getch();
}

std::string inputString(const std::string& etiqueta, int maxLen) {
    echo();
    int alto, ancho;
    getmaxyx(stdscr, alto, ancho);
    int fila = alto / 2;
    mvprintw(fila, 1, "%s: ", etiqueta.c_str());
    char buffer[maxLen + 1];
    getstr(buffer);
    noecho();
    return std::string(buffer);
}

int inputInt(const std::string& etiqueta, int min, int max) {
    while (true) {
        std::string res = inputString(etiqueta);
        if (res.empty()) return 0;
        if (validarNumeroPositivo(res)) {
            int val = std::stoi(res);
            if (val >= min && val <= max) {
                return val;
            }
            mostrarMensaje("Error", ("Valor debe estar entre " + std::to_string(min) + " y " + std::to_string(max)).c_str());
        } else {
            mostrarMensaje("Error", "Ingrese un numero valido");
        }
    }
}

double inputDouble(const std::string& etiqueta, double min, double max) {
    while (true) {
        std::string res = inputString(etiqueta);
        if (res.empty()) return 0.0;
        if (validarDecimal(res)) {
            double val = std::stod(res);
            if (val >= min && val <= max) {
                return val;
            }
            mostrarMensaje("Error", ("Valor debe estar entre " + std::to_string(min) + " y " + std::to_string(max)).c_str());
        } else {
            mostrarMensaje("Error", "Ingrese un numero valido");
        }
    }
}

bool confirmar(const std::string& mensaje) {
    int alto, ancho;
    getmaxyx(stdscr, alto, ancho);
    int filas = 7;
    int cols = mensaje.length() + 10;
    int inicioY = (alto - filas) / 2;
    int inicioX = (ancho - cols) / 2;

    WINDOW* win = newwin(filas, cols, inicioY, inicioX);
    box(win, 0, 0);
    mvwaddstr(win, 1, 2, mensaje.c_str());
    mvwaddstr(win, 3, 2, "[S] Si   [N] No");
    wrefresh(win);

    int ch = wgetch(win);
    delwin(win);
    return (ch == 's' || ch == 'S' || ch == 'y' || ch == 'Y');
}

void mostrarMensaje(const std::string& titulo, const std::string& mensaje) {
    int alto, ancho;
    getmaxyx(stdscr, alto, ancho);
    int filas = 6;
    int cols = std::max((int)mensaje.length(), (int)titulo.length()) + 6;
    int inicioY = (alto - filas) / 2;
    int inicioX = (ancho - cols) / 2;

    WINDOW* win = newwin(filas, cols, inicioY, inicioX);
    box(win, 0, 0);
    wattron(win, A_BOLD);
    mvwaddstr(win, 1, (cols - titulo.length()) / 2, titulo.c_str());
    wattroff(win, A_BOLD);
    mvwaddstr(win, 3, 2, mensaje.c_str());
    mvwaddstr(win, 4, (cols - 14) / 2, "[Presione una tecla]");
    wrefresh(win);
    wgetch(win);
    delwin(win);
}

void mostrarTabla(const std::vector<std::string>& headers, const std::vector<std::vector<std::string>>& filas, const std::string& titulo) {
    if (filas.empty()) {
        mostrarMensaje("Info", "No hay datos para mostrar");
        return;
    }

    const int MAX_FILAS_PAGINA = LINES - 10;
    int pagina = 0;
    int totalPaginas = (filas.size() + MAX_FILAS_PAGINA - 1) / MAX_FILAS_PAGINA;
    
    while (true) {
        clear();
        dibujarMarco(titulo + " (Pag " + std::to_string(pagina + 1) + "/" + std::to_string(totalPaginas) + ")");

        int cols = headers.size();
        int anchoMax = 0;
        for (size_t i = 0; i < headers.size(); i++) {
            anchoMax = std::max(anchoMax, (int)headers[i].length());
        }
        for (size_t j = pagina * MAX_FILAS_PAGINA; j < std::min((size_t)(pagina + 1) * MAX_FILAS_PAGINA, filas.size()); j++) {
            for (size_t i = 0; i < filas[j].size(); i++) {
                anchoMax = std::max(anchoMax, (int)filas[j][i].length());
            }
        }
        anchoMax += 2;

        int inicioX = 1;
        int fila = 3;

        attron(COLOR_PAIR(colorSeleccion) | A_BOLD);
        for (size_t i = 0; i < headers.size(); i++) {
            mvprintw(fila, inicioX + i * anchoMax, "%-*s", anchoMax - 1, headers[i].c_str());
        }
        attroff(COLOR_PAIR(colorSeleccion) | A_BOLD);

        attron(COLOR_PAIR(colorBorde));
        mvaddch(fila + 1, 0, ACS_LTEE);
        int anchoTotal = cols * anchoMax;
        for (int x = 1; x < anchoTotal; x++) {
            mvaddch(fila + 1, x, ACS_HLINE);
        }
        mvaddch(fila + 1, anchoTotal, ACS_RTEE);
        attroff(COLOR_PAIR(colorBorde));

        fila += 2;
        int fin = std::min((pagina + 1) * MAX_FILAS_PAGINA, (int)filas.size());
        for (int j = pagina * MAX_FILAS_PAGINA; j < fin; j++) {
            for (size_t i = 0; i < filas[j].size() && i < (size_t)cols; i++) {
                mvprintw(fila, inicioX + i * anchoMax, "%-*s", anchoMax - 1, filas[j][i].c_str());
            }
            fila++;
        }

        std::string nav = "[←/→] Paginar  [Esc] Salir";
        attron(COLOR_PAIR(colorBorde));
        mvaddstr(LINES - 3, (COLS - nav.length()) / 2, nav.c_str());
        attroff(COLOR_PAIR(colorBorde));
        refresh();

        int ch = getch();
        if (ch == KEY_LEFT && pagina > 0) {
            pagina--;
        } else if (ch == KEY_RIGHT && pagina < totalPaginas - 1) {
            pagina++;
        } else if (ch == 27 || ch == KEY_ENTER) {
            break;
        }
    }
}

int selectorLista(const std::vector<std::string>& opciones, const std::string& titulo) {
    if (opciones.empty()) return -1;

    int alto = LINES;
    int ancho = COLS;
    int menuAlto = opciones.size() + 4;
    int menuAncho = titulo.length() + 10;
    for (const auto& op : opciones) {
        menuAncho = std::max(menuAncho, (int)op.length() + 6);
    }

    int inicioY = (alto - menuAlto) / 2;
    int inicioX = (ancho - menuAncho) / 2;

    WINDOW* win = newwin(menuAlto, menuAncho, inicioY, inicioX);
    keypad(win, TRUE);
    box(win, 0, 0);
    mvwaddstr(win, 1, (menuAncho - titulo.length()) / 2, titulo.c_str());

    int seleccion = 0;
    int clave = 0;

    while (true) {
        for (size_t i = 0; i < opciones.size(); i++) {
            if ((int)i == seleccion) {
                wattron(win, A_REVERSE);
            }
            mvwprintw(win, i + 3, 2, " %-*s ", menuAncho - 5, opciones[i].c_str());
            wattroff(win, A_REVERSE);
        }
        wrefresh(win);
        clave = wgetch(win);

        if (clave == KEY_UP) {
            seleccion = (seleccion - 1 + opciones.size()) % opciones.size();
        } else if (clave == KEY_DOWN) {
            seleccion = (seleccion + 1) % opciones.size();
        } else if (clave == 10 || clave == KEY_ENTER) {
            break;
        } else if (clave == 27) {
            seleccion = -1;
            break;
        }
    }

    delwin(win);
    return seleccion;
}

std::string selectorOpcion(const std::vector<std::pair<std::string, std::string>>& opciones, const std::string& titulo) {
    if (opciones.empty()) return "";

    std::vector<std::string> nombres;
    for (const auto& op : opciones) {
        nombres.push_back(op.first);
    }

    int idx = selectorLista(nombres, titulo);
    if (idx >= 0 && idx < (int)opciones.size()) {
        return opciones[idx].second;
    }
    return "";
}

int menuPrincipal() {
    std::vector<std::string> opciones = {
        "Lotes y Animales",
        "Alimentacion",
        "Ventas y Clientes",
        "Granjas y Corrales",
        "Veterinaria",
        "Reportes y Metricas",
        "Configuracion",
        "Salir"
    };

    while (true) {
        clear();
        dibujarMarco("🐔 CHICKEN CONTROL - Menu Principal");
        int sel = selectorLista(opciones, "Menu Principal");

        if (sel == -1 || sel == 7) {
            return 0;
        } else if (sel == 0) {
            menuLotes();
        } else if (sel == 1) {
            menuAlimentacion();
        } else if (sel == 2) {
            menuVentas();
        } else if (sel == 3) {
            menuGranjas();
        } else if (sel == 4) {
            menuVeterinaria();
        } else if (sel == 5) {
            menuReportes();
        } else if (sel == 6) {
            menuConfiguracion();
        }
    }
}

int menuLotes() {
    std::vector<std::string> opciones = {
        "Ver Lotes",
        "Crear Nuevo Lote",
        "Agregar Animales",
        "Registrar Muerte",
        "Registrar Sacrificio",
        "Cerrar Lote",
        "Volver"
    };

    while (true) {
        clear();
        dibujarMarco("🐔 Lotes y Animales");
        int sel = selectorLista(opciones, "Lotes y Animales");

        if (sel == -1 || sel == 6) {
            return 0;
        } else if (sel == 0) {
            auto lotes = Lotes::listar();
            if (lotes.empty()) {
                mostrarMensaje("Lotes", "No hay lotes registrados");
                continue;
            }
            std::vector<std::vector<std::string>> filas;
            for (const auto& l : lotes) {
                std::vector<std::string> fila = {
                    std::to_string(l.numero),
                    l.fecha_inicio,
                    l.activo ? "Activo" : "Cerrado",
                    l.fecha_fin.empty() ? "En curso" : l.fecha_fin
                };
                filas.push_back(fila);
            }
            mostrarTabla({"#", "Fecha Inicio", "Estado", "Fecha Fin"}, filas, "Lista de Lotes");
        } else if (sel == 1) {
            clear();
            dibujarMarco("Crear Nuevo Lote");
            int numero = inputInt("Numero de lote");
            if (numero > 0) {
                std::string fecha = fechaActual();
                int id = Lotes::crear(numero, fecha, 45);
                mostrarMensaje("Exito", ("Lote #" + std::to_string(numero) + " creado").c_str());
            }
        } else if (sel == 2) {
            clear();
            dibujarMarco("Agregar Animales");
            auto lotes = Lotes::listar();
            if (lotes.empty()) {
                mostrarMensaje("Error", "No hay lotes disponibles");
                continue;
            }
            std::vector<std::pair<std::string, std::string>> opts;
            for (const auto& l : lotes) {
                opts.push_back({"Lote #" + std::to_string(l.numero), std::to_string(l.id)});
            }
            std::string loteId = selectorOpcion(opts, "Seleccionar Lote");
            if (loteId.empty()) continue;

            int cantidad = inputInt("Cantidad de animales");
            double precio = inputDouble("Precio por unidad");
            double peso = inputDouble("Peso promedio (kg)");

            int id = Animales::agregar(std::stoi(loteId), cantidad, precio, peso, fechaActual());
            mostrarMensaje("Exito", (std::to_string(cantidad) + " animales agregados").c_str());
        } else if (sel == 3) {
            clear();
            dibujarMarco("Registrar Muerte");
            auto lotes = Lotes::listar();
            if (lotes.empty()) {
                mostrarMensaje("Error", "No hay lotes");
                continue;
            }
            std::vector<std::pair<std::string, std::string>> opts;
            for (const auto& l : lotes) {
                opts.push_back({"Lote #" + std::to_string(l.numero), std::to_string(l.id)});
            }
            std::string loteId = selectorOpcion(opts, "Seleccionar Lote");
            if (loteId.empty()) continue;

            int cantidad = inputInt("Cantidad de muertes");
            std::vector<std::pair<std::string, std::string>> causas = {
                {"Aplastamiento", "Aplastamiento"},
                {"Ahogamiento", "Ahogamiento"},
                {"Gases toxicos", "Gases_toxicos"},
                {"Enfermedad", "Enfermdad"},
                {"Frio", "Frio"},
                {"Calor", "Calor"},
                {"Otro", "Otro"}
            };
            std::string causa = selectorOpcion(causas, "Seleccionar Causa");
            if (causa.empty()) continue;

            CausaMuerte c = stringToCausaMuerte(causa);
            Animales::registrarMuerte(std::stoi(loteId), c, cantidad, fechaActual());
            mostrarMensaje("Exito", "Muerte registrada");
        } else if (sel == 4) {
            clear();
            dibujarMarco("Registrar Sacrificio");
            auto lotes = Lotes::listar();
            if (lotes.empty()) {
                mostrarMensaje("Error", "No hay lotes");
                continue;
            }
            std::vector<std::pair<std::string, std::string>> opts;
            for (const auto& l : lotes) {
                opts.push_back({"Lote #" + std::to_string(l.numero), std::to_string(l.id)});
            }
            std::string loteId = selectorOpcion(opts, "Seleccionar Lote");
            if (loteId.empty()) continue;

            int cantidad = inputInt("Cantidad sacrificada");
            double pesoTotal = inputDouble("Peso total (kg)");

            Animales::registrarSacrificio(std::stoi(loteId), cantidad, pesoTotal, fechaActual());
            mostrarMensaje("Exito", "Sacrificio registrado");
        } else if (sel == 5) {
            clear();
            dibujarMarco("Cerrar Lote");
            auto lotes = Lotes::listar();
            std::vector<std::pair<std::string, std::string>> opts;
            for (const auto& l : lotes) {
                if (l.activo) {
                    opts.push_back({"Lote #" + std::to_string(l.numero), std::to_string(l.id)});
                }
            }
            if (opts.empty()) {
                mostrarMensaje("Info", "No hay lotes activos");
                continue;
            }
            std::string loteId = selectorOpcion(opts, "Seleccionar Lote");
            if (loteId.empty()) continue;

            if (confirmar("Cerrar el lote?")) {
                Lotes::cerrar(std::stoi(loteId), fechaActual());
                mostrarMensaje("Exito", "Lote cerrado");
            }
        }
    }
}

int menuAlimentacion() {
    std::vector<std::string> opciones = {
        "Ver Inventario",
        "Agregar Alimento",
        "Registrar Consumo",
        "Volver"
    };

    while (true) {
        clear();
        dibujarMarco("🍞 Alimentacion");
        int sel = selectorLista(opciones, "Alimentacion");

        if (sel == -1 || sel == 3) {
            return 0;
        } else if (sel == 0) {
            clear();
            dibujarMarco("Inventario de Alimentos");
            std::vector<std::vector<std::string>> filas;
            std::vector<Alimento> alimentos = Alimentacion::listar();
            for (const auto& a : alimentos) {
                std::vector<std::string> fila = {
                    a.nombre,
                    faseToString(a.fase),
                    std::to_string(a.inventario),
                    std::to_string(a.precio_unitario)
                };
                filas.push_back(fila);
            }
            if (filas.empty()) {
                mostrarMensaje("Inventario", "No hay alimentos");
            } else {
                mostrarTabla({"Nombre", "Fase", "Stock", "Precio"}, filas, "Inventario de Alimentos");
            }
        } else if (sel == 1) {
            clear();
            dibujarMarco("Agregar Alimento");
            std::string nombre = inputString("Nombre del alimento", 30);
            if (nombre.empty()) continue;

            std::vector<std::pair<std::string, std::string>> fases = {
                {"Bebe", "Bebe"},
                {"Intermedio", "Intermedio"},
                {"Grande", "Grande"}
            };
            std::string faseStr = selectorOpcion(fases, "Seleccionar Fase");
            if (faseStr.empty()) continue;

            Fase fase = stringToFase(faseStr);
            int inventario = inputInt("Cantidad en inventario");
            double precio = inputDouble("Precio por saco");

            AnalisisNutricional anal;
            int id = Alimentacion::agregar(nombre, "", fase, anal, 40, precio, inventario, 0, 999);
            mostrarMensaje("Exito", ("Alimento '" + nombre + "' agregado").c_str());
        } else if (sel == 2) {
            clear();
            dibujarMarco("Registrar Consumo");
            auto lotes = Lotes::listar();
            if (lotes.empty()) {
                mostrarMensaje("Error", "No hay lotes");
                continue;
            }
            std::vector<std::pair<std::string, std::string>> optsLote;
            for (const auto& l : lotes) {
                optsLote.push_back({"Lote #" + std::to_string(l.numero), std::to_string(l.id)});
            }
            std::string loteId = selectorOpcion(optsLote, "Seleccionar Lote");
            if (loteId.empty()) continue;

            std::vector<Alimento> alimentos = Alimentacion::listar();
            if (alimentos.empty()) {
                mostrarMensaje("Error", "No hay alimentos");
                continue;
            }
            std::vector<std::pair<std::string, std::string>> optsAlim;
            for (const auto& a : alimentos) {
                optsAlim.push_back({a.nombre, a.nombre});
            }
            std::string nombreAlim = selectorOpcion(optsAlim, "Seleccionar Alimento");
            if (nombreAlim.empty()) continue;

            int cantidad = inputInt("Cantidad de sacos");
            Alimento* a = Alimentacion::obtenerPorNombre(nombreAlim);
            if (a) {
                Alimentacion::registrarConsumo(std::stoi(loteId), a->id, cantidad);
                mostrarMensaje("Exito", "Consumo registrado");
            }
        }
    }
}

int menuVentas() {
    std::vector<std::string> opciones = {
        "Nueva Venta",
        "Ver Ventas",
        "Ver Deudas",
        "Agregar Cliente",
        "Listar Clientes",
        "Volver"
    };

    while (true) {
        clear();
        dibujarMarco("💰 Ventas y Clientes");
        int sel = selectorLista(opciones, "Ventas y Clientes");

        if (sel == -1 || sel == 5) {
            return 0;
        } else if (sel == 0) {
            clear();
            dibujarMarco("Nueva Venta");
            auto lotes = Lotes::listar();
            if (lotes.empty()) {
                mostrarMensaje("Error", "No hay lotes");
                continue;
            }
            std::vector<std::pair<std::string, std::string>> optsLote;
            for (const auto& l : lotes) {
                optsLote.push_back({"Lote #" + std::to_string(l.numero), std::to_string(l.id)});
            }
            std::string loteId = selectorOpcion(optsLote, "Seleccionar Lote");
            if (loteId.empty()) continue;

            auto clientes = Ventas::listarClientes();
            if (clientes.empty()) {
                mostrarMensaje("Error", "No hay clientes. Agregue uno primero.");
                continue;
            }
            std::vector<std::pair<std::string, std::string>> optsCliente;
            for (const auto& c : clientes) {
                optsCliente.push_back({c.nombre, std::to_string(c.id)});
            }
            std::string clienteId = selectorOpcion(optsCliente, "Seleccionar Cliente");
            if (clienteId.empty()) continue;

            int cantidad = inputInt("Cantidad de pollos");
            double precio = inputDouble("Precio por pollo");

            std::vector<double> pesos;
            for (int i = 0; i < cantidad; i++) {
                double p = inputDouble(("Peso pollo " + std::to_string(i + 1)).c_str());
                pesos.push_back(p);
            }

            std::vector<std::string> metodos = {"efectivo"};
            int id = Ventas::registrarVenta(std::stoi(loteId), std::stoi(clienteId), pesos, precio, metodos, fechaActual());
            mostrarMensaje("Exito", ("Venta #" + std::to_string(id) + " registrada").c_str());
        } else if (sel == 1) {
            clear();
            dibujarMarco("Lista de Ventas");
            auto ventas = Ventas::listar();
            if (ventas.empty()) {
                mostrarMensaje("Ventas", "No hay ventas");
            } else {
                std::vector<std::vector<std::string>> filas;
                for (const auto& v : ventas) {
                    std::vector<std::string> fila = {
                        std::to_string(v.id),
                        v.nombre_cliente,
                        std::to_string(v.cantidad),
                        std::to_string(v.total),
                        v.pagada ? "Pagada" : "Pendiente"
                    };
                    filas.push_back(fila);
                }
                mostrarTabla({"ID", "Cliente", "Cant", "Total", "Estado"}, filas, "Lista de Ventas");
            }
        } else if (sel == 2) {
            clear();
            dibujarMarco("Deudas Pendientes");
            Reportes::reporteDeudas();
            esperarTecla("Presione una tecla...");
        } else if (sel == 3) {
            clear();
            dibujarMarco("Agregar Cliente");
            std::string nombre = inputString("Nombre del cliente", 40);
            std::string telefono = inputString("Telefono", 20);
            std::string referencia = inputString("Referencia", 40);

            if (!nombre.empty()) {
                int id = Ventas::agregarCliente(nombre, telefono, referencia);
                mostrarMensaje("Exito", ("Cliente '" + nombre + "' agregado").c_str());
            }
        } else if (sel == 4) {
            clear();
            dibujarMarco("Lista de Clientes");
            auto clientes = Ventas::listarClientes();
            if (clientes.empty()) {
                mostrarMensaje("Clientes", "No hay clientes");
            } else {
                std::vector<std::vector<std::string>> filas;
                for (const auto& c : clientes) {
                    std::vector<std::string> fila = {
                        std::to_string(c.id),
                        c.nombre,
                        c.telefono,
                        c.referencia
                    };
                    filas.push_back(fila);
                }
                mostrarTabla({"ID", "Nombre", "Telefono", "Referencia"}, filas, "Lista de Clientes");
            }
        }
    }
}

int menuGranjas() {
    std::vector<std::string> opciones = {
        "Listar Granjas",
        "Crear Granja",
        "Listar Corrales",
        "Crear Corral",
        "Volver"
    };

    while (true) {
        clear();
        dibujarMarco("🏠 Granjas y Corrales");
        int sel = selectorLista(opciones, "Granjas y Corrales");

        if (sel == -1 || sel == 4) {
            return 0;
        } else if (sel == 0) {
            clear();
            dibujarMarco("Lista de Granjas");
            auto granjas = Granjas::listar();
            if (granjas.empty()) {
                mostrarMensaje("Granjas", "No hay granjas");
            } else {
                std::vector<std::vector<std::string>> filas;
                for (const auto& g : granjas) {
                    std::vector<std::string> fila = {
                        std::to_string(g.id),
                        g.nombre,
                        g.nombre_tipo,
                        g.ubicacion
                    };
                    filas.push_back(fila);
                }
                mostrarTabla({"ID", "Nombre", "Tipo", "Ubicacion"}, filas, "Lista de Granjas");
            }
        } else if (sel == 1) {
            clear();
            dibujarMarco("Crear Granja");
            std::string nombre = inputString("Nombre de la granja", 30);
            std::vector<std::pair<std::string, std::string>> tipos = {
                {"Terrestre", "1"},
                {"Aerea", "2"},
                {"Maritima", "3"}
            };
            std::string tipoId = selectorOpcion(tipos, "Seleccionar Tipo");
            if (tipoId.empty() || nombre.empty()) continue;

            std::string ubicacion = inputString("Ubicacion", 40);
            int id = Granjas::crear(nombre, std::stoi(tipoId), ubicacion, "", 10);
            mostrarMensaje("Exito", ("Granja '" + nombre + "' creada").c_str());
        } else if (sel == 2) {
            clear();
            dibujarMarco("Lista de Corrales");
            auto corrales = Corrales::listarTodos();
            if (corrales.empty()) {
                mostrarMensaje("Corrales", "No hay corrales");
            } else {
                std::vector<std::vector<std::string>> filas;
                for (const auto& c : corrales) {
                    std::vector<std::string> fila = {
                        std::to_string(c.id),
                        c.nombre,
                        std::to_string(c.capacidad_maxima),
                        std::to_string(c.cantidad_actual)
                    };
                    filas.push_back(fila);
                }
                mostrarTabla({"ID", "Nombre", "Capacidad", "Ocupados"}, filas, "Lista de Corrales");
            }
        } else if (sel == 3) {
            clear();
            dibujarMarco("Crear Corral");
            auto granjas = Granjas::listar();
            if (granjas.empty()) {
                mostrarMensaje("Error", "No hay granjas. Cree una primero.");
                continue;
            }
            std::vector<std::pair<std::string, std::string>> optsGranja;
            for (const auto& g : granjas) {
                optsGranja.push_back({g.nombre, std::to_string(g.id)});
            }
            std::string granjaId = selectorOpcion(optsGranja, "Seleccionar Granja");
            if (granjaId.empty()) continue;

            std::string nombre = inputString("Nombre del corral", 30);
            int numero = inputInt("Numero de corral");
            int capacidad = inputInt("Capacidad maxima");

            int id = Corrales::crear(std::stoi(granjaId), numero, nombre, 10.0, 5.0, 3.0,
                "zinc", "completa", "natural", "norte", "bloques", "canal", "led",
                true, false, "ninguna", true, capacidad, 10);
            mostrarMensaje("Exito", ("Corral '" + nombre + "' creado").c_str());
        }
    }
}

int menuVeterinaria() {
    std::vector<std::string> opciones = {
        "Listar Vacunas",
        "Agregar Vacuna",
        "Listar Medicamentos",
        "Agregar Medicamento",
        "Volver"
    };

    while (true) {
        clear();
        dibujarMarco("🏥 Veterinaria");
        int sel = selectorLista(opciones, "Veterinaria");

        if (sel == -1 || sel == 4) {
            return 0;
        } else if (sel == 0) {
            clear();
            dibujarMarco("Lista de Vacunas");
            std::vector<std::vector<std::string>> filas;
            std::vector<Vacuna> vacunas = Veterinaria::listarVacunas();
            for (const auto& v : vacunas) {
                std::vector<std::string> fila = {
                    v.nombre,
                    "Dia " + std::to_string(v.aplicacion_dias),
                    std::to_string(v.costo)
                };
                filas.push_back(fila);
            }
            if (filas.empty()) {
                mostrarMensaje("Vacunas", "No hay vacunas");
            } else {
                mostrarTabla({"Nombre", "Aplicar", "Costo"}, filas, "Lista de Vacunas");
            }
        } else if (sel == 1) {
            clear();
            dibujarMarco("Agregar Vacuna");
            std::string nombre = inputString("Nombre de la vacuna", 30);
            int dias = inputInt("Dia de aplicacion");
            double costo = inputDouble("Costo");

            if (!nombre.empty()) {
                int id = Veterinaria::agregarVacuna(nombre, dias, costo, 0, "");
                mostrarMensaje("Exito", ("Vacuna '" + nombre + "' agregada").c_str());
            }
        } else if (sel == 2) {
            clear();
            dibujarMarco("Lista de Medicamentos");
            std::vector<std::vector<std::string>> filas;
            std::vector<Medicamento> medicamentos = Veterinaria::listarMedicamentos();
            for (const auto& m : medicamentos) {
                std::vector<std::string> fila = {
                    m.nombre,
                    m.categoria,
                    std::to_string(m.stock),
                    std::to_string(m.precio)
                };
                filas.push_back(fila);
            }
            if (filas.empty()) {
                mostrarMensaje("Medicamentos", "No hay medicamentos");
            } else {
                mostrarTabla({"Nombre", "Categoria", "Stock", "Precio"}, filas, "Lista de Medicamentos");
            }
        } else if (sel == 3) {
            clear();
            dibujarMarco("Agregar Medicamento");
            std::string nombre = inputString("Nombre del medicamento", 30);
            std::string dosis = inputString("Dosis", 30);
            double precio = inputDouble("Precio");
            int stock = inputInt("Stock inicial");

            if (!nombre.empty()) {
                int id = Veterinaria::agregarMedicamento(nombre, "", dosis, precio, 0, "general", stock);
                mostrarMensaje("Exito", ("Medicamento '" + nombre + "' agregado").c_str());
            }
        }
    }
}

int menuReportes() {
    std::vector<std::string> opciones = {
        "Dashboard General",
        "Metricas por Lote",
        "Reporte Financiero",
        "Reporte de Inventario",
        "Reporte de Deudas",
        "Volver"
    };

    while (true) {
        clear();
        dibujarMarco("📊 Reportes y Metricas");
        int sel = selectorLista(opciones, "Reportes y Metricas");

        if (sel == -1 || sel == 5) {
            return 0;
        } else if (sel == 0) {
            clear();
            dibujarMarco("Dashboard General");
            Metricas::dashboardGeneral();
            esperarTecla("Presione una tecla...");
        } else if (sel == 1) {
            clear();
            dibujarMarco("Metricas por Lote");
            auto lotes = Lotes::listar();
            if (lotes.empty()) {
                mostrarMensaje("Lotes", "No hay lotes");
                continue;
            }
            std::vector<std::pair<std::string, std::string>> opts;
            for (const auto& l : lotes) {
                opts.push_back({"Lote #" + std::to_string(l.numero), std::to_string(l.id)});
            }
            std::string loteId = selectorOpcion(opts, "Seleccionar Lote");
            if (loteId.empty()) continue;

            Metricas::imprimirMetricasLote(std::stoi(loteId));
            esperarTecla("Presione una tecla...");
        } else if (sel == 2) {
            clear();
            dibujarMarco("Reporte Financiero");
            Reportes::reporteFinanciero();
            esperarTecla("Presione una tecla...");
        } else if (sel == 3) {
            clear();
            dibujarMarco("Reporte de Inventario");
            Reportes::reporteInventario();
            esperarTecla("Presione una tecla...");
        } else if (sel == 4) {
            clear();
            dibujarMarco("Reporte de Deudas");
            Reportes::reporteDeudas();
            esperarTecla("Presione una tecla...");
        }
    }
}

int menuConfiguracion() {
    std::vector<std::string> opciones = {
        "Ver Configuracion",
        "Actualizar Precio Dolar",
        "Actualizar Precio KG VES",
        "Exportar Datos (JSON)",
        "Importar Datos (JSON)",
        "Optimizar Base de Datos",
        "Cargar Datos de Ejemplo",
        "Limpiar Base de Datos",
        "Volver"
    };

    while (true) {
        clear();
        dibujarMarco("⚙️ Configuracion");
        int sel = selectorLista(opciones, "Configuracion");

        if (sel == -1 || sel == 8) {
            return 0;
        } else if (sel == 0) {
            clear();
            dibujarMarco("Configuracion Actual");
            BaseDatos* db = BaseDatos::getInstancia();
            Configuracion c = db->getConfiguracion();
            std::ostringstream oss;
            oss << "Precio Dolar: Bs. " << std::fixed << std::setprecision(2) << c.precio_dolar << "\n";
            oss << "Precio KG VES: Bs. " << std::fixed << std::setprecision(2) << c.precio_kg_ves;
            mostrarMensaje("Configuracion", oss.str());
        } else if (sel == 1) {
            clear();
            dibujarMarco("Actualizar Precio Dolar");
            double nuevoPrecio = inputDouble("Nuevo precio del dolar", 1.0, 100000.0);
            if (nuevoPrecio > 0) {
                BaseDatos* db = BaseDatos::getInstancia();
                db->setConfigValue("precio_dolar", std::to_string(nuevoPrecio));
                mostrarMensaje("Exito", ("Precio actualizado a Bs. " + std::to_string(nuevoPrecio)).c_str());
            }
        } else if (sel == 2) {
            clear();
            dibujarMarco("Actualizar Precio KG VES");
            double nuevoPrecio = inputDouble("Nuevo precio por kg", 1.0, 100000.0);
            if (nuevoPrecio > 0) {
                BaseDatos* db = BaseDatos::getInstancia();
                db->setConfigValue("precio_kg_ves", std::to_string(nuevoPrecio));
                mostrarMensaje("Exito", ("Precio actualizado a Bs. " + std::to_string(nuevoPrecio)).c_str());
            }
        } else if (sel == 3) {
            clear();
            dibujarMarco("Exportar Datos");
            BaseDatos* db = BaseDatos::getInstancia();
            std::string json = db->exportarJSON();
            std::ofstream out("datos/backup.json");
            if (out.is_open()) {
                out << json;
                out.close();
                mostrarMensaje("Exito", "Datos exportados a datos/backup.json");
            } else {
                mostrarMensaje("Error", "No se pudo crear el archivo");
            }
        } else if (sel == 4) {
            if (confirmar("Esto sobrescribira datos existentes. Continuar?")) {
                std::ifstream in("datos/backup.json");
                if (in.is_open()) {
                    std::string json((std::istreambuf_iterator<char>(in)),
                                     std::istreambuf_iterator<char>());
                    in.close();
                    BaseDatos* db = BaseDatos::getInstancia();
                    db->importarJSON(json);
                    mostrarMensaje("Exito", "Datos importados");
                } else {
                    mostrarMensaje("Error", "Archivo no encontrado");
                }
            }
        } else if (sel == 5) {
            clear();
            dibujarMarco("Optimizar Base de Datos");
            if (confirmar("Optimizar la base de datos (VACUUM)?")) {
                BaseDatos* db = BaseDatos::getInstancia();
                db->vacuum();
                mostrarMensaje("Exito", "Base de datos optimizada");
            }
        } else if (sel == 6) {
            if (confirmar("Cargar datos de ejemplo?")) {
                Herramientas::cargarEjemplo();
                Alimentacion::cargarEjemplo();
                Ventas::cargarClientesEjemplo();
                Inversores::cargarEjemplo();
                int prox = Lotes::ultimoNumero() + 1;
                int lote_id = Lotes::crear(prox, "2022-02-02", 45);
                Animales::agregar(lote_id, 70, 1.0, 2.0, "2022-02-02");
                Animales::registrarMuerte(lote_id, CausaMuerte::Aplastamiento, 4, "2022-02-10");
                mostrarMensaje("Exito", "Datos de ejemplo cargados");
            }
        } else if (sel == 7) {
            if (confirmar("Esto eliminara TODOS los datos. Continuar?")) {
                system("rm -f datos/granja.db");
                BaseDatos* db = BaseDatos::getInstancia();
                db->inicializar();
                mostrarMensaje("Exito", "Base de datos limpiada");
            }
        }
    }
}

bool validarNumeroPositivo(const std::string& s) {
    if (s.empty()) return false;
    for (char c : s) {
        if (!isdigit(c)) return false;
    }
    return true;
}

bool validarDecimal(const std::string& s) {
    if (s.empty()) return false;
    bool tienePunto = false;
    for (size_t i = 0; i < s.length(); i++) {
        char c = s[i];
        if (c == '.') {
            if (tienePunto) return false;
            tienePunto = true;
        } else if (!isdigit(c)) {
            return false;
        }
    }
    return true;
}

bool validarTelefono(const std::string& s) {
    if (s.empty()) return true;
    if (s.length() < 10) return false;
    for (char c : s) {
        if (!isdigit(c) && c != '-' && c != ' ') return false;
    }
    return true;
}

bool validarFecha(const std::string& s) {
    if (s.length() != 10) return false;
    if (s[4] != '-' || s[7] != '-') return false;
    std::string anio = s.substr(0, 4);
    std::string mes = s.substr(5, 2);
    std::string dia = s.substr(8, 2);
    if (!validarNumeroPositivo(anio) || !validarNumeroPositivo(mes) || !validarNumeroPositivo(dia)) {
        return false;
    }
    int m = std::stoi(mes);
    int d = std::stoi(dia);
    return (m >= 1 && m <= 12 && d >= 1 && d <= 31);
}

}