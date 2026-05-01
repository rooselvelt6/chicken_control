#include "test_framework.h"
#include "../include/utils.h"
#include "../include/modelos.h"
#include <cmath>
#include <cstring>

TEST_CASE(sanitizarSQL_basico) {
    CHECK(sanitizarSQL("test") == "test");
    CHECK(sanitizarSQL("test's") == "test''s");
    CHECK(sanitizarSQL("test; DROP") == "valor_invalido");
}

TEST_CASE(sanitizarSQL_block_SQL_injection) {
    CHECK_EQ(sanitizarSQL("test UNION SELECT"), "valor_invalido");
    CHECK_EQ(sanitizarSQL("test SELECT * FROM"), "valor_invalido");
    CHECK_EQ(sanitizarSQL("test; DELETE FROM"), "valor_invalido");
}

TEST_CASE(sanitizarInput_basico) {
    CHECK(sanitizarInput("test", 10) == "test");
    CHECK(sanitizarInput("test muy largo", 5).length() <= 5);
    CHECK(sanitizarInput("", 10) == "");
}

TEST_CASE(sanitizarInput_caracteres_validos) {
    CHECK(sanitizarInput("hola mundo", 20) == "hola mundo");
    CHECK(sanitizarInput("numero 123", 20) == "numero 123");
}

TEST_CASE(sanitizarTelefono_basico) {
    CHECK(sanitizarTelefono("04121234567") == "04121234567");
    CHECK(sanitizarTelefono("+58 412 123 4567") == "+58 412 123 4567");
    CHECK(sanitizarTelefono("0412-123-4567") == "0412-123-4567");
}

TEST_CASE(sanitizarTelefono_limpia) {
    CHECK(sanitizarTelefono("0412abc1234") == "04121234");
}

TEST_CASE(esNumeroValido) {
    CHECK(esNumeroValido("123") == true);
    CHECK(esNumeroValido("0") == true);
    CHECK(esNumeroValido("") == false);
    CHECK(esNumeroValido("12a") == false);
    CHECK(esNumeroValido("12.5") == false);
}

TEST_CASE(parsearIdSeguro) {
    CHECK_EQ(parsearIdSeguro("123"), 123);
    CHECK_EQ(parsearIdSeguro("0"), 0);
    CHECK_EQ(parsearIdSeguro("-1"), -1);
    CHECK_EQ(parsearIdSeguro("abc"), -1);
    CHECK_EQ(parsearIdSeguro(""), -1);
    CHECK_EQ(parsearIdSeguro("12.5"), -1);
}

TEST_CASE(fechaActual_formato) {
    std::string fecha = fechaActual();
    CHECK(fecha.length() == 10);
    CHECK(fecha[4] == '-');
    CHECK(fecha[7] == '-');
}

TEST_CASE(calcularDias) {
    CHECK_EQ(calcularDias("2024-01-01", "2024-01-11"), 10);
    CHECK_EQ(calcularDias("2024-01-01", "2024-01-01"), 0);
    CHECK(calcularDias("2024-12-31", "2024-01-01") < 0);
}

TEST_CASE(faseToString) {
    CHECK(faseToString(Fase::Bebe) == "Bebe");
    CHECK(faseToString(Fase::Intermedio) == "Intermedio");
    CHECK(faseToString(Fase::Grande) == "Grande");
}

TEST_CASE(stringToFase) {
    CHECK(stringToFase("Bebe") == Fase::Bebe);
    CHECK(stringToFase("Grande") == Fase::Grande);
    CHECK(stringToFase("Desconocido") == Fase::Desconocido);
}

TEST_CASE(causaMuerteToString) {
    CHECK(causaMuerteToString(CausaMuerte::Aplastamiento) == "Aplastamiento");
    CHECK(causaMuerteToString(CausaMuerte::Ahogamiento) == "Ahogamiento");
}

TEST_CASE(stringToCausaMuerte) {
    CHECK(stringToCausaMuerte("Aplastamiento") == CausaMuerte::Aplastamiento);
    CHECK(stringToCausaMuerte("Desconocida") == CausaMuerte::Desconocida);
}

