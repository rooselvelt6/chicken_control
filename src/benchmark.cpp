#include <iostream>
#include <chrono>
#include <vector>
#include <string>
#include <sstream>
#include <cstdlib>
#include <thread>
#include <mutex>
#include <atomic>
#include <random>
#include "../include/base_datos.h"
#include "../include/modelos.h"
#include "../include/utils.h"
#include "../include/lotes.h"
#include "../include/animales.h"
#include "../include/alimentacion.h"
#include "../include/ventas.h"
#include "../include/pesaje.h"
#include "../include/metricas.h"
#include "../include/granjas.h"
#include "../include/corrales.h"
#include "../include/proveedores.h"

using namespace std;
using namespace std::chrono;

atomic<int> operaciones_exitosas(0);
atomic<int> operaciones_fallidas(0);
mutex log_mutex;

void logSeguro(const string& msg) {
    lock_guard<mutex> lock(log_mutex);
}

void testCrearLote(int iteraciones) {
    auto* db = BaseDatos::getInstancia();
    db->iniciarTransaccion();
    auto inicio = high_resolution_clock::now();
    for (int i = 0; i < iteraciones; i++) {
        int num = 1000 + i;
        Lotes::crear(num, "2024-01-01", 45);
    }
    auto fin = high_resolution_clock::now();
    db->terminarTransaccion();
    auto duracion = duration_cast<milliseconds>(fin - inicio);
    cout << "  Crear " << iteraciones << " lotes: " << duracion.count() << " ms" << endl;
}

void testCrearAnimales(int iteraciones) {
    auto* db = BaseDatos::getInstancia();
    db->iniciarTransaccion();
    auto inicio = high_resolution_clock::now();
    for (int i = 0; i < iteraciones; i++) {
        Animales::agregar(1, 50, 1.0, 0.5, "2024-01-01");
    }
    auto fin = high_resolution_clock::now();
    db->terminarTransaccion();
    auto duracion = duration_cast<milliseconds>(fin - inicio);
    cout << "  Crear " << iteraciones << " registros de animales: " << duracion.count() << " ms" << endl;
}

void testConsultarLotes(int iteraciones) {
    auto inicio = high_resolution_clock::now();
    for (int i = 0; i < iteraciones; i++) {
        auto lista = Lotes::listar();
    }
    auto fin = high_resolution_clock::now();
    auto duracion = duration_cast<milliseconds>(fin - inicio);
    cout << "  Consultar lotes " << iteraciones << " veces: " << duracion.count() << " ms" << endl;
}

void testConsultarAnimales(int iteraciones) {
    auto inicio = high_resolution_clock::now();
    for (int i = 0; i < iteraciones; i++) {
        auto lista = Animales::listarPorLote(1);
    }
    auto fin = high_resolution_clock::now();
    auto duracion = duration_cast<milliseconds>(fin - inicio);
    cout << "  Consultar animales " << iteraciones << " veces: " << duracion.count() << " ms" << endl;
}

void testCrearGranjas(int iteraciones) {
    auto* db = BaseDatos::getInstancia();
    db->iniciarTransaccion();
    auto inicio = high_resolution_clock::now();
    for (int i = 0; i < iteraciones; i++) {
        string nombre = "Granja Test " + to_string(i);
        Granjas::crear(nombre, 1, "", "", 10);
    }
    auto fin = high_resolution_clock::now();
    db->terminarTransaccion();
    auto duracion = duration_cast<milliseconds>(fin - inicio);
    cout << "  Crear " << iteraciones << " granjas: " << duracion.count() << " ms" << endl;
}

void testCrearCorrales(int iteraciones) {
    auto* db = BaseDatos::getInstancia();
    db->iniciarTransaccion();
    auto inicio = high_resolution_clock::now();
    for (int i = 0; i < iteraciones; i++) {
        Corrales::crear(1, i+1, "Corral Test", 10.0, 5.0, 3.0, 
                       "dos_aguas", "completa", "natural", "norte", 
                       "bloques", "canal", "led", true, false, "ninguna", 
                       true, 100, 10);
    }
    auto fin = high_resolution_clock::now();
    db->terminarTransaccion();
    auto duracion = duration_cast<milliseconds>(fin - inicio);
    cout << "  Crear " << iteraciones << " corrales: " << duracion.count() << " ms" << endl;
}

void testRegistrarPesaje(int iteraciones) {
    auto* db = BaseDatos::getInstancia();
    db->iniciarTransaccion();
    auto inicio = high_resolution_clock::now();
    for (int i = 0; i < iteraciones; i++) {
        string pesos = "1.5,1.6,1.7,1.5,1.8";
        Pesaje::registrar(1, (i % 6) + 1, "2024-01-01", pesos, "Test");
    }
    auto fin = high_resolution_clock::now();
    db->terminarTransaccion();
    auto duracion = duration_cast<milliseconds>(fin - inicio);
    cout << "  Registrar " << iteraciones << " pesajes: " << duracion.count() << " ms" << endl;
}

