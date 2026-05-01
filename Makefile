.PHONY: all clean build run help install

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
LDFLAGS = -lsqlite3 -lpthread -ldl -lncurses

SRC = src/main.cpp src/modelos.cpp src/utils.cpp src/base_datos.cpp \
      src/lotes.cpp src/animales.cpp src/alimentacion.cpp src/ventas.cpp \
      src/inversores.cpp src/herramientas.cpp src/reportes.cpp \
      src/granjas.cpp src/corrales.cpp src/proveedores.cpp \
      src/veterinaria.cpp src/pesaje.cpp src/metricas.cpp \
      src/ui.cpp src/alertas.cpp

OBJ = $(SRC:.cpp=.o)
TARGET = build/granja

all: build

build: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp include/*.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: build
	./$(TARGET)

clean:
	rm -rf build/*.o $(TARGET)

install: build
	cp $(TARGET) /usr/local/bin/granja

help:
	@echo "Sistema de Control de Granja de Pollos v2.0 - C++"
	@echo ""
	@echo "Para compilar: make build"
	@echo "Para ejecutar: make run"
	@echo "Para limpiar: make clean"
	@echo "Para pruebas: make benchmark"
	@echo ""
	@echo "Antes de compilar, asegurese de tener sqlite3 instalado:"
	@echo "  sudo dnf install sqlite sqlite-devel"

BENCHMARK_SRC = src/benchmark.cpp src/modelos.cpp src/utils.cpp src/base_datos.cpp \
                src/lotes.cpp src/animales.cpp src/alimentacion.cpp src/ventas.cpp \
                src/inversores.cpp src/herramientas.cpp src/reportes.cpp \
                src/granjas.cpp src/corrales.cpp src/proveedores.cpp \
                src/veterinaria.cpp src/pesaje.cpp src/metricas.cpp

BENCHMARK_OBJ = $(BENCHMARK_SRC:.cpp=.o)
BENCHMARK_TARGET = build/benchmark

benchmark: $(BENCHMARK_TARGET)
	./$(BENCHMARK_TARGET)

$(BENCHMARK_TARGET): $(BENCHMARK_OBJ)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)