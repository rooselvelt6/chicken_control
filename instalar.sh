#!/bin/bash

# Script de instalación para el Sistema de Control de Granja de Pollos (C++)
# Fedora Linux

set -e

echo "====================================="
echo "  INSTALACIÓN - Granja de Pollos C++"
echo "====================================="
echo ""

# Verificar si es root
if [ "$EUID" -eq 0 ]; then
    echo "Ejecutando como root..."
    SUDO=""
else
    echo "Se requiere privilegios de administrador"
    SUDO="sudo"
fi

# Instalar dependencias
echo "1. Instalando dependencias..."
$SUDO dnf install -y gcc gcc-c++ make sqlite sqlite-devel

echo "2. Verificando instalación..."
gcc --version | head -1
sqlite3 --version

# Crear directorio de datos
echo "3. Creando directorio de datos..."
mkdir -p datos

# Compilar proyecto
echo "4. Compilando proyecto..."
cd "$(dirname "$0")"
make build

echo ""
echo "====================================="
echo "  INSTALACIÓN COMPLETADA"
echo "====================================="
echo ""
echo "Para ejecutar el sistema:"
echo "  ./build/granja ayuda"
echo ""
echo "Para cargar datos de ejemplo:"
echo "  ./build/granja ejemplo"
echo ""
echo "Para ver reportes:"
echo "  ./build/granja reporte inventario"
echo "  ./build/granja reporte financiero"
echo ""