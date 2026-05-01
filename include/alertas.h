#ifndef ALERTAS_H
#define ALERTAS_H

#include <string>
#include <vector>

struct Alerta {
    std::string tipo;
    std::string mensaje;
    int prioridad; // 1=baja, 2=media, 3=alta
};

class Alertas {
public:
    static std::vector<Alerta> verificarAlertas();
    static void mostrarAlertas();
    static void verificarInventario(std::vector<Alerta>& alertas);
    static void verificarMortandad(std::vector<Alerta>& alertas);
    static void verificarDeudas(std::vector<Alerta>& alertas);
    static void verificarLotesActivos(std::vector<Alerta>& alertas);
};

#endif // ALERTAS_H