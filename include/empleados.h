#ifndef EMPLEADOS_H
#define EMPLEADOS_H

#include <string>
#include <vector>
#include "modelos.h"

enum class Cargo {
    Administrador,
    Gerente,
    Trabajador,
    Veterinario,
    Mantenimiento,
    Seguridad
};

enum class TipoContrato {
    Fijo,
    Temporal,
    PorDia
};

enum class Turno {
    Matutino,
    Vespertino,
    Nocturno,
    Rotativo
};

struct Empleado {
    int id = 0;
    std::string nombre;
    std::string cedula;
    std::string telefono;
    std::string direccion;
    Cargo cargo;
    TipoContrato tipo_contrato;
    double salario = 0.0;
    Turno turno;
    bool activo = true;
    std::string fecha_ingreso;
};

struct Asistencia {
    int id = 0;
    int empleado_id = 0;
    std::string fecha;
    bool presente = true;
    std::string hora_entrada;
    std::string hora_salida;
    int horas_extras = 0;
};

class Empleados {
public:
    static int agregar(const std::string& nombre, const std::string& cedula, const std::string& telefono, Cargo cargo, double salario, Turno turno);
    static Empleado* obtener(int id);
    static std::vector<Empleado> listar();
    static std::vector<Empleado> listarActivos();
    static bool actualizar(int id, const std::string& nombre, const std::string& telefono, Cargo cargo, double salario);
    static bool eliminar(int id);
    static bool cambiarEstado(int id, bool activo);
    
    static int registrarAsistencia(int empleado_id, bool presente, const std::string& hora_entrada, const std::string& hora_salida, int horas_extras);
    static std::vector<Asistencia> listarAsistencia(int empleado_id = 0);
    static std::vector<Asistencia> listarAsistenciaPorFecha(const std::string& fecha);
    static double getHorasExtrasMes(int empleado_id, int mes, int anio);
    
    static double getTotalSalarios();
    static double getTotalSalariosActivos();
    static std::string cargoToString(Cargo c);
    static Cargo stringToCargo(const std::string& s);
    static std::string turnoToString(Turno t);
    static Turno stringToTurno(const std::string& s);
};

#endif // EMPLEADOS_H