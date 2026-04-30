#include "../include/corrales.h"
#include "../include/base_datos.h"
#include "../include/modelos.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cmath>

int Corrales::crear(int granja_id, int numero, const std::string& nombre,
                    double largo, double ancho, double altura,
                    const std::string& tipo_techo, const std::string& proteccion,
                    const std::string& ventilacion, const std::string& orientacion,
                    const std::string& material_paredes,
                    const std::string& sistema_comederos, const std::string& sistema_bebederos,
                    bool tiene_iluminacion, bool tiene_calefaccion, const std::string& tipo_calefaccion,
                    bool tiene_drenaje, int capacidad_maxima, int densidad_maxima) {
    
    double area = largo * ancho;
    if (capacidad_maxima == 0) {
        capacidad_maxima = (int)(area * densidad_maxima);
    }
    
    auto* db = BaseDatos::getInstancia();
    std::stringstream ss;
    ss << "INSERT INTO corrales (granja_id, numero, nombre, largo_m, ancho_m, altura_m, area_m2, "
       << "tipo_techo, proteccion, ventilacion, orientacion, material_paredes, "
       << "sistema_comederos, sistema_bebederos, tiene_iluminacion, tiene_calefaccion, "
       << "tipo_calefaccion, tiene_drenaje, capacidad_maxima, densidad_maxima) VALUES ("
       << granja_id << ", " << numero << ", '" << nombre << "', "
       << largo << ", " << ancho << ", " << altura << ", " << area << ", '"
       << tipo_techo << "', '" << proteccion << "', '" << ventilacion << "', '"
       << orientacion << "', '" << material_paredes << "', '" << sistema_comederos << "', '"
       << sistema_bebederos << "', " << (tiene_iluminacion ? 1 : 0) << ", "
       << (tiene_calefaccion ? 1 : 0) << ", '" << tipo_calefaccion << "', "
       << (tiene_drenaje ? 1 : 0) << ", " << capacidad_maxima << ", " << densidad_maxima << ")";
    return db->insertarYGetId(ss.str());
}

Corral* Corrales::obtener(int id) {
    auto* db = BaseDatos::getInstancia();
    std::stringstream ss;
    ss << "SELECT id, granja_id, numero, nombre, largo_m, ancho_m, altura_m, area_m2, "
       << "tipo_techo, proteccion, ventilacion, orientacion, material_paredes, "
       << "sistema_comederos, sistema_bebederos, tiene_iluminacion, tiene_calefaccion, "
       << "tipo_calefaccion, tiene_drenaje, capacidad_maxima, densidad_maxima, activo "
       << "FROM corrales WHERE id = " << id;
    sqlite3_stmt* stmt;
    static Corral c;
    if (sqlite3_prepare_v2(db->abrir(), ss.str().c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            c.id = sqlite3_column_int(stmt, 0);
            c.granja_id = sqlite3_column_int(stmt, 1);
            c.numero = sqlite3_column_int(stmt, 2);
            const char* n = (const char*)sqlite3_column_text(stmt, 3);
            c.nombre = n ? n : "";
            c.largo_m = sqlite3_column_double(stmt, 4);
            c.ancho_m = sqlite3_column_double(stmt, 5);
            c.altura_m = sqlite3_column_double(stmt, 6);
            c.area_m2 = sqlite3_column_double(stmt, 7);
            const char* t = (const char*)sqlite3_column_text(stmt, 8);
            c.tipo_techo = stringToTipoTecho(t ? t : "zinc");
            const char* p = (const char*)sqlite3_column_text(stmt, 9);
            c.proteccion = stringToTipoProteccion(p ? p : "completo");
            const char* v = (const char*)sqlite3_column_text(stmt, 10);
            c.ventilacion = stringToTipoVentilacion(v ? v : "natural");
            const char* o = (const char*)sqlite3_column_text(stmt, 11);
            c.orientacion = stringToOrientacion(o ? o : "norte-sur");
            const char* mp = (const char*)sqlite3_column_text(stmt, 12);
            c.material_paredes = mp ? mp : "";
            const char* sc = (const char*)sqlite3_column_text(stmt, 13);
            c.sistema_comederos = stringToSistemaComederos(sc ? sc : "lineal");
            const char* sb = (const char*)sqlite3_column_text(stmt, 14);
            c.sistema_bebederos = stringToSistemaBebederos(sb ? sb : "campana");
            c.tiene_iluminacion = sqlite3_column_int(stmt, 15) == 1;
            c.tiene_calefaccion = sqlite3_column_int(stmt, 16) == 1;
            const char* tc = (const char*)sqlite3_column_text(stmt, 17);
            c.tipo_calefaccion = stringToTipoCalefaccion(tc ? tc : "ninguna");
            c.tiene_drenaje = sqlite3_column_int(stmt, 18) == 1;
            c.capacidad_maxima = sqlite3_column_int(stmt, 19);
            c.densidad_maxima = sqlite3_column_int(stmt, 20);
            c.activo = sqlite3_column_int(stmt, 21) == 1;
            sqlite3_finalize(stmt);
            return &c;
        }
    }
    sqlite3_finalize(stmt);
    return nullptr;
}

