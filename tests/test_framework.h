#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <iostream>
#include <string>
#include <sstream>

static int tests_passed = 0;
static int tests_failed = 0;

#define CHECK(expr) \
    do { \
        if (expr) { \
            tests_passed++; \
        } else { \
            tests_failed++; \
            std::cerr << "FAIL: " << #expr << " in " << __FILE__ << ":" << __LINE__ << std::endl; \
        } \
    } while(0)

#define CHECK_EQ(a, b) \
    do { \
        if ((a) == (b)) { \
            tests_passed++; \
        } else { \
            tests_failed++; \
            std::cerr << "FAIL: " << #a << " == " << #b << " (" << (a) << " vs " << (b) << ") in " << __FILE__ << ":" << __LINE__ << std::endl; \
        } \
    } while(0)

#define CHECK_NE(a, b) \
    do { \
        if ((a) != (b)) { \
            tests_passed++; \
        } else { \
            tests_failed++; \
            std::cerr << "FAIL: " << #a << " != " << #b << " in " << __FILE__ << ":" << __LINE__ << std::endl; \
        } \
    } while(0)

#define TEST_CASE(name) void test_##name()

#define RUN_TEST(name) \
    do { \
        std::cout << "Running test: " << #name << "... "; \
        test_##name(); \
        std::cout << "OK" << std::endl; \
    } while(0)

#define PRINT_RESULTS() \
    do { \
        std::cout << "\n========== TEST RESULTS ==========" << std::endl; \
        std::cout << "Passed: " << tests_passed << std::endl; \
        std::cout << "Failed: " << tests_failed << std::endl; \
        std::cout << "Total:  " << (tests_passed + tests_failed) << std::endl; \
        return tests_failed > 0 ? 1 : 0; \
    } while(0)

#endif