#ifndef MODELOS_H
#define MODELOS_H

#include <string>
#include <vector>
#include <map>
#include <utility>

// ==================== ENUMS ====================

enum class Fase {
    Bebe,
    Intermedio,
    Grande,
    Desconocido
};

enum class CausaMuerte {
    Aplastamiento,
    Ahogamiento,
    Gases_toxicos,
    Infarto,
    Retencion_liquidos,
    Desconocida,
    Frio,
    Calor,
    Enfermdad,
    Otro
};

enum class TipoGranja {
    Terrestre,
    Aerea,
    Maritima,
    Desconocido
};

enum class TipoTecho {
    Zinc,
    Eternit,
    Manga,
    Concreto,
    Otro
};

enum class TipoProteccion {
    Completo,
    Semi_abierto,
    Malla,
    Lona,
   Otro
};

enum class TipoVentilacion {
    Natural,
    Forzada,
    Mixta,
    Ausente
};

enum class Orientacion {
    Norte_Sur,
    Este_Oeste,
    Desconocido
};

enum class SistemaComederos {
    Lineal,
    Circular,
    Automatico,
    Manual
};

enum class SistemaBebederos {
    Nipple,
    Campana,
    Automatico,
    Manual
};

enum class TipoCalefaccion {
    Gas,
    Electrico,
    Solar,
    Ninguna
};

enum class EstadoEquipo {
    Nuevo,
    Bueno,
    Regular,
    Malo
};

// ==================== ESTRUCTURAS ====================

// Análisis nutricional del alimento
struct AnalisisNutricional {
    double humedad = 12.5;
    double proteina = 23.5;
    double grasa = 5.0;
    double ceniza = 6.0;
    double calcio = 1.10;
    double fibra = 3.0;
    double fosforo = 0.70;
};

// Tipos de granja
struct TipoGranjaInfo {
    int id = 0;
    std::string nombre;
    std::string descripcion;
    int capacidad_recomendada = 0;
    double costo_inicial = 0.0;
    int densidad_recomendada = 10; // pollos por m2
};

// Granja
struct Granja {
    int id = 0;
    std::string nombre;
    int tipo_granja_id = 0;
    std::string nombre_tipo;
    std::string ubicacion;
    std::string dimensiones; // "largo x ancho"
    int densidad_maxima = 10; // pollos por m2
    bool activa = true;
    std::string created_at;
};

// Corral/Galpón
struct Corral {
    int id = 0;
    int granja_id = 0;
    int numero = 0;
    std::string nombre;
    
    // Dimensiones del corral
    double largo_m = 0.0;
    double ancho_m = 0.0;
    double altura_m = 0.0;
    double area_m2 = 0.0;
    
    // Características físicas
    TipoTecho tipo_techo = TipoTecho::Zinc;
    TipoProteccion proteccion = TipoProteccion::Completo;
    TipoVentilacion ventilacion = TipoVentilacion::Natural;
    Orientacion orientacion = Orientacion::Norte_Sur;
    std::string material_paredes;
    
    // Sistemas
    SistemaComederos sistema_comederos = SistemaComederos::Lineal;
    SistemaBebederos sistema_bebederos = SistemaBebederos::Campana;
    bool tiene_iluminacion = true;
    bool tiene_calefaccion = false;
    TipoCalefaccion tipo_calefaccion = TipoCalefaccion::Ninguna;
    bool tiene_drenaje = true;
    
    // Capacidad
    int capacidad_maxima = 0;
    int densidad_maxima = 10; // pollos por m2
    int cantidad_actual = 0;
    
    // Estado
    bool activo = true;
    std::string created_at;
};

// Tipo de cama
struct TipoCama {
    int id = 0;
    std::string nombre;
    std::string descripcion;
    double costo_por_kg = 0.0;
    int proveedor_id = 0;
    std::string nombre_proveedor;
    int frecuencia_cambio_dias = 14;
    bool activa = true;
};

// Proveedor
struct Proveedor {
    int id = 0;
    std::string nombre;
    std::string telefono;
    std::string email;
    std::string direccion;
    std::string tipo; // alimentos, medicamentos, materiales, equipos, todos
    int rating = 3; // 1-5 estrellas
    bool activo = true;
};