std::vector<Corral> Corrales::listarPorGranja(int granja_id, bool activos) {
    auto* db = BaseDatos::getInstancia();
    std::string sql = "SELECT id, granja_id, numero, nombre, largo_m, ancho_m, altura_m, area_m2, "
                      "tipo_techo, proteccion, ventilacion, orientacion, material_paredes, "
                      "sistema_comederos, sistema_bebederos, tiene_iluminacion, tiene_calefaccion, "
                      "tipo_calefaccion, tiene_drenaje, capacidad_maxima, densidad_maxima, activo "
                      "FROM corrales WHERE granja_id = " + std::to_string(granja_id);
    if (activos) sql += " AND activo = 1";
    sql += " ORDER BY numero";
    
    std::vector<Corral> resultado;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Corral c;
            c.id = sqlite3_column_int(stmt, 0);
            c.granja_id = sqlite3_column_int(stmt, 1);
            c.numero = sqlite3_column_int(stmt, 2);
            const char* n = (const char*)sqlite3_column_text(stmt, 3);
            c.nombre = n ? n : "";
            c.largo_m = sqlite3_column_double(stmt, 4);
            c.ancho_m = sqlite3_column_double(stmt, 5);
            c.altura_m = sqlite3_column_double(stmt, 6);
            c.area_m2 = sqlite3_column_double(stmt, 7);
            const char* t = (const char*)sqlite3_column_text(stmt, 8);
            c.tipo_techo = stringToTipoTecho(t ? t : "zinc");
            const char* p = (const char*)sqlite3_column_text(stmt, 9);
            c.proteccion = stringToTipoProteccion(p ? p : "completo");
            const char* v = (const char*)sqlite3_column_text(stmt, 10);
            c.ventilacion = stringToTipoVentilacion(v ? v : "natural");
            const char* o = (const char*)sqlite3_column_text(stmt, 11);
            c.orientacion = stringToOrientacion(o ? o : "norte-sur");
            const char* mp = (const char*)sqlite3_column_text(stmt, 12);
            c.material_paredes = mp ? mp : "";
            const char* sc = (const char*)sqlite3_column_text(stmt, 13);
            c.sistema_comederos = stringToSistemaComederos(sc ? sc : "lineal");
            const char* sb = (const char*)sqlite3_column_text(stmt, 14);
            c.sistema_bebederos = stringToSistemaBebederos(sb ? sb : "campana");
            c.tiene_iluminacion = sqlite3_column_int(stmt, 15) == 1;
            c.tiene_calefaccion = sqlite3_column_int(stmt, 16) == 1;
            const char* tc = (const char*)sqlite3_column_text(stmt, 17);
            c.tipo_calefaccion = stringToTipoCalefaccion(tc ? tc : "ninguna");
            c.tiene_drenaje = sqlite3_column_int(stmt, 18) == 1;
            c.capacidad_maxima = sqlite3_column_int(stmt, 19);
            c.densidad_maxima = sqlite3_column_int(stmt, 20);
            c.activo = sqlite3_column_int(stmt, 21) == 1;
            resultado.push_back(c);
        }
    }
    sqlite3_finalize(stmt);
    return resultado;
}

std::vector<Corral> Corrales::listarTodos(bool activos) {
    auto* db = BaseDatos::getInstancia();
    std::string sql = "SELECT id, granja_id, numero, nombre, largo_m, ancho_m, altura_m, area_m2, "
                      "tipo_techo, proteccion, ventilacion, orientacion, material_paredes, "
                      "sistema_comederos, sistema_bebederos, tiene_iluminacion, tiene_calefaccion, "
                      "tipo_calefaccion, tiene_drenaje, capacidad_maxima, densidad_maxima, activo "
                      "FROM corrales";
    if (activos) sql += " WHERE activo = 1";
    sql += " ORDER BY numero";
    
    std::vector<Corral> resultado;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->abrir(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Corral c;
            c.id = sqlite3_column_int(stmt, 0);
            c.granja_id = sqlite3_column_int(stmt, 1);
            c.numero = sqlite3_column_int(stmt, 2);
            const char* n = (const char*)sqlite3_column_text(stmt, 3);
            c.nombre = n ? n : "";
            c.largo_m = sqlite3_column_double(stmt, 4);
            c.ancho_m = sqlite3_column_double(stmt, 5);
            c.altura_m = sqlite3_column_double(stmt, 6);
            c.area_m2 = sqlite3_column_double(stmt, 7);
            const char* t = (const char*)sqlite3_column_text(stmt, 8);
            c.tipo_techo = stringToTipoTecho(t ? t : "zinc");
            const char* p = (const char*)sqlite3_column_text(stmt, 9);
            c.proteccion = stringToTipoProteccion(p ? p : "completo");
            const char* v = (const char*)sqlite3_column_text(stmt, 10);
            c.ventilacion = stringToTipoVentilacion(v ? v : "natural");
            const char* o = (const char*)sqlite3_column_text(stmt, 11);
            c.orientacion = stringToOrientacion(o ? o : "norte-sur");
            const char* mp = (const char*)sqlite3_column_text(stmt, 12);
            c.material_paredes = mp ? mp : "";
            const char* sc = (const char*)sqlite3_column_text(stmt, 13);
            c.sistema_comederos = stringToSistemaComederos(sc ? sc : "lineal");
            const char* sb = (const char*)sqlite3_column_text(stmt, 14);
            c.sistema_bebederos = stringToSistemaBebederos(sb ? sb : "campana");
            c.tiene_iluminacion = sqlite3_column_int(stmt, 15) == 1;
            c.tiene_calefaccion = sqlite3_column_int(stmt, 16) == 1;
            const char* tc = (const char*)sqlite3_column_text(stmt, 17);
            c.tipo_calefaccion = stringToTipoCalefaccion(tc ? tc : "ninguna");
            c.tiene_drenaje = sqlite3_column_int(stmt, 18) == 1;
            c.capacidad_maxima = sqlite3_column_int(stmt, 19);
            c.densidad_maxima = sqlite3_column_int(stmt, 20);
            c.activo = sqlite3_column_int(stmt, 21) == 1;
            resultado.push_back(c);
        }
    }
    sqlite3_finalize(stmt);
    return resultado;
}

