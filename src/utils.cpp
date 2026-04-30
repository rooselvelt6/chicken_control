#include "../include/utils.h"
#include "../include/modelos.h"
#include <algorithm>
#include <iostream>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>

std::string fechaActual() {
    time_t now = time(nullptr);
    tm* ltm = localtime(&now);
    std::stringstream ss;
    ss << 1900 + ltm->tm_year << "-"
       << std::setw(2) << std::setfill('0') << 1 + ltm->tm_mon << "-"
       << std::setw(2) << std::setfill('0') << ltm->tm_mday;
    return ss.str();
}

int calcularDias(const std::string& fecha_inicio, const std::string& fecha_fin) {
    try {
        int anio1, mes1, dia1, anio2, mes2, dia2;
        char sep;
        std::stringstream ss1(fecha_inicio);
        ss1 >> anio1 >> sep >> mes1 >> sep >> dia1;
        std::stringstream ss2(fecha_fin);
        ss2 >> anio2 >> sep >> mes2 >> sep >> dia2;

        tm t1 = {}, t2 = {};
        t1.tm_year = anio1 - 1900; t1.tm_mon = mes1 - 1; t1.tm_mday = dia1;
        t2.tm_year = anio2 - 1900; t2.tm_mon = mes2 - 1; t2.tm_mday = dia2;

        time_t time1 = mktime(&t1);
        time_t time2 = mktime(&t2);
        return static_cast<int>(difftime(time2, time1) / (60 * 60 * 24));
    } catch (...) {
        return 0;
    }
}

Fase clasificarFaseDias(int dias) {
    if (dias <= 26) return Fase::Bebe;
    else if (dias <= 45) return Fase::Intermedio;
    else return Fase::Grande;
}

std::string floatListToString(const std::vector<float>& lista) {
    std::string result;
    for (size_t i = 0; i < lista.size(); i++) {
        if (i > 0) result += ",";
        result += std::to_string(lista[i]);
    }
    return result;
}

std::vector<float> stringToFloatList(const std::string& s) {
    std::vector<float> result;
    if (s.empty()) return result;
    size_t pos = 0;
    while (pos < s.size()) {
        size_t comma = s.find(',', pos);
        std::string token = (comma == std::string::npos) ? s.substr(pos) : s.substr(pos, comma - pos);
        try {
            result.push_back(std::stof(token));
        } catch (...) {}
        if (comma == std::string::npos) break;
        pos = comma + 1;
    }
    return result;
}

std::string stringListToString(const std::vector<std::string>& lista) {
    std::string result;
    for (size_t i = 0; i < lista.size(); i++) {
        if (i > 0) result += ",";
        result += lista[i];
    }
    return result;
}

std::vector<std::string> stringToStringList(const std::string& s) {
    std::vector<std::string> result;
    if (s.empty()) return result;
    size_t pos = 0;
    while (pos < s.size()) {
        size_t comma = s.find(',', pos);
        std::string token = (comma == std::string::npos) ? s.substr(pos) : s.substr(pos, comma - pos);
        result.push_back(token);
        if (comma == std::string::npos) break;
        pos = comma + 1;
    }
    return result;
}

double calcularPrecioUSD(double precio_ves, double precio_dolar) {
    return (precio_dolar > 0.0) ? precio_ves / precio_dolar : 0.0;
}

void printSeparator() {
    std::cout << std::string(50, '-') << std::endl;
}

void printSeparatorLong() {
    std::cout << std::string(80, '=') << std::endl;
}

void printHeader(const std::string& titulo) {
    printSeparatorLong();
    std::cout << "  " << titulo << std::endl;
    printSeparatorLong();
}

std::string boolToString(bool b) {
    return b ? "Sí" : "No";
}

bool isNotEmpty(const std::string& s) {
    std::string trimmed = s;
    trimmed.erase(remove_if(trimmed.begin(), trimmed.end(), ::isspace), trimmed.end());
    return !trimmed.empty();
}

std::string capitalize(const std::string& s) {
    if (s.empty()) return s;
    std::string result = s;
    result[0] = std::toupper(result[0]);
    return result;
}

std::string sanitizarSQL(const std::string& input) {
    std::string result;
    for (char c : input) {
        if (c == '\'' || c == ';' || c == '-' || c == '=' || c == '(' || c == ')') {
            continue;
        }
        result += c;
    }
    return result;
}

bool esNumeroValido(const std::string& s) {
    if (s.empty()) return false;
    for (char c : s) {
        if (!std::isdigit(c)) return false;
    }
    return true;
}

int parsearIdSeguro(const std::string& s) {
    if (!esNumeroValido(s)) return -1;
    try {
        return std::stoi(s);
    } catch (...) {
        return -1;
    }
}