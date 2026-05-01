.PHONY: all clean build run help install test test-run

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
LDFLAGS = -lsqlite3 -lpthread -ldl -lncurses
TEST_LDFLAGS = -lsqlite3 -lpthread -ldl

SRC = src/main.cpp src/modelos.cpp src/utils.cpp src/base_datos.cpp \
      src/lotes.cpp src/animales.cpp src/alimentacion.cpp src/ventas.cpp \
      src/inversores.cpp src/herramientas.cpp src/reportes.cpp \
      src/granjas.cpp src/corrales.cpp src/proveedores.cpp \
      src/veterinaria.cpp src/pesaje.cpp src/metricas.cpp \
      src/ui.cpp src/alertas.cpp src/facturacion.cpp \
      src/contenedores.cpp src/beneficio.cpp \
      src/dashboard.cpp src/empleados.cpp src/temperatura.cpp src/servicios.cpp \
      src/financiero.cpp src/graficos.cpp src/reportes_txt.cpp

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
	rm -rf build/*.o $(TARGET) build/test

install: build
	cp $(TARGET) /usr/local/bin/granja

test: build/test
	./build/test

build/test: tests/test_main.cpp src/utils.o src/modelos.o
	@mkdir -p build
	$(CXX) $(CXXFLAGS) tests/test_main.cpp src/utils.o src/modelos.o -o build/test $(TEST_LDFLAGS)

test-run: build/test
	./build/test

help:
	@echo ""
	@echo "╔═══════════════════════════════════════════════════════════╗"
	@echo "║          🐔 CHICKEN CONTROL v3.1 - C++                    ║"
	@echo "║          Nivel de Seguridad: 10/10                       ║"
	@echo "╚═══════════════════════════════════════════════════════════╝"
	@echo ""
	@echo "Usage:"
	@echo "  make build          - Compilar el proyecto"
	@echo "  make run            - Ejecutar el sistema"
	@echo "  make clean          - Limpiar archivos de compilación"
	@echo "  make install       - Instalar globalmente (requiere root)"
	@echo "  make test           - Compilar tests unitarios"
	@echo "  make test-run      - Ejecutar tests unitarios"
	@echo "  make help           - Mostrar esta ayuda"
	@echo ""
	@echo "Instalación automática:"
	@echo "  ./instalar.sh                - Modo interactivo"
	@echo "  sudo ./instalar.sh --install - Instalar y configurar"
	@echo "  ./instalar.sh --update       - Actualizar y recompilar"
	@echo ""
	@echo "Requisitos:"
	@echo "  Ubuntu/Debian: sudo apt install g++ make libsqlite3-dev libncurses-dev"
	@echo "  Fedora/RHEL:   sudo dnf install gcc gcc-c++ make sqlite sqlite-devel ncurses-devel"
	@echo "  Arch Linux:    sudo pacman -S base-devel sqlite ncurses"

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