void testCalcularMetricas(int iteraciones) {
    auto inicio = high_resolution_clock::now();
    for (int i = 0; i < iteraciones; i++) {
        MetricasLote m = Metricas::calcularMetricasLote(1);
    }
    auto fin = high_resolution_clock::now();
    auto duracion = duration_cast<milliseconds>(fin - inicio);
    cout << "  Calcular métricas " << iteraciones << " veces: " << duracion.count() << " ms" << endl;
}

void testEscrituraMasiva(int registros) {
    auto* db = BaseDatos::getInstancia();
    db->iniciarTransaccion();
    auto inicio = high_resolution_clock::now();
    for (int i = 0; i < registros; i++) {
        stringstream ss;
        ss << "INSERT INTO test_performance (data) VALUES ('test_" << i << "')";
        db->ejecutarSQL(ss.str());
    }
    auto fin = high_resolution_clock::now();
    db->terminarTransaccion();
    auto duracion = duration_cast<milliseconds>(fin - inicio);
    cout << "  Escritura masiva " << registros << " registros: " << duracion.count() << " ms" << endl;
    
    db->ejecutarSQL("DELETE FROM test_performance");
}

void testLecturaMasiva(int registros) {
    auto* db = BaseDatos::getInstancia();
    for (int i = 0; i < registros; i++) {
        stringstream ss;
        ss << "INSERT INTO test_performance (data) VALUES ('test_" << i << "')";
        db->ejecutarSQL(ss.str());
    }
    
    auto inicio = high_resolution_clock::now();
    sqlite3_stmt* stmt;
    string sql = "SELECT * FROM test_performance";
    int count = 0;
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) count++;
    }
    sqlite3_finalize(stmt);
    
    auto fin = high_resolution_clock::now();
    auto duracion = duration_cast<milliseconds>(fin - inicio);
    cout << "  Lectura masiva " << registros << " registros: " << duracion.count() << " ms" << endl;
    
    db->ejecutarSQL("DELETE FROM test_performance");
}

void hiloConcurrente(int id, int operaciones) {
    for (int i = 0; i < operaciones; i++) {
        try {
            int lote_id = (id % 3) + 1;
            Animales::agregar(lote_id, 5, 1.0, 1.5, "2024-01-01");
            operaciones_exitosas++;
        } catch (...) {
            operaciones_fallidas++;
        }
    }
}

void testRaceCondition(int num_hilos, int ops_por_hilo) {
    operaciones_exitosas = 0;
    operaciones_fallidas = 0;
    
    auto* db = BaseDatos::getInstancia();
    db->iniciarTransaccion();
    
    auto inicio = high_resolution_clock::now();
    
    vector<thread> hilos;
    for (int i = 0; i < num_hilos; i++) {
        hilos.emplace_back(hiloConcurrente, i, ops_por_hilo);
    }
    
    for (auto& h : hilos) {
        h.join();
    }
    
    auto fin = high_resolution_clock::now();
    db->terminarTransaccion();
    
    auto duracion = duration_cast<milliseconds>(fin - inicio);
    cout << "  Race condition (" << num_hilos << " hilos, " << ops_por_hilo << " ops): " << duracion.count() << " ms" << endl;
    cout << "    Exitosas: " << operaciones_exitosas << ", Fallidas: " << operaciones_fallidas << endl;
}

void testInyeccionSQL() {
    cout << "  Probando inyección SQL..." << endl;
    auto* db = BaseDatos::getInstancia();
    
    vector<string> ataques = {
        "'; DROP TABLE lotes; --",
        "1; DELETE FROM animales; --",
        "' OR '1'='1",
        "admin'--",
        "<script>alert('xss')</script>",
        " UNION SELECT * FROM configuracion--",
        "1; UPDATE lotes SET activo=0; --",
        "'; CREATE TABLE injected(x TEXT); --"
    };
    
    int bloqueados = 0;
    int vulnerables = 0;
    
    for (const auto& ataque : ataques) {
        try {
            int id_seguro = parsearIdSeguro(ataque);
            if (id_seguro < 0) {
                bloqueados++;
                continue;
            }
            
            stringstream ss;
            ss << "SELECT * FROM lotes WHERE id = " << id_seguro;
            string sql = ss.str();
            
            sqlite3_stmt* stmt;
            int rc = sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr);
            
            if (rc != SQLITE_OK) {
                bloqueados++;
            } else {
                if (sqlite3_step(stmt) == SQLITE_ROW) {
                }
                sqlite3_finalize(stmt);
                vulnerables++;
            }
        } catch (...) {
            bloqueados++;
        }
    }
    
    cout << "    Inyecciones bloqueadas: " << bloqueados << "/" << ataques.size() << endl;
    cout << "    Vulnerabilidades encontradas: " << vulnerables << endl;
}