// Equipo
struct Equipo {
    int id = 0;
    int corral_id = 0;
    std::string tipo; // comedero, bebedero, ventilador, calentador, bombillo
    std::string nombre;
    int cantidad = 0;
    double costo_unitario = 0.0;
    EstadoEquipo estado = EstadoEquipo::Bueno;
    bool activo = true;
};

// Vacunas
struct Vacuna {
    int id = 0;
    std::string nombre;
    int aplicacion_dias = 0; // día del ciclo
    double costo = 0.0;
    int proveedor_id = 0;
    std::string nombre_proveedor;
    std::string observaciones;
    bool activa = true;
};

// Medicamento
struct Medicamento {
    int id = 0;
    std::string nombre;
    std::string principio_activo;
    std::string dosis;
    double precio = 0.0;
    int proveedor_id = 0;
    std::string nombre_proveedor;
    std::string categoria; // antibiotico, vitamina, desinfectante, otro
    int stock = 0;
    bool activo = true;
};

// Aplicación de vacuna por lote
struct LoteVacuna {
    int id = 0;
    int lote_id = 0;
    int vacuna_id = 0;
    std::string nombre_vacuna;
    std::string fecha_aplicacion;
    std::string observacion;
};

// Uso de medicamento por lote
struct LoteMedicamento {
    int id = 0;
    int lote_id = 0;
    int medicamento_id = 0;
    std::string nombre_medicamento;
    double cantidad_usada = 0.0;
    std::string fecha;
    std::string motivo;
};

// Pesaje semanal
struct PesajeSemanal {
    int id = 0;
    int lote_id = 0;
    int semana = 0;
    std::string fecha;
    int cantidad_pollos_pesados = 0;
    double peso_promedio = 0.0;
    double peso_total = 0.0;
    double peso_minimo = 0.0;
    double peso_maximo = 0.0;
    double peso_objetivo = 0.0;
    std::string observacion;
};

// Servicios básicos
struct Servicio {
    int id = 0;
    std::string nombre;
    std::string tipo; // agua, luz, gas, internet, otro
    double costo_mensual = 0.0;
    std::string proveedor;
    bool activo = true;
};

// Gastos operativos
struct GastoOperativo {
    int id = 0;
    int lote_id = 0;
    std::string tipo;
    std::string descripcion;
    double monto = 0.0;
    std::string fecha;
};

// Ciclo de producción
struct CicloProduccion {
    int lote_id = 0;
    int numero = 0;
    std::string fecha_inicio;
    std::string fecha_fin_estimada;
    int duracion_dias = 45;
    int dias_descanso = 14;
    std::string estado; // activo, cerrado, pendiente
};

// ==================== ESTRUCTURAS EXISTENTES (ACTUALIZADAS) ====================

struct Lote {
    int id = 0;
    int corral_id = 0;
    int numero = 0;
    std::string fecha_inicio;
    std::string fecha_fin;
    int duracion_dias = 45;
    bool activo = true;
    // Información del corral
    std::string nombre_corral;
    int capacidad_corral;
};

struct Animal {
    int id = 0;
    int lote_id = 0;
    int cantidad = 0;
    double precio_unitario = 0.0;
    double peso_promedio = 0.0;
    Fase fase = Fase::Bebe;
    std::string fecha_ingreso;
};

struct Muerte {
    int id = 0;
    int lote_id = 0;
    CausaMuerte causa = CausaMuerte::Desconocida;
    int cantidad = 0;
    std::string fecha;
};

struct Sacrificio {
    int id = 0;
    int lote_id = 0;
    int cantidad = 0;
    double peso_total = 0.0;
    double peso_promedio = 0.0;
    std::string fecha;
};

struct Alimento {
    int id = 0;
    std::string nombre;
    std::string marca;
    Fase fase = Fase::Desconocido;
    AnalisisNutricional analisis;
    int contenido_kg = 40;
    double precio_unitario = 0.0;
    int inventario = 0;
    int inicio_dias = 0;
    int final_dias = 999;
};

struct ConsumoAlimento {
    int id = 0;
    int lote_id = 0;
    int alimento_id = 0;
    int cantidad_sacos = 0;
    std::string nombre_alimento;
};

