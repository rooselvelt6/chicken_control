#!/bin/bash

set -e

ROJO="\033[0;31m"
VERDE="\033[0;32m"
AMARILLO="\033[1;33m"
AZUL="\033[0;34m"
CYAN="\033[0;36m"
BLANCO="\033[1;37m"
NC="\033[0m"

mostrar_banner() {
    clear
    echo -e "${CYAN}"
    echo "╔═══════════════════════════════════════════════════════════════╗"
    echo "║                                                               ║"
    echo "║   🐔  CHICKEN CONTROL v3.0 - INSTALADOR                      ║"
    echo "║   Sistema Inteligente de Gestión Avícola                    ║"
    echo "║                                                               ║"
    echo "╚═══════════════════════════════════════════════════════════════╝"
    echo -e "${NC}"
}

verificar_root() {
    if [ "$EUID" -eq 0 ]; then
        echo -e "${VERDE}✓${NC} Ejecutando como root"
        SUDO=""
    else
        echo -e "${AMARILLO}⚠${NC} Se requieren privilegios de administrador"
        SUDO="sudo"
    fi
}

detectar_distro() {
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        DISTRO=$ID
    elif [ -f /etc/debian_version ]; then
        DISTRO="debian"
    elif [ -f /etc/fedora-release ]; then
        DISTRO="fedora"
    elif [ -f /etc/arch-release ]; then
        DISTRO="arch"
    else
        DISTRO="unknown"
    fi
    echo -e "${AZUL}▸${NC} Distribución detectada: ${BLANCO}$DISTRO${NC}"
}

instalar_dependencias() {
    echo ""
    echo -e "${AMARILLO}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${CYAN}  📦 INSTALANDO DEPENDENCIAS${NC}"
    echo -e "${AMARILLO}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"

    case $DISTRO in
        debian|ubuntu|linuxmint)
            echo -e "${AZUL}▸${NC} Instalando dependencias para Debian/Ubuntu..."
            $SUDO apt update
            $SUDO apt install -y g++ make libsqlite3-dev libncurses-dev
            ;;
        fedora|rhel|centos)
            echo -e "${AZUL}▸${NC} Instalando dependencias para Fedora/RHEL..."
            $SUDO dnf install -y gcc gcc-c++ make sqlite sqlite-devel ncurses-devel
            ;;
        arch|manjaro)
            echo -e "${AZUL}▸${NC} Instalando dependencias para Arch Linux..."
            $SUDO pacman -S --noconfirm base-devel sqlite ncurses
            ;;
        *)
            echo -e "${ROJO}✗${NC} Distribución no soportada. Instale manualmente:"
            echo "   g++, make, libsqlite3-dev, libncurses-dev"
            exit 1
            ;;
    esac
    echo -e "${VERDE}✓${NC} Dependencias instaladas"
}

