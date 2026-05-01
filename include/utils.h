#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include "modelos.h"

std::string fechaActual();
int calcularDias(const std::string& fecha_inicio, const std::string& fecha_fin);
std::string faseToString(Fase f);
Fase stringToFase(const std::string& s);
std::string causaMuerteToString(CausaMuerte c);
CausaMuerte stringToCausaMuerte(const std::string& s);
Fase clasificarFaseDias(int dias);
std::string floatListToString(const std::vector<float>& lista);
std::vector<float> stringToFloatList(const std::string& s);
std::string stringListToString(const std::vector<std::string>& lista);
std::vector<std::string> stringToStringList(const std::string& s);
void printSeparator();
void printSeparatorLong();
void printHeader(const std::string& titulo);
std::string boolToString(bool b);
bool isNotEmpty(const std::string& s);
std::string capitalize(const std::string& s);

std::string sanitizarSQL(const std::string& input);
std::string sanitizarInput(const std::string& input, int maxLen);
std::string sanitizarTelefono(const std::string& telefono);
bool esNumeroValido(const std::string& s);
int parsearIdSeguro(const std::string& s);

#endif // UTILS_H