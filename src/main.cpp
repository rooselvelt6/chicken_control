#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <iomanip>
#include <fstream>
#include "../include/modelos.h"
#include "../include/utils.h"
#include "../include/base_datos.h"
#include "../include/lotes.h"
#include "../include/animales.h"
#include "../include/alimentacion.h"
#include "../include/ventas.h"
#include "../include/inversores.h"
#include "../include/herramientas.h"
#include "../include/reportes.h"
#include "../include/granjas.h"
#include "../include/corrales.h"
#include "../include/proveedores.h"
#include "../include/veterinaria.h"
#include "../include/pesaje.h"
#include "../include/metricas.h"
#include "../include/ui.h"
#include "../include/alertas.h"
#include "../include/facturacion.h"
#include "../include/contenedores.h"
#include "../include/beneficio.h"

void mostrarAyuda() {
    std::cout << R"(
Sistema de Control de Granja de Pollos - C++
==============================================

Uso: ./granja <comando> [opciones]

Comandos principales:
  ayuda             - Mostrar esta ayuda
  ejemplo           - Cargar datos de ejemplo
  ui                - Iniciar interfaz grafica TUI (menu interactivo)
  exportar          - Exportar base de datos a JSON
  importar <archivo> - Importar datos desde archivo JSON
  optimizar         - Optimizar base de datos (VACUUM)
  alertas           - Ver alertas del sistema
  factura generar <venta> - Generar factura de venta
  factura ver <id>       - Ver factura
  factura listar          - Listar facturas
  factura anular <id>    - Anular factura
  contenedor crear <nombre> <ubicacion> <capacidad> [temp]
  contenedor listar       - Listar contenedores
  contenedor productos    - Ver productos en contenedores
  beneficio registrar <lote> <cant> <peso> <tipo> <operador> <cedula>
  beneficio listar        - Listar registros de beneficio
  beneficio stats         - Ver estadisticas

  config            - Configurar parámetros
    set-precio <valor>   Establecer precio por kg (USD)
    get-precio          Mostrar precio actual por kg

  lote              - Gestionar lotes
    nuevo <num> [--fecha YYYY-MM-DD] [--duracion dias]
    listar              Mostrar todos los lotes
    cerrar <id> [--fecha YYYY-MM-DD]

  animal            - Gestionar animales
    agregar <lote> <cant> [--precio P] [--peso KG]
    listar [--lote ID]
    muerte <lote> <causa> <cant> [--fecha YYYY-MM-DD]
    sacrificar <lote> <cant> <peso_total> [--fecha YYYY-MM-DD]

  alimento          - Gestionar alimentos
    agregar <nombre> [--fase fase] [--precio P] [--inventario N]
    consumir <lote> <nombre> <cant>
    inventario         Mostrar inventario

  cliente           - Gestionar clientes
    agregar <nombre> [--telefono T] [--referencia R]
    listar              Mostrar clientes

  venta             - Gestionar ventas
    nueva <lote> <cliente> <pesos> [--precio P] [--metodos m1,m2]
    pagar <id> <monto>
    deudas             Mostrar deudas pendientes

  inversor          - Gestionar inversores
    agregar <nombre> <cantidad>
    listar              Mostrar inversores

  herramienta       - Gestionar herramientas
    agregar <nombre> <cant> [--precio P]
    listar              Mostrar herramientas

  reporte            - Ver reportes
    lote <id>         Reporte de un lote
    financiero        Reporte financiero
    inventario        Inventario completo
    deudas            Deudas pendientes
    completo          Reporte completo

Ejemplos:
  ./granja ayuda
  ./granja ejemplo
  ./granja config set-precio 2.50
  ./granja config get-precio
  ./granja lote nuevo 1 --fecha 2024-01-01
  ./granja animal agregar 1 100 --precio 1.0
  ./granja animal muerte 1 aplastamiento 5
  ./granja animal sacrificar 1 10 22.5
  ./granja alimento agregar "Preinicial 1" --fase bebe --precio 37 --inventario 50
  ./granja alimento consumir 1 "Preinicial 1" 10
  ./granja cliente agregar "Cliente1" --telefono 04120000000
  ./granja venta nueva 1 "Cliente1" "2.2,2.5,2.4" --precio 34500
  ./granja venta pagar 1 34500
  ./granja inversor agregar "Inversor1" 100
  ./granja herramienta agregar "Tornillos" 20 --precio 0.10
  ./granja reporte lote 1
  ./granja reporte financiero
  ./granja reporte inventario
  ./granja reporte completo
)";
}