TEST_CASE(clasificarFaseDias) {
    CHECK(clasificarFaseDias(0) == Fase::Bebe);
    CHECK(clasificarFaseDias(20) == Fase::Bebe);
    CHECK(clasificarFaseDias(30) == Fase::Intermedio);
    CHECK(clasificarFaseDias(50) == Fase::Grande);
    CHECK(clasificarFaseDias(100) == Fase::Grande);
}

TEST_CASE(floatListToString) {
    std::vector<float> nums = {1.5f, 2.5f, 3.0f};
    std::string str = floatListToString(nums);
    CHECK(str.find("1.5") != std::string::npos);
}

TEST_CASE(stringToFloatList) {
    std::string str = "1.5,2.5,3.0";
    std::vector<float> parsed = stringToFloatList(str);
    CHECK(parsed.size() == 3);
    CHECK(std::abs(parsed[0] - 1.5f) < 0.01f);
}

TEST_CASE(stringListToString) {
    std::vector<std::string> lista = {"uno", "dos", "tres"};
    std::string str = stringListToString(lista);
    CHECK(str == "uno,dos,tres");
}

TEST_CASE(stringToStringList) {
    std::string str = "uno,dos,tres";
    std::vector<std::string> parsed = stringToStringList(str);
    CHECK(parsed.size() == 3);
    CHECK(parsed[0] == "uno");
}

TEST_CASE(capitalize) {
    CHECK(capitalize("hola") == "Hola");
    CHECK(capitalize("HOLA") == "HOLA");
    CHECK(capitalize("") == "");
    CHECK(capitalize("a") == "A");
}

TEST_CASE(isNotEmpty) {
    CHECK(isNotEmpty("test") == true);
    CHECK(isNotEmpty("  test  ") == true);
    CHECK(isNotEmpty("") == false);
    CHECK(isNotEmpty("   ") == false);
}

TEST_CASE(boolToString) {
    CHECK(boolToString(true) == "Sí");
    CHECK(boolToString(false) == "No");
}

TEST_CASE(longitud_maxima_sanitizarSQL) {
    std::string input(600, 'a');
    std::string result = sanitizarSQL(input);
    CHECK(result.length() <= 500);
}

TEST_CASE(edge_cases) {
    CHECK(sanitizarSQL("") == "");
    CHECK(sanitizarInput("test", 0) == "");
    CHECK(sanitizarInput("test", -5) == "");
    CHECK(sanitizarTelefono("") == "");
    CHECK(sanitizarTelefono("123456789012345678901") == "12345678901234567890");
    CHECK(esNumeroValido("000") == true);
    CHECK(esNumeroValido(" ") == false);
    CHECK(parsearIdSeguro("999999999") > 0);
    CHECK(parsearIdSeguro("abc123") == -1);
}

int main() {
    std::cout << "========== CHICKEN CONTROL TESTS ==========" << std::endl << std::endl;

    RUN_TEST(sanitizarSQL_basico);
    RUN_TEST(sanitizarSQL_block_SQL_injection);
    RUN_TEST(sanitizarInput_basico);
    RUN_TEST(sanitizarInput_caracteres_validos);
    RUN_TEST(sanitizarTelefono_basico);
    RUN_TEST(sanitizarTelefono_limpia);
    RUN_TEST(esNumeroValido);
    RUN_TEST(parsearIdSeguro);
    RUN_TEST(fechaActual_formato);
    RUN_TEST(calcularDias);
    RUN_TEST(faseToString);
    RUN_TEST(stringToFase);
    RUN_TEST(causaMuerteToString);
    RUN_TEST(stringToCausaMuerte);
    RUN_TEST(clasificarFaseDias);
    RUN_TEST(floatListToString);
    RUN_TEST(stringToFloatList);
    RUN_TEST(stringListToString);
    RUN_TEST(stringToStringList);
    RUN_TEST(capitalize);
    RUN_TEST(isNotEmpty);
    RUN_TEST(boolToString);
    RUN_TEST(longitud_maxima_sanitizarSQL);
    RUN_TEST(edge_cases);

    PRINT_RESULTS();
}