struct Cliente {
    int id = 0;
    std::string nombre;
    std::string telefono;
    std::string referencia;
};

struct Venta {
    int id = 0;
    int lote_id = 0;
    int cliente_id = 0;
    std::string nombre_cliente;
    int cantidad = 0;
    std::vector<double> pesos;
    double precio_unitario = 0.0;
    double total = 0.0;
    double abonado = 0.0;
    double pendiente = 0.0;
    bool pagada = false;
    std::vector<std::string> metodos_pago;
    std::string fecha;
};

struct Inversor {
    int id = 0;
    std::string nombre;
    double cantidad_invertida = 0.0;
};

struct Encargado {
    int id = 0;
    std::string nombre;
};

struct Herramienta {
    int id = 0;
    std::string nombre;
    int cantidad = 0;
    double precio_unitario = 0.0;
    std::string tipo;
};

struct Configuracion {
    double precio_dolar = 4340.0;
    double precio_kg_ves = 15000.0;
};

// ==================== MÉTRICAS ====================

struct MetricasLote {
    int lote_id;
    int corral_id;
    std::string nombre_corral;
    
    // Animales
    int total_inicial = 0;
    int total_muertes = 0;
    int total_sacrificados = 0;
    int total_vendidos = 0;
    double porcentaje_mortalidad = 0.0;
    
    // Pesaje
    double peso_promedio_final = 0.0;
    double peso_promedio_objetivo = 0.0;
    bool peso_ok = false;
    
    // Alimentación
    double total_alimento_kg = 0.0;
    double conversion_alimenticia = 0.0;
    
    // Costos
    double costo_animales = 0.0;
    double costo_alimento = 0.0;
    double costo_medicamentos = 0.0;
    double costo_servicios = 0.0;
    double costo_total = 0.0;
    
    // Ventas
    double ingreso_ventas = 0.0;
    double ganancia = 0.0;
    double rentabilidad_porcentaje = 0.0;
    
    // Producción
    double kg_producidos = 0.0;
    double costo_por_kg = 0.0;
};

struct MetricasGranja {
    int granja_id;
    std::string nombre_granja;
    
    int total_corrales = 0;
    int corrales_activos = 0;
    int capacidad_total = 0;
    int ocupacion_actual = 0;
    double porcentaje_ocupacion = 0.0;
    
    // Lotes
    int lotes_activos = 0;
    int lotes_cerrados = 0;
    
    // Métricas consolidadas
    double mortalidad_promedio = 0.0;
    double conversion_promedio = 0.0;
    double rentabilidad_promedio = 0.0;
    
    // Costos
    double gasto_total = 0.0;
    double ingreso_total = 0.0;
};

// ==================== FUNCIONES DE CONVERSIÓN ====================

std::string faseToString(Fase f);
Fase stringToFase(const std::string& s);
std::string causaMuerteToString(CausaMuerte c);
CausaMuerte stringToCausaMuerte(const std::string& s);
std::string tipoGranjaToString(TipoGranja t);
TipoGranja stringToTipoGranja(const std::string& s);
std::string tipoTechoToString(TipoTecho t);
TipoTecho stringToTipoTecho(const std::string& s);
std::string tipoProteccionToString(TipoProteccion p);
TipoProteccion stringToTipoProteccion(const std::string& s);
std::string tipoVentilacionToString(TipoVentilacion v);
TipoVentilacion stringToTipoVentilacion(const std::string& s);
std::string orientacionToString(Orientacion o);
Orientacion stringToOrientacion(const std::string& s);
std::string sistemaComederosToString(SistemaComederos s);
SistemaComederos stringToSistemaComederos(const std::string& s);
std::string sistemaBebederosToString(SistemaBebederos s);
SistemaBebederos stringToSistemaBebederos(const std::string& s);
std::string tipoCalefaccionToString(TipoCalefaccion t);
TipoCalefaccion stringToTipoCalefaccion(const std::string& s);
std::string estadoEquipoToString(EstadoEquipo e);
EstadoEquipo stringToEstadoEquipo(const std::string& s);

// Calcular peso objetivo por semana
double pesoObjetivoSemana(int semana);

#endif // MODELOS_H