verificar_herramientas() {
    echo ""
    echo -e "${AMARILLO}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${CYAN}  🔍 VERIFICANDO HERRAMIENTAS${NC}"
    echo -e "${AMARILLO}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"

    FALTANTES=()

    if ! command -v g++ &> /dev/null; then
        FALTANTES+=("g++")
    fi
    if ! command -v make &> /dev/null; then
        FALTANTES+=("make")
    fi
    if ! command -v sqlite3 &> /dev/null; then
        FALTANTES+=("sqlite3")
    fi

    if [ ${#FALTANTES[@]} -eq 0 ]; then
        echo -e "${VERDE}✓${NC} Todas las herramientas están instaladas"
        echo -e "   ${AZUL}g++:${NC} $(g++ --version | head -1 | cut -d' ' -f3)"
        echo -e "   ${AZUL}sqlite:${NC} $(sqlite3 --version | cut -d' ' -f2)"
    else
        echo -e "${ROJO}✗${NC} Faltan: ${FALTANTES[*]}"
        exit 1
    fi
}

crear_directorios() {
    echo ""
    echo -e "${AMARILLO}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${CYAN}  📁 CREANDO DIRECTORIOS${NC}"
    echo -e "${AMARILLO}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"

    mkdir -p datos
    mkdir -p build
    echo -e "${VERDE}✓${NC} Directorios creados"
}

compilar_proyecto() {
    echo ""
    echo -e "${AMARILLO}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${CYAN}  ⚙️  COMPILANDO PROYECTO${NC}"
    echo -e "${AMARILLO}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"

    make build

    if [ -f "build/granja" ]; then
        echo -e "${VERDE}✓${NC} Compilación exitosa"
        ls -lh build/granja
    else
        echo -e "${ROJO}✗${NC} Error en compilación"
        exit 1
    fi
}

instalar_sistema() {
    echo ""
    echo -e "${AMARILLO}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${CYAN}  🚀 INSTALANDO EN EL SISTEMA${NC}"
    echo -e "${AMARILLO}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"

    if [ "$EUID" -ne 0 ]; then
        echo -e "${AMARILLO}⚠${NC} Para instalar globalmente necesita ejecutar como root:"
        echo "   sudo ./instalar.sh --install"
        return
    fi

    cp build/granja /usr/local/bin/granja
    chmod +x /usr/local/bin/granja

    echo -e "${VERDE}✓${NC} Instalado en /usr/local/bin/granja"
    echo -e "${VERDE}✓${NC} Ahora puede ejecutar: ${CYAN}granja ayuda${NC}"
}

confirmar() {
    local mensaje="$1"
    echo -e "${AMARILLO}⚠${NC} $mensaje [S/N]: "
    read -n 1 -r
    echo
    if [[ $REPLY =~ ^[SsYy]$ ]]; then
        return 0
    else
        return 1
    fi
}

mostrar_ayuda() {
    echo ""
    echo -e "${AMARILLO}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${CYAN}  📖 USO DEL SISTEMA${NC}"
    echo -e "${AMARILLO}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo ""
    echo -e "  ${CYAN}Comandos básicos:${NC}"
    echo "    ./build/granja ayuda              - Mostrar ayuda"
    echo "    ./build/granja ui                 - Interfaz visual"
    echo "    ./build/granja ejemplo            - Cargar datos de ejemplo"
    echo "    ./build/granja dashboard          - Ver dashboard"
    echo ""
    echo -e "  ${CYAN}Gestión de lotes:${NC}"
    echo "    ./build/granja lote nuevo <num>   - Crear lote"
    echo "    ./build/granja lote listar        - Listar lotes"
    echo ""
    echo -e "  ${CYAN}Ventas:${NC}"
    echo "    ./build/granja venta nueva <lote> <cliente> <pesos>"
    echo "    ./build/granja cliente agregar <nombre>"
    echo ""
    echo -e "  ${CYAN}Reportes:${NC}"
    echo "    ./build/granja reporte inventario"
    echo "    ./build/granja reporte financiero"
    echo ""
}

actualizar_sistema() {
    echo ""
    echo -e "${AMARILLO}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${CYAN}  🔄 ACTUALIZANDO SISTEMA${NC}"
    echo -e "${AMARILLO}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"

    echo -e "${AZUL}▸${NC} Verificando cambios en Git..."
    if [ -d ".git" ]; then
        git fetch origin main
        LOCAL=$(git rev-parse HEAD)
        REMOTE=$(git rev-parse origin/main)

        if [ "$LOCAL" != "$REMOTE" ]; then
            echo -e "${AMARILLO}⚠${NC} Hay actualizaciones disponibles"
            echo -e "   ${AZUL}Local:${NC}  $LOCAL"
            echo -e "   ${AZUL}Remote:${NC} $REMOTE"
            if confirmar "¿Descargar actualizaciones?"; then
                git pull origin main
                echo -e "${VERDE}✓${NC} Cambios descargados"
            fi
        else
            echo -e "${VERDE}✓${NC} Sistema ya está actualizado"
        fi
    else
        echo -e "${AMARILLO}⚠${NC} No es un repositorio Git, compilando directamente"
    fi

    echo ""
    echo -e "${AZUL}▸${NC} Limpiando compilación anterior..."
    make clean

    echo ""
    echo -e "${AZUL}▸${NC} Compilando nueva versión..."
    compilar_proyecto

    if [ "$SUDO" != "" ]; then
        echo ""
        if confirmar "¿Actualizar instalación global?"; then
            instalar_sistema
        fi
    fi

    echo ""
    echo -e "${VERDE}════════════════════════════════════════════════════════════${NC}"
    echo -e "${VERDE}  ✓ SISTEMA ACTUALIZADO A LA ÚLTIMA VERSIÓN${NC}"
    echo -e "${VERDE}════════════════════════════════════════════════════════════${NC}"
}

menu_interactivo() {
    while true; do
        clear
        mostrar_banner
        echo ""
        echo -e "${BLANCO}  Seleccione una opción:${NC}"
        echo ""
        echo -e "  ${CYAN}1)${NC} Instalar sistema (primera vez)"
        echo -e "  ${CYAN}2)${NC} 🔄 Actualizar sistema (recompilar)"
        echo -e "  ${CYAN}3)${NC} Instalar globalmente (requiere root)"
        echo -e "  ${CYAN}4)${NC} Cargar datos de ejemplo"
        echo -e "  ${CYAN}5)${NC} Ver ayuda rápida"
        echo -e "  ${CYAN}6)${NC} Ejecutar interfaz visual"
        echo -e "  ${CYAN}7)${NC} Salir"
        echo ""
        read -p "  Opción: " opcion

        case $opcion in
            1)
                verificar_root
                detectar_distro
                instalar_dependencias
                verificar_herramientas
                crear_directorios
                compilar_proyecto
                echo -e "${VERDE}✓${NC} ¡Instalación completada!"
                read -p "Presione Enter para continuar..."
                ;;
            2)
                actualizar_sistema
                read -p "Presione Enter para continuar..."
                ;;
            3)
                instalar_sistema
                read -p "Presione Enter para continuar..."
                ;;
            4)
                ./build/granja ejemplo
                read -p "Presione Enter para continuar..."
                ;;
            5)
                mostrar_ayuda
                read -p "Presione Enter para continuar..."
                ;;
            6)
                ./build/granja ui
                ;;
            7)
                echo ""
                echo -e "${CYAN}¡Gracias por usar Chicken Control! 🐔${NC}"
                exit 0
                ;;
        esac
    done
}

mostrar_banner
echo -e "${BLANCO}  Bienvenido al instalador de Chicken Control${NC}"
echo ""

if [ "$1" == "--install" ]; then
    verificar_root
    detectar_distro
    instalar_dependencias
    verificar_herramientas
    crear_directorios
    compilar_proyecto
    instalar_sistema
    mostrar_ayuda
elif [ "$1" == "--update" ] || [ "$1" == "-u" ]; then
    actualizar_sistema
elif [ "$1" == "--help" ] || [ "$1" == "-h" ]; then
    echo -e "${CYAN}Uso:${NC} $0 [opciones]"
    echo ""
    echo "Opciones:"
    echo "  --install    Instalar y configurar el sistema"
    echo "  --update    Actualizar sistema (recompilar)"
    echo "  --help       Mostrar esta ayuda"
    echo "  (sin args)  Modo interactivo"
    echo ""
    echo "Ejemplos:"
    echo "  $0                    # Modo interactivo"
    echo "  sudo $0 --install     # Instalar globally"
    echo "  ./instalar.sh --update # Actualizar y recompilar"
else
    menu_interactivo
fi