void Corrales::cerrar(int id) {
    auto* db = BaseDatos::getInstancia();
    std::stringstream ss;
    ss << "UPDATE corrales SET activo = 0 WHERE id = " << id;
    db->ejecutarSQL(ss.str());
}

int Corrales::calcularCapacidad(double area_m2, int densidad) {
    return (int)(area_m2 * densidad);
}

double Corrales::calcularArea(double largo, double ancho) {
    return largo * ancho;
}

void Corrales::imprimirCorral(const Corral& c) {
    std::cout << "Corral #" << c.numero << " - " << c.nombre << " (ID: " << c.id << ")" << std::endl;
    std::cout << "  Dimensiones: " << std::fixed << std::setprecision(1) << c.largo_m << "m x " 
              << c.ancho_m << "m x " << c.altura_m << "m (Area: " << c.area_m2 << " m2)" << std::endl;
    std::cout << "  Capacidad: " << c.capacidad_maxima << " pollos (densidad: " << c.densidad_maxima << "/m2)" << std::endl;
    std::cout << "  Techo: " << tipoTechoToString(c.tipo_techo) << " | Protección: " << tipoProteccionToString(c.proteccion) << std::endl;
    std::cout << "  Ventilación: " << tipoVentilacionToString(c.ventilacion) << " | Orientación: " << orientacionToString(c.orientacion) << std::endl;
    std::cout << "  Estado: " << (c.activo ? "ACTIVO" : "INACTIVO") << std::endl;
}

void Corrales::imprimirCompleto(const Corral& c) {
    std::cout << "================================================================================" << std::endl;
    std::cout << "  CORRAL #" << c.numero << " - " << c.nombre << std::endl;
    std::cout << "================================================================================" << std::endl;
    std::cout << "ID: " << c.id << " | Granja: " << c.granja_id << std::endl << std::endl;
    
    std::cout << "--- DIMENSIONES ---" << std::endl;
    std::cout << "Largo: " << std::fixed << std::setprecision(2) << c.largo_m << " m" << std::endl;
    std::cout << "Ancho: " << c.ancho_m << " m" << std::endl;
    std::cout << "Altura: " << c.altura_m << " m" << std::endl;
    std::cout << "Area: " << c.area_m2 << " m2" << std::endl << std::endl;
    
    std::cout << "--- CARACTERISTICAS ---" << std::endl;
    std::cout << "Techo: " << tipoTechoToString(c.tipo_techo) << std::endl;
    std::cout << "Protección: " << tipoProteccionToString(c.proteccion) << std::endl;
    std::cout << "Ventilación: " << tipoVentilacionToString(c.ventilacion) << std::endl;
    std::cout << "Orientación: " << orientacionToString(c.orientacion) << std::endl;
    std::cout << "Material paredes: " << c.material_paredes << std::endl << std::endl;
    
    std::cout << "--- SISTEMAS ---" << std::endl;
    std::cout << "Comederos: " << sistemaComederosToString(c.sistema_comederos) << std::endl;
    std::cout << "Bebederos: " << sistemaBebederosToString(c.sistema_bebederos) << std::endl;
    std::cout << "Iluminación: " << (c.tiene_iluminacion ? "Sí" : "No") << std::endl;
    std::cout << "Calefacción: " << (c.tiene_calefaccion ? "Sí (" + tipoCalefaccionToString(c.tipo_calefaccion) + ")" : "No") << std::endl;
    std::cout << "Drenaje: " << (c.tiene_drenaje ? "Sí" : "No") << std::endl << std::endl;
    
    std::cout << "--- CAPACIDAD ---" << std::endl;
    std::cout << "Capacidad máxima: " << c.capacidad_maxima << " pollos" << std::endl;
    std::cout << "Densidad máxima: " << c.densidad_maxima << " pollos/m2" << std::endl;
}