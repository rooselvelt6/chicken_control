#include "../include/modelos.h"
#include <cmath>
#include <algorithm>
#include <iostream>

// Funciones de conversión de Fase
std::string faseToString(Fase f) {
    switch (f) {
        case Fase::Bebe: return "Bebe";
        case Fase::Intermedio: return "Intermedio";
        case Fase::Grande: return "Grande";
        default: return "Desconocido";
    }
}

Fase stringToFase(const std::string& s) {
    if (s == "Bebe" || s == "bebe") return Fase::Bebe;
    if (s == "Intermedio" || s == "intermedio") return Fase::Intermedio;
    if (s == "Grande" || s == "grande") return Fase::Grande;
    return Fase::Desconocido;
}

// Funciones de conversión de CausaMuerte
std::string causaMuerteToString(CausaMuerte c) {
    switch (c) {
        case CausaMuerte::Aplastamiento: return "Aplastamiento";
        case CausaMuerte::Ahogamiento: return "Ahogamiento";
        case CausaMuerte::Gases_toxicos: return "Gases toxicos";
        case CausaMuerte::Infarto: return "Infarto";
        case CausaMuerte::Retencion_liquidos: return "Retencion de liquidos";
        case CausaMuerte::Frio: return "Frio";
        case CausaMuerte::Calor: return "Calor";
        case CausaMuerte::Enfermdad: return "Enfermedad";
        case CausaMuerte::Otro: return "Otro";
        default: return "Desconocida";
    }
}

CausaMuerte stringToCausaMuerte(const std::string& s) {
    std::string lower = s;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    if (lower == "aplastamiento") return CausaMuerte::Aplastamiento;
    if (lower == "ahogamiento") return CausaMuerte::Ahogamiento;
    if (lower == "gases toxicos" || lower == "gases_toxicos" || lower == "gases") return CausaMuerte::Gases_toxicos;
    if (lower == "infarto") return CausaMuerte::Infarto;
    if (lower == "retencion de liquidos" || lower == "retencion") return CausaMuerte::Retencion_liquidos;
    if (lower == "frio") return CausaMuerte::Frio;
    if (lower == "calor") return CausaMuerte::Calor;
    if (lower == "enfermedad" || lower == "enfermo") return CausaMuerte::Enfermdad;
    if (lower == "otro") return CausaMuerte::Otro;
    return CausaMuerte::Desconocida;
}

// Funciones de TipoGranja
std::string tipoGranjaToString(TipoGranja t) {
    switch (t) {
        case TipoGranja::Terrestre: return "terrestre";
        case TipoGranja::Aerea: return "aerea";
        case TipoGranja::Maritima: return "maritima";
        default: return "desconocido";
    }
}

TipoGranja stringToTipoGranja(const std::string& s) {
    std::string lower = s;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    if (lower == "terrestre") return TipoGranja::Terrestre;
    if (lower == "aerea" || lower == "aéreo" || lower == "poleo") return TipoGranja::Aerea;
    if (lower == "maritima" || lower == "maritima") return TipoGranja::Maritima;
    return TipoGranja::Desconocido;
}

// Funciones de TipoTecho
std::string tipoTechoToString(TipoTecho t) {
    switch (t) {
        case TipoTecho::Zinc: return "zinc";
        case TipoTecho::Eternit: return "eternit";
        case TipoTecho::Manga: return "manga";
        case TipoTecho::Concreto: return "concreto";
        case TipoTecho::Otro: return "otro";
        default: return "desconocido";
    }
}

TipoTecho stringToTipoTecho(const std::string& s) {
    std::string lower = s;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    if (lower == "zinc") return TipoTecho::Zinc;
    if (lower == "eternit") return TipoTecho::Eternit;
    if (lower == "manga") return TipoTecho::Manga;
    if (lower == "concreto") return TipoTecho::Concreto;
    return TipoTecho::Otro;
}

// Funciones de TipoProteccion
std::string tipoProteccionToString(TipoProteccion p) {
    switch (p) {
        case TipoProteccion::Completo: return "completo";
        case TipoProteccion::Semi_abierto: return "semi-abierto";
        case TipoProteccion::Malla: return "malla";
        case TipoProteccion::Lona: return "lona";
        case TipoProteccion::Otro: return "otro";
        default: return "desconocido";
    }
}

TipoProteccion stringToTipoProteccion(const std::string& s) {
    std::string lower = s;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    if (lower == "completo") return TipoProteccion::Completo;
    if (lower == "semi-abierto" || lower == "semi" || lower == "semiabierto") return TipoProteccion::Semi_abierto;
    if (lower == "malla") return TipoProteccion::Malla;
    if (lower == "lona") return TipoProteccion::Lona;
    return TipoProteccion::Otro;
}

// Funciones de TipoVentilacion
std::string tipoVentilacionToString(TipoVentilacion v) {
    switch (v) {
        case TipoVentilacion::Natural: return "natural";
        case TipoVentilacion::Forzada: return "forzada";
        case TipoVentilacion::Mixta: return "mixta";
        case TipoVentilacion::Ausente: return "ausente";
        default: return "desconocido";
    }
}

TipoVentilacion stringToTipoVentilacion(const std::string& s) {
    std::string lower = s;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    if (lower == "natural") return TipoVentilacion::Natural;
    if (lower == "forzada") return TipoVentilacion::Forzada;
    if (lower == "mixta") return TipoVentilacion::Mixta;
    if (lower == "ausente" || lower == "sin ventilacion") return TipoVentilacion::Ausente;
    return TipoVentilacion::Natural;
}

