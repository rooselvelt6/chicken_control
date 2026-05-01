#ifndef TEMPERATURA_H
#define TEMPERATURA_H

#include <string>
#include <vector>
#include "modelos.h"

struct RegistroTemperatura {
    int id = 0;
    int corral_id = 0;
    double temperatura = 0.0;
    double humedad = 0.0;
    std::string fecha;
    std::string observaciones;
};

class Temperatura {
public:
    static int registrar(int corral_id, double temperatura, double humedad, const std::string& observaciones = "");
    static std::vector<RegistroTemperatura> listar(int corral_id = 0);
    static std::vector<RegistroTemperatura> listarPorFecha(const std::string& fecha);
    static std::vector<RegistroTemperatura> listarRango(int corral_id, const std::string& fecha_inicio, const std::string& fecha_fin);
    static double getTemperaturaPromedio(int corral_id, const std::string& fecha);
    static double getTemperaturaMinima(int corral_id);
    static double getTemperaturaMaxima(int corral_id);
    static bool verificarAlertas(int corral_id, double temp_min, double temp_max);
    static void mostrarHistorico(int corral_id, int dias = 7);
};

#endif // TEMPERATURA_H