void testNullPointers() {
    cout << "  Probando punteros nulos..." << endl;
    
    int errores = 0;
    int pruebas = 0;
    
    pruebas++;
    Lote* lote = Lotes::obtener(99999);
    if (lote == nullptr) {
        cout << "    Lote inexistente devuelve nullptr: OK" << endl;
    } else {
        errores++;
    }
    
    pruebas++;
    Granja* granja = Granjas::obtener(99999);
    if (granja == nullptr) {
        cout << "    Granja inexistente devuelve nullptr: OK" << endl;
    } else {
        errores++;
    }
    
    pruebas++;
    Corral* corral = Corrales::obtener(99999);
    if (corral == nullptr) {
        cout << "    Corral inexistente devuelve nullptr: OK" << endl;
    } else {
        errores++;
    }
    
    pruebas++;
    Proveedor* proveedor = Proveedores::obtener(99999);
    if (proveedor == nullptr) {
        cout << "    Proveedor inexistente devuelve nullptr: OK" << endl;
    } else {
        errores++;
    }
    
    cout << "    Pruebas de null: " << pruebas << ", Errores: " << errores << endl;
}

void testStressBD(int duracion_segundos) {
    cout << "  Test de estrés (" << duracion_segundos << " segundos)..." << endl;
    
    auto* db = BaseDatos::getInstancia();
    atomic<int> ops(0);
    
    auto inicio = high_resolution_clock::now();
    auto fin = inicio + seconds(duracion_segundos);
    
    while (high_resolution_clock::now() < fin) {
        try {
            db->iniciarTransaccion();
            for (int i = 0; i < 10; i++) {
                Animales::agregar(1, 1, 1.0, 1.5, "2024-01-01");
                Lotes::listar();
            }
            db->terminarTransaccion();
            ops++;
        } catch (...) {
        }
    }
    
    auto duracion = duration_cast<seconds>(high_resolution_clock::now() - inicio).count();
    cout << "    Operaciones completadas: " << ops * 10 << " en " << duracion << "s" << endl;
    cout << "    Throughput: " << (ops * 10 / duracion) << " ops/s" << endl;
}

int main(int argc, char* argv[]) {
    cout << "======================================================" << endl;
    cout << "   PRUEBAS DE RENDIMIENTO - CHICKEN CONTROL" << endl;
    cout << "======================================================" << endl << endl;
    
    BaseDatos* db = BaseDatos::getInstancia();
    db->inicializar();
    
    db->ejecutarSQL("CREATE TABLE IF NOT EXISTS test_performance (id INTEGER PRIMARY KEY, data TEXT)");
    
    cout << "--- PRUEBAS DE ESCRITURA ---" << endl;
    testCrearLote(10);
    testCrearAnimales(10);
    testCrearGranjas(5);
    testCrearCorrales(5);
    testRegistrarPesaje(10);
    
    cout << endl << "--- PRUEBAS DE CONSULTA ---" << endl;
    testConsultarLotes(50);
    testConsultarAnimales(50);
    
    cout << endl << "--- PRUEBAS DE CÁLCULO ---" << endl;
    testCalcularMetricas(10);
    
    cout << endl << "--- PRUEBAS DE RENDIMIENTO SQLite ---" << endl;
    testEscrituraMasiva(100);
    testLecturaMasiva(100);
    
    cout << endl << "--- PRUEBAS DE SENSIBILIDAD ---" << endl;
    cout << "  Probando con diferentes volúmenes de datos:" << endl;
    
    for (int n = 10; n <= 100; n += 30) {
        auto* db2 = BaseDatos::getInstancia();
        db2->iniciarTransaccion();
        auto inicio = high_resolution_clock::now();
        for (int i = 0; i < n; i++) {
            Animales::agregar(1, 10, 1.0, 1.5, "2024-01-01");
        }
        auto lista = Animales::listarPorLote(1);
        auto fin = high_resolution_clock::now();
        db2->terminarTransaccion();
        auto duracion = duration_cast<microseconds>(fin - inicio);
        cout << "    " << n << " operaciones: " << duracion.count() << " µs" << endl;
    }
    
    cout << endl << "--- PRUEBAS DE RACE CONDITION ---" << endl;
    testRaceCondition(4, 25);
    testRaceCondition(8, 15);
    
    cout << endl << "--- PRUEBAS DE SEGURIDAD - INYECCIÓN SQL ---" << endl;
    testInyeccionSQL();
    
    cout << endl << "--- PRUEBAS DE SEGURIDAD - PUNTEROS NULOS ---" << endl;
    testNullPointers();
    
    cout << endl << "--- PRUEBAS DE ESTRÉS ---" << endl;
    testStressBD(3);
    
    cout << endl << "======================================================" << endl;
    cout << "   PRUEBAS COMPLETADAS" << endl;
    cout << "======================================================" << endl;
    
    return 0;
}