// Funciones de Orientacion
std::string orientacionToString(Orientacion o) {
    switch (o) {
        case Orientacion::Norte_Sur: return "norte-sur";
        case Orientacion::Este_Oeste: return "este-oeste";
        default: return "desconocido";
    }
}

Orientacion stringToOrientacion(const std::string& s) {
    std::string lower = s;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    if (lower == "norte-sur" || lower == "norte sur" || lower == "ns") return Orientacion::Norte_Sur;
    if (lower == "este-oeste" || lower == "este oeste" || lower == "eo") return Orientacion::Este_Oeste;
    return Orientacion::Norte_Sur;
}

// Funciones de SistemaComederos
std::string sistemaComederosToString(SistemaComederos s) {
    switch (s) {
        case SistemaComederos::Lineal: return "lineal";
        case SistemaComederos::Circular: return "circular";
        case SistemaComederos::Automatico: return "automatico";
        case SistemaComederos::Manual: return "manual";
        default: return "desconocido";
    }
}

SistemaComederos stringToSistemaComederos(const std::string& s) {
    std::string lower = s;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    if (lower == "lineal") return SistemaComederos::Lineal;
    if (lower == "circular") return SistemaComederos::Circular;
    if (lower == "automatico" || lower == "automático") return SistemaComederos::Automatico;
    return SistemaComederos::Manual;
}

// Funciones de SistemaBebederos
std::string sistemaBebederosToString(SistemaBebederos s) {
    switch (s) {
        case SistemaBebederos::Nipple: return "nipple";
        case SistemaBebederos::Campana: return "campana";
        case SistemaBebederos::Automatico: return "automatico";
        case SistemaBebederos::Manual: return "manual";
        default: return "desconocido";
    }
}

SistemaBebederos stringToSistemaBebederos(const std::string& s) {
    std::string lower = s;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    if (lower == "nipple") return SistemaBebederos::Nipple;
    if (lower == "campana") return SistemaBebederos::Campana;
    if (lower == "automatico" || lower == "automático") return SistemaBebederos::Automatico;
    return SistemaBebederos::Manual;
}

// Funciones de TipoCalefaccion
std::string tipoCalefaccionToString(TipoCalefaccion t) {
    switch (t) {
        case TipoCalefaccion::Gas: return "gas";
        case TipoCalefaccion::Electrico: return "electrico";
        case TipoCalefaccion::Solar: return "solar";
        case TipoCalefaccion::Ninguna: return "ninguna";
        default: return "desconocido";
    }
}

TipoCalefaccion stringToTipoCalefaccion(const std::string& s) {
    std::string lower = s;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    if (lower == "gas") return TipoCalefaccion::Gas;
    if (lower == "electrico" || lower == "eléctrico") return TipoCalefaccion::Electrico;
    if (lower == "solar") return TipoCalefaccion::Solar;
    return TipoCalefaccion::Ninguna;
}

// Funciones de EstadoEquipo
std::string estadoEquipoToString(EstadoEquipo e) {
    switch (e) {
        case EstadoEquipo::Nuevo: return "nuevo";
        case EstadoEquipo::Bueno: return "bueno";
        case EstadoEquipo::Regular: return "regular";
        case EstadoEquipo::Malo: return "malo";
        default: return "desconocido";
    }
}

EstadoEquipo stringToEstadoEquipo(const std::string& s) {
    std::string lower = s;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    if (lower == "nuevo") return EstadoEquipo::Nuevo;
    if (lower == "bueno") return EstadoEquipo::Bueno;
    if (lower == "regular") return EstadoEquipo::Regular;
    if (lower == "malo") return EstadoEquipo::Malo;
    return EstadoEquipo::Bueno;
}

// Calcular peso objetivo por semana (45 días)
double pesoObjetivoSemana(int semana) {
    // Curva de crecimiento típica de pollo de engorde
    switch (semana) {
        case 1: return 0.15;   // 0.15 kg
        case 2: return 0.35;  // 0.35 kg
        case 3: return 0.65;  // 0.65 kg
        case 4: return 1.00;  // 1.00 kg
        case 5: return 1.40;   // 1.40 kg
        case 6: return 1.85;   // 1.85 kg
        case 7: return 2.20;  // 2.20 kg
        default: return 2.20;
    }
}

// Funciones de TipoBeneficio
std::string tipoBeneficioToString(TipoBeneficio t) {
    switch (t) {
        case TipoBeneficio::Manual: return "manual";
        case TipoBeneficio::Automatico: return "automatico";
        case TipoBeneficio::Mixto: return "mixto";
        default: return "desconocido";
    }
}

TipoBeneficio stringToTipoBeneficio(const std::string& s) {
    std::string lower = s;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    if (lower == "manual") return TipoBeneficio::Manual;
    if (lower == "automatico" || lower == "automatic") return TipoBeneficio::Automatico;
    if (lower == "mixto") return TipoBeneficio::Mixto;
    return TipoBeneficio::Manual;
}

// Funciones de EstadoContenedor
std::string estadoContenedorToString(EstadoContenedor e) {
    switch (e) {
        case EstadoContenedor::Disponible: return "disponible";
        case EstadoContenedor::Ocupado: return "ocupado";
        case EstadoContenedor::Mantenimiento: return "mantenimiento";
        case EstadoContenedor::Desactivado: return "desactivado";
        default: return "desconocido";
    }
}

EstadoContenedor stringToEstadoContenedor(const std::string& s) {
    std::string lower = s;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    if (lower == "disponible") return EstadoContenedor::Disponible;
    if (lower == "ocupado") return EstadoContenedor::Ocupado;
    if (lower == "mantenimiento") return EstadoContenedor::Mantenimiento;
    if (lower == "desactivado") return EstadoContenedor::Desactivado;
    return EstadoContenedor::Disponible;
}