std::vector<std::string> splitArgs(const std::string& s) {
    std::vector<std::string> args;
    std::istringstream iss(s);
    std::string arg;
    while (iss >> arg) args.push_back(arg);
    return args;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        mostrarAyuda();
        return 0;
    }

    BaseDatos* db = BaseDatos::getInstancia();
    system("mkdir -p datos");
    db->inicializar();

    std::string cmd = argv[1];

    if (cmd == "ayuda") {
        mostrarAyuda();
    }
    else if (cmd == "ui") {
        UI::inicializar();
        UI::menuPrincipal();
        UI::finalizar();
    }
    else if (cmd == "ejemplo") {
        std::cout << "Cargando datos de ejemplo..." << std::endl;
        Herramientas::cargarEjemplo();
        Alimentacion::cargarEjemplo();
        Ventas::cargarClientesEjemplo();
        Inversores::cargarEjemplo();
        int prox = Lotes::ultimoNumero() + 1;
        int lote_id = Lotes::crear(prox, "2022-02-02", 45);
        Animales::agregar(lote_id, 70, 1.0, 2.0, "2022-02-02");
        Animales::registrarMuerte(lote_id, CausaMuerte::Aplastamiento, 4, "2022-02-10");
        Animales::registrarMuerte(lote_id, CausaMuerte::Ahogamiento, 2, "2022-02-15");
        Animales::registrarMuerte(lote_id, CausaMuerte::Gases_toxicos, 1, "2022-02-20");
        Animales::registrarMuerte(lote_id, CausaMuerte::Infarto, 2, "2022-03-01");
        Animales::registrarMuerte(lote_id, CausaMuerte::Retencion_liquidos, 2, "2022-03-10");
        std::cout << "Lote #" << prox << " creado con animales" << std::endl;
        std::cout << "Datos de ejemplo cargados correctamente" << std::endl;
    }
    else if (cmd == "config") {
        if (argc < 3) {
            std::cout << "Uso: config set-precio <valor> | get-precio" << std::endl;
            return 0;
        }
        std::string sub = argv[2];
        if (sub == "set-precio" && argc > 3) {
            try {
                double valor = std::stod(argv[3]);
                if (valor <= 0) { std::cout << "Error: El precio debe ser mayor a 0" << std::endl; return 1; }
                db->setConfigValue("precio_kg", std::to_string(valor));
                std::cout << "Precio por kg establecido: USD " << std::fixed << std::setprecision(2) << valor << std::endl;
            } catch (...) {
                std::cout << "Error: Valor inválido. Use un número válido." << std::endl;
                return 1;
            }
        }
        else if (sub == "get-precio") {
            Configuracion c = db->getConfiguracion();
            std::cout << "Precio por kg actual: USD " << std::fixed << std::setprecision(2) << c.precio_kg << std::endl;
        }
    }
    else if (cmd == "lote") {
        if (argc < 3) {
            std::cout << "Uso: lote nuevo <num> | listar | cerrar <id>" << std::endl;
            return 0;
        }
        std::string sub = argv[2];
        if (sub == "nuevo" && argc > 3) {
            int numero = parsearIdSeguro(argv[3]);
            if (numero <= 0) { std::cout << "Error: Número de lote inválido" << std::endl; return 1; }
            std::string fecha = fechaActual();
            int duracion = 45;
            for (int i = 4; i < argc; i++) {
                std::string a = argv[i];
                if (a == "--fecha" && i+1 < argc) fecha = argv[++i];
                if (a == "--duracion" && i+1 < argc) duracion = parsearIdSeguro(argv[++i]);
            }
            int id = Lotes::crear(numero, fecha, duracion);
            std::cout << "Lote #" << numero << " creado exitosamente (ID: " << id << ")" << std::endl;
        }
        else if (sub == "listar") {
            for (auto& l : Lotes::listar()) Lotes::imprimirLote(l);
        }
        else if (sub == "cerrar" && argc > 3) {
            int id = std::stoi(argv[3]);
            std::string fecha = fechaActual();
            Lotes::cerrar(id, fecha);
            std::cout << "Lote cerrado exitosamente" << std::endl;
        }
    }
    else if (cmd == "animal") {
        if (argc < 3) {
            std::cout << "Uso: animal agregar | listar | muerte | sacrificar" << std::endl;
            return 0;
        }
        std::string sub = argv[2];
        if (sub == "agregar" && argc > 4) {
            int lote = std::stoi(argv[3]);
            int cantidad = std::stoi(argv[4]);
            double precio = 1.0, peso = 2.0;
            for (int i = 5; i < argc; i++) {
                std::string a = argv[i];
                if (a == "--precio" && i+1 < argc) precio = std::stod(argv[++i]);
                if (a == "--peso" && i+1 < argc) peso = std::stod(argv[++i]);
            }
            int id = Animales::agregar(lote, cantidad, precio, peso, fechaActual());
            std::cout << "Agregados " << cantidad << " animales al lote " << lote << " (ID: " << id << ")" << std::endl;
        }
        else if (sub == "listar") {
            int lote_id = 0;
            for (int i = 3; i < argc; i++) {
                std::string a = argv[i];
                if (a == "--lote" && i+1 < argc) lote_id = std::stoi(argv[++i]);
            }
            if (lote_id > 0) {
                for (auto& a : Animales::listarPorLote(lote_id)) {
                    std::cout << "  ID: " << a.id << " - Cantidad: " << a.cantidad << " - Fase: " << faseToString(a.fase) << std::endl;
                }
            } else {
                for (auto& l : Lotes::listar()) {
                    std::cout << "Lote #" << l.numero << ":" << std::endl;
                    for (auto& a : Animales::listarPorLote(l.id)) {
                        std::cout << "  ID: " << a.id << " - Cantidad: " << a.cantidad << " - Fase: " << faseToString(a.fase) << std::endl;
                    }
                }
            }
        }
        else if (sub == "muerte" && argc > 5) {
            int lote = std::stoi(argv[3]);
            std::string causa_str = argv[4];
            int cantidad = std::stoi(argv[5]);
            CausaMuerte causa = stringToCausaMuerte(causa_str);
            int id = Animales::registrarMuerte(lote, causa, cantidad, fechaActual());
            std::cout << "Registradas " << cantidad << " muertes por '" << causa_str << "' (ID: " << id << ")" << std::endl;
        }
        else if (sub == "sacrificar" && argc > 5) {
            int lote = std::stoi(argv[3]);
            int cantidad = std::stoi(argv[4]);
            double peso = std::stod(argv[5]);
            int id = Animales::registrarSacrificio(lote, cantidad, peso, fechaActual());
            std::cout << "Registrados " << cantidad << " sacrificios (" << peso << " kg) (ID: " << id << ")" << std::endl;
        }
    }
    else if (cmd == "alimento") {
        if (argc < 3) {
            std::cout << "Uso: alimento agregar | consumir | inventario" << std::endl;
            return 0;
        }
        std::string sub = argv[2];
        if (sub == "agregar" && argc > 3) {
            std::string nombre = argv[3];
            Fase fase = Fase::Desconocido;
            double precio = 0.0;
            int inventario = 0;
            for (int i = 4; i < argc; i++) {
                std::string a = argv[i];
                if (a == "--fase" && i+1 < argc) fase = stringToFase(argv[++i]);
                if (a == "--precio" && i+1 < argc) precio = std::stod(argv[++i]);
                if (a == "--inventario" && i+1 < argc) inventario = std::stoi(argv[++i]);
            }
            AnalisisNutricional anal;
            int id = Alimentacion::agregar(nombre, "", fase, anal, 40, precio, inventario, 0, 999);
            std::cout << "Alimento '" << nombre << "' agregado (ID: " << id << ")" << std::endl;
        }
        else if (sub == "consumir" && argc > 5) {
            int lote = std::stoi(argv[3]);
            std::string nombre = argv[4];
            int cantidad = std::stoi(argv[5]);
            Alimento* a = Alimentacion::obtenerPorNombre(nombre);
            if (a) {
                int id = Alimentacion::registrarConsumo(lote, a->id, cantidad);
                std::cout << "Registrado consumo de " << cantidad << " sacks de '" << nombre << "' (ID: " << id << ")" << std::endl;
            } else {
                std::cout << "Alimento '" << nombre << "' no encontrado" << std::endl;
            }
        }
        else if (sub == "inventario") {
            Alimentacion::imprimirInventario();
        }
    }
    else if (cmd == "cliente") {
        if (argc < 3) {
            std::cout << "Uso: cliente agregar | listar" << std::endl;
            return 0;
        }
        std::string sub = argv[2];
        if (sub == "agregar" && argc > 3) {
            std::string nombre = argv[3];
            std::string telefono, referencia;
            for (int i = 4; i < argc; i++) {
                std::string a = argv[i];
                if (a == "--telefono" && i+1 < argc) telefono = argv[++i];
                if (a == "--referencia" && i+1 < argc) referencia = argv[++i];
            }
            int id = Ventas::agregarCliente(nombre, telefono, referencia);
            std::cout << "Cliente '" << nombre << "' agregado (ID: " << id << ")" << std::endl;
        }
        else if (sub == "listar") {
            for (auto& c : Ventas::listarClientes()) {
                std::cout << "  " << c.id << ": " << c.nombre << std::endl;
            }
        }
    }
    else if (cmd == "venta") {
        if (argc < 3) {
            std::cout << "Uso: venta nueva | pagar | deudas" << std::endl;
            return 0;
        }
        std::string sub = argv[2];
        if (sub == "nueva" && argc > 5) {
            int lote = std::stoi(argv[3]);
            std::string cliente = argv[4];
            std::string pesos_str = argv[5];
            double precio = 34500.0;
            for (int i = 6; i < argc; i++) {
                std::string a = argv[i];
                if (a == "--precio" && i+1 < argc) precio = std::stod(argv[++i]);
            }
            Cliente* c = Ventas::obtenerClientePorNombre(cliente);
            if (c) {
                std::vector<float> pesos_vec = stringToFloatList(pesos_str);
                std::vector<double> pesos(pesos_vec.begin(), pesos_vec.end());
                std::vector<std::string> metodos = {"efectivo"};
                int id = Ventas::registrarVenta(lote, c->id, pesos, precio, metodos, fechaActual());
                std::cout << "Venta registrada (ID: " << id << ") - " << pesos.size() << " pollos" << std::endl;
            } else {
                std::cout << "Cliente '" << cliente << "' no encontrado" << std::endl;
            }
        }
        else if (sub == "pagar" && argc > 4) {
            int id = std::stoi(argv[3]);
            double monto = std::stod(argv[4]);
            Ventas::registrarPago(id, monto);
            std::cout << "Pago de USD " << std::fixed << std::setprecision(2) << monto << " registrado" << std::endl;
        }
        else if (sub == "deudas") {
            Reportes::reporteDeudas();
        }
    }
    else if (cmd == "inversor") {
        if (argc < 3) {
            std::cout << "Uso: inversor agregar | listar" << std::endl;
            return 0;
        }
        std::string sub = argv[2];
        if (sub == "agregar" && argc > 4) {
            std::string nombre = argv[3];
            double cantidad = std::stod(argv[4]);
            int id = Inversores::agregar(nombre, cantidad);
            std::cout << "Inversor '" << nombre << "' agregado (ID: " << id << ")" << std::endl;
        }
        else if (sub == "listar") {
            for (auto& i : Inversores::listar()) {
                std::cout << "  " << i.id << ": " << i.nombre << " - USD " << std::fixed << std::setprecision(2) << i.cantidad_invertida << std::endl;
            }
        }
    }
    else if (cmd == "herramienta") {
        if (argc < 3) {
            std::cout << "Uso: herramienta agregar | listar" << std::endl;
            return 0;
        }
        std::string sub = argv[2];
        if (sub == "agregar" && argc > 4) {
            std::string nombre = argv[3];
            int cantidad = std::stoi(argv[4]);
            double precio = 0.0;
            for (int i = 5; i < argc; i++) {
                std::string a = argv[i];
                if (a == "--precio" && i+1 < argc) precio = std::stod(argv[++i]);
            }
            int id = Herramientas::agregar(nombre, cantidad, precio, "");
            std::cout << "Herramienta '" << nombre << "' agregada (ID: " << id << ")" << std::endl;
        }
        else if (sub == "listar") {
            for (auto& h : Herramientas::listar()) {
                std::cout << "  " << h.id << ": " << h.nombre << " - Cant: " << h.cantidad << " - USD " << std::fixed << std::setprecision(2) << h.precio_unitario << " c/u" << std::endl;
            }
            std::cout << "Total: USD " << std::fixed << std::setprecision(2) << Herramientas::totalInvertido() << std::endl;
        }
    }
    else if (cmd == "reporte") {
        if (argc < 3) {
            std::cout << "Uso: reporte lote | financiero | inventario | deudas | completo" << std::endl;
            return 0;
        }
        std::string sub = argv[2];
        if (sub == "lote" && argc > 3) {
            int id = std::stoi(argv[3]);
            Reportes::reporteLote(id);
        }
        else if (sub == "financiero") {
            Reportes::reporteFinanciero();
        }
        else if (sub == "inventario") {
            Reportes::reporteInventario();
        }
        else if (sub == "deudas") {
            Reportes::reporteDeudas();
        }
        else if (sub == "completo") {
            Reportes::reporteCompleto();
        }
    }
    else if (cmd == "granja") {
        if (argc < 3) {
            std::cout << "Uso: granja crear <nombre> <tipo_id> | listar" << std::endl;
            return 0;
        }
        std::string sub = argv[2];
        if (sub == "crear" && argc > 4) {
            std::string nombre = argv[3];
            int tipo_id = std::stoi(argv[4]);
            int id = Granjas::crear(nombre, tipo_id, "", "", 10);
            std::cout << "Granja '" << nombre << "' creada (ID: " << id << ")" << std::endl;
        }
        else if (sub == "listar") {
            for (auto& g : Granjas::listar()) {
                Granjas::imprimirGranja(g);
            }
        }
    }
    else if (cmd == "corral") {
        if (argc < 3) {
            std::cout << "Uso: corral crear <granja> <num> <nombre> | listar [granja]" << std::endl;
            return 0;
        }
        std::string sub = argv[2];
        if (sub == "crear" && argc > 6) {
            int granja_id = std::stoi(argv[3]);
            int numero = std::stoi(argv[4]);
            std::string nombre = argv[5];
            int capacidad = std::stoi(argv[6]);
            int id = Corrales::crear(granja_id, numero, nombre, 10.0, 5.0, 3.0, "dos_aguas", "completa", "natural", "norte", "bloques", "canal", "led", true, false, "ninguna", true, capacidad, 10);
            std::cout << "Corral '" << nombre << "' creado (ID: " << id << ")" << std::endl;
        }
        else if (sub == "listar") {
            int granja_id = 0;
            if (argc > 3) granja_id = std::stoi(argv[3]);
            auto lista = Corrales::listarPorGranja(granja_id);
            for (auto& c : lista) {
                Corrales::imprimirCorral(c);
            }
        }
    }
    else if (cmd == "proveedor") {
        if (argc < 3) {
            std::cout << "Uso: proveedor agregar | listar" << std::endl;
            return 0;
        }
        std::string sub = argv[2];
        if (sub == "agregar" && argc > 3) {
            std::string nombre = argv[3];
            std::string telefono = (argc > 4) ? argv[4] : "";
            std::string direccion = (argc > 5) ? argv[5] : "";
            int id = Proveedores::agregar(nombre, telefono, "", direccion, "general", 0);
            std::cout << "Proveedor '" << nombre << "' agregado (ID: " << id << ")" << std::endl;
        }
        else if (sub == "listar") {
            for (auto& p : Proveedores::listar()) {
                std::cout << "  " << p.id << ": " << p.nombre << " - " << p.telefono << std::endl;
            }
        }
    }
    else if (cmd == "vacuna") {
        if (argc < 3) {
            std::cout << "Uso: vaccine agregar | listar" << std::endl;
            return 0;
        }
        std::string sub = argv[2];
        if (sub == "agregar" && argc > 4) {
            std::string nombre = argv[3];
            int dias = std::stoi(argv[4]);
            double costo = (argc > 5) ? std::stod(argv[5]) : 0.0;
            int id = Veterinaria::agregarVacuna(nombre, dias, costo, 0, "");
            std::cout << "Vacuna '" << nombre << "' agregada (ID: " << id << ")" << std::endl;
        }
        else if (sub == "listar") {
            Veterinaria::imprimirVacunas();
        }
    }
    else if (cmd == "medicamento") {
        if (argc < 3) {
            std::cout << "Uso: medicamento agregar | listar" << std::endl;
            return 0;
        }
        std::string sub = argv[2];
        if (sub == "agregar" && argc > 4) {
            std::string nombre = argv[3];
            std::string dosis = argv[4];
            double precio = (argc > 5) ? std::stod(argv[5]) : 0.0;
            int id = Veterinaria::agregarMedicamento(nombre, "", dosis, precio, 0, "General", 0);
            std::cout << "Medicamento '" << nombre << "' agregado (ID: " << id << ")" << std::endl;
        }
        else if (sub == "listar") {
            Veterinaria::imprimirMedicamentos();
        }
    }
    else if (cmd == "pesaje") {
        if (argc < 3) {
            std::cout << "Uso: pesaje registrar <lote> <semana> <pesos> | listar <lote>" << std::endl;
            return 0;
        }
        std::string sub = argv[2];
        if (sub == "registrar" && argc > 6) {
            int lote = std::stoi(argv[3]);
            int semana = std::stoi(argv[4]);
            std::string pesos = argv[5];
            std::string obs = (argc > 6) ? argv[6] : "";
            int id = Pesaje::registrar(lote, semana, fechaActual(), pesos, obs);
            std::cout << "Pesaje registrado (ID: " << id << ")" << std::endl;
        }
        else if (sub == "listar" && argc > 3) {
            int lote = std::stoi(argv[3]);
            auto lista = Pesaje::listarPorLote(lote);
            for (auto& p : lista) {
                std::cout << "Sem " << p.semana << ": " << std::fixed << std::setprecision(2) << p.peso_promedio << " kg (obj: " << p.peso_objetivo << ")" << std::endl;
            }
        }
    }
    else if (cmd == "metricas") {
        if (argc < 3) {
            std::cout << "Uso: metricas lote <id> | dashboard" << std::endl;
            return 0;
        }
        std::string sub = argv[2];
        if (sub == "lote" && argc > 3) {
            int id = std::stoi(argv[3]);
            Metricas::imprimirMetricasLote(id);
        }
        else if (sub == "dashboard") {
            Metricas::dashboardGeneral();
        }
    }
    else if (cmd == "exportar") {
        BaseDatos* db = BaseDatos::getInstancia();
        std::string json = db->exportarJSON();
        std::ofstream out("datos/backup.json");
        if (out.is_open()) {
            out << json;
            out.close();
            std::cout << "Datos exportados a datos/backup.json" << std::endl;
        } else {
            std::cout << "Error al crear archivo de exportacion" << std::endl;
        }
    }
    else if (cmd == "importar" && argc > 2) {
        std::ifstream in(argv[2]);
        if (in.is_open()) {
            std::string json((std::istreambuf_iterator<char>(in)),
                             std::istreambuf_iterator<char>());
            in.close();
            BaseDatos* db = BaseDatos::getInstancia();
            if (db->importarJSON(json)) {
                std::cout << "Datos importados correctamente" << std::endl;
            } else {
                std::cout << "Error al importar datos" << std::endl;
            }
        } else {
            std::cout << "No se pudo abrir el archivo: " << argv[2] << std::endl;
        }
    }
    else if (cmd == "optimizar") {
        BaseDatos* db = BaseDatos::getInstancia();
        db->vacuum();
    }
    else if (cmd == "alertas") {
        Alertas::mostrarAlertas();
    }
    else if (cmd == "factura") {
        if (argc < 3) {
            std::cout << "Uso: factura generar <venta_id> | ver <id> | listar | anular <id>" << std::endl;
            return 0;
        }
        std::string sub = argv[2];
        if (sub == "generar" && argc > 3) {
            int venta_id = std::stoi(argv[3]);
            std::string cedula = (argc > 4) ? argv[4] : "";
            std::string direccion = (argc > 5) ? argv[5] : "";
            int id = Facturacion::generarFactura(venta_id, cedula, direccion);
            std::cout << "Factura #" << id << " generada" << std::endl;
        }
        else if (sub == "ver" && argc > 3) {
            Facturacion::mostrarFactura(std::stoi(argv[3]));
        }
        else if (sub == "listar") {
            for (auto& f : Facturacion::listarFacturas()) {
                std::cout << f.numero_factura << " | " << f.cliente_nombre << " | USD " << std::fixed << std::setprecision(2) << f.total << " | " << (f.anulada ? "ANULADA" : "ACTIVA") << std::endl;
            }
        }
        else if (sub == "anular" && argc > 3) {
            Facturacion::anularFactura(std::stoi(argv[3]));
            std::cout << "Factura anulada" << std::endl;
        }
    }
    else if (cmd == "contenedor") {
        if (argc < 3) {
            std::cout << "Uso: contenedor crear <nombre> <ubicacion> <capacidad> [temp] | listar | productos" << std::endl;
            return 0;
        }
        std::string sub = argv[2];
        if (sub == "crear" && argc > 5) {
            std::string nombre = argv[3];
            std::string ubicacion = argv[4];
            int capacidad = std::stoi(argv[5]);
            double temp = (argc > 6) ? std::stod(argv[6]) : 4.0;
            int id = Contenedores::crear(nombre, ubicacion, capacidad, temp);
            std::cout << "Contenedor '" << nombre << "' creado (ID: " << id << ")" << std::endl;
        }
        else if (sub == "listar") {
            for (auto& c : Contenedores::listar()) {
                std::cout << c.id << ": " << c.nombre << " | " << c.ubicacion << " | Cap: " << c.cantidad_actual << "/" << c.capacidad_maxima << " | " << estadoContenedorToString(c.estado) << " | " << c.temperatura << "°C" << std::endl;
            }
        }
        else if (sub == "productos") {
            for (auto& p : Contenedores::listarProductos()) {
                std::cout << "ID: " << p.id << " | Contenedor: " << p.contenedor_id << " | Lote: " << p.lote_id << " | Cant: " << p.cantidad << " | Peso: " << std::fixed << std::setprecision(2) << p.peso_total << " kg" << std::endl;
            }
        }
    }
    else if (cmd == "beneficio") {
        if (argc < 3) {
            std::cout << "Uso: beneficio registrar <lote> <cant> <peso> <tipo> <operador> <cedula> | listar | stats" << std::endl;
            return 0;
        }
        std::string sub = argv[2];
        if (sub == "registrar" && argc > 8) {
            int lote = std::stoi(argv[3]);
            int cantidad = std::stoi(argv[4]);
            double peso = std::stod(argv[5]);
            std::string tipo = argv[6];
            std::string operador = argv[7];
            std::string cedula = argv[8];
            TipoBeneficio tb = stringToTipoBeneficio(tipo);
            int id = Beneficio::registrar(lote, cantidad, peso, tb, operador, cedula);
            std::cout << "Beneficio registrado (ID: " << id << ")" << std::endl;
        }
        else if (sub == "listar") {
            for (auto& b : Beneficio::listar()) {
                std::cout << "Lote: " << b.lote_id << " | Cant: " << b.cantidad << " | Peso: " << std::fixed << std::setprecision(2) << b.peso_total << " kg | Tipo: " << tipoBeneficioToString(b.tipo_beneficio) << " | Operador: " << b.operador_nombre << std::endl;
            }
        }
        else if (sub == "stats") {
            Beneficio::mostrarEstadisticas();
        }
    }
    else {
        std::cout << "Comando desconocido: " << cmd << std::endl;
        std::cout << "Ejecute './granja ayuda' para ver los comandos disponibles" << std::endl;
    }

    return 0;
}