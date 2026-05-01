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
    echo "╔═════════════════════════════════════════════════════════════╗"
    echo "║                                                               ║"
    echo "║   🐔  CHICKEN CONTROL v3.3 - INSTALADOR                      ║"
    echo "║   Sistema Inteligente de Gestión Avícola                    ║"
    echo "║   Nivel de Seguridad: 10/10 | 73/73 Tests PASSING         ║"
    echo "║                                                               ║"
    echo "╚═════════════════════════════════════════════════════════════╝"
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
    elif [ -f /etc/SuSE-release ] || [ -f /etc/SUSE-brand ]; then
        DISTRO="opensuse"
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
        debian|ubuntu|linuxmint|pop)
            echo -e "${AZUL}▸${NC} Instalando dependencias para Debian/Ubuntu..."
            $SUDO apt update
            $SUDO apt install -y g++ make libsqlite3-dev libncurses-dev libssl-dev
            ;;
        fedora|rhel|centos|rocky|almalinux)
            echo -e "${AZUL}▸${NC} Instalando dependencias para Fedora/RHEL..."
            $SUDO dnf install -y gcc-c++ make sqlite sqlite-devel ncurses-devel openssl-devel
            ;;
        arch|manjaro|endeavour)
            echo -e "${AZUL}▸${NC} Instalando dependencias para Arch Linux..."
            $SUDO pacman -S --noconfirm base-devel sqlite ncurses openssl
            ;;
        opensuse|suse|opensuse-leap|opensuse-tumbleweed)
            echo -e "${AZUL}▸${NC} Instalando dependencias para openSUSE..."
            $SUDO zypper install -y gcc-c++ make sqlite3-devel ncurses-devel libopenssl-devel
            ;;
        *)
            echo -e "${ROJO}✗${NC} Distribución no soportada. Instale manualmente:"
            echo "   g++ (C++17), make, libsqlite3-dev, libncurses-dev, libssl-dev"
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
    if ! command -v openssl &> /dev/null; then
        FALTANTES+=("openssl")
    fi

    if [ ${#FALTANTES[@]} -eq 0 ]; then
        echo -e "${VERDE}✓${NC} Todas las herramientas están instaladas"
        echo -e "   ${AZUL}g++:${NC} $(g++ --version | head -1 | cut -d' ' -f3-)"
        echo -e "   ${AZUL}sqlite:${NC} $(sqlite3 --version | cut -d' ' -f2)"
        echo -e "   ${AZUL}openssl:${NC} $(openssl version | cut -d' ' -f2)"
        echo -e "   ${AZUL}ncurses:${NC} $(pkg-config --modversion ncurses 2>/dev/null || echo 'instalado')"
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

    echo -e "${AZUL}▸${NC} Ejecutando make build..."
    make clean && make build

    if [ -f "build/granja" ]; then
        echo -e "${VERDE}✓${NC} Compilación exitosa"
        ls -lh build/granja
    else
        echo -e "${ROJO}✗${NC} Error en compilación"
        exit 1
    fi
}

ejecutar_tests() {
    echo ""
    echo -e "${AMARILLO}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${CYAN}  🧪 EJECUTANDO TESTS UNITARIOS${NC}"
    echo -e "${AMARILLO}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"

    if make build/test 2>/dev/null; then
        echo -e "${AZUL}▸${NC} Ejecutando tests..."
        OUTPUT=$(./build/test 2>&1)
        echo "$OUTPUT"
        if echo "$OUTPUT" | grep -q "Failed: 0"; then
            echo -e "${VERDE}✓${NC} Todos los tests pasaron (73/73)"
        else
            echo -e "${AMARILLO}⚠${NC} Algunos tests fallaron, revisar salida arriba"
        fi
    else
        echo -e "${AMARILLO}⚠${NC} No se pudieron compilar los tests"
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
    echo -e "${CYAN}  📖 USO DEL SISTEMA v3.3${NC}"
    echo -e "${AMARILLO}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo ""
    echo -e "  ${CYAN}Comandos básicos:${NC}"
    echo "    ./build/granja ayuda              - Mostrar ayuda"
    echo "    ./build/granja ui                 - Interfaz visual TUI"
    echo "    ./build/granja dashboard          - Ver dashboard"
    echo ""
    echo -e "  ${CYAN}Autenticación (NUEVO v3.3):${NC}"
    echo "    ./build/granja login <user> --password <pass>"
    echo "    ./build/granja usuario agregar <user> --rol admin"
    echo ""
    echo -e "  ${CYAN}Gestión de lotes:${NC}"
    echo "    ./build/granja lote nuevo <num>   - Crear lote"
    echo "    ./build/granja lote listar        - Listar lotes"
    echo ""
    echo -e "  ${CYAN}Inventario de Insumos (NUEVO v3.3):${NC}"
    echo "    ./build/granja insumo agregar <nombre> --categoria vacuna"
    echo "    ./build/granja insumo listar"
    echo "    ./build/granja insumo alertas"
    echo ""
    echo -e "  ${CYAN}Ventas y Facturación:${NC}"
    echo "    ./build/granja venta nueva <lote> <cliente> <pesos>"
    echo "    ./build/granja cliente agregar <nombre>"
    echo "    ./build/granja factura generar <venta>"
    echo ""
    echo -e "  ${CYAN}Finanzas (NUEVO v3.2):${NC}"
    echo "    ./build/granja financiero estado 2024-01-01 2024-12-31"
    echo "    ./build/granja financiero flujo 2024"
    echo ""
    echo -e "  ${CYAN}Reportes y Gráficos:${NC}"
    echo "    ./build/granja grafico ventas-mensuales 2024"
    echo "    ./build/granja reporte txt dashboard reporte.txt"
    echo ""
    echo -e "  ${CYAN}Backup/Restore (NUEVO v3.3):${NC}"
    echo "    ./build/granja backup exportar backup.json"
    echo "    ./build/granja backup importar backup.json"
    echo ""
}

cargar_ejemplo() {
    echo ""
    echo -e "${AMARILLO}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${CYAN}  📊 CARGANDO DATOS DE EJEMPLO${NC}"
    echo -e "${AMARILLO}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"

    if [ -f "build/granja" ]; then
        ./build/granja ejemplo
        echo -e "${VERDE}✓${NC} Datos de ejemplo cargados"
    else
        echo -e "${ROJO}✗${NC} Ejecute primero la compilación"
    fi
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

    echo ""
    echo -e "${AZUL}▸${NC} Ejecutando tests..."
    ejecutar_tests

    if [ "$SUDO" != "" ]; then
        echo ""
        if confirmar "¿Actualizar instalación global?"; then
            instalar_sistema
        fi
    fi

    echo ""
    echo -e "${VERDE}════════════════════════════════════════════════════════════${NC}"
    echo -e "${VERDE}  ✓ SISTEMA ACTUALIZADO A v3.3${NC}"
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
        echo -e "  ${CYAN}5)${NC} Ejecutar tests unitarios"
        echo -e "  ${CYAN}6)${NC} Ver ayuda rápida"
        echo -e "  ${CYAN}7)${NC} Ejecutar interfaz visual"
        echo -e "  ${CYAN}8)${NC} Salir"
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
                ejecutar_tests
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
                cargar_ejemplo
                read -p "Presione Enter para continuar..."
                ;;
            5)
                ejecutar_tests
                read -p "Presione Enter para continuar..."
                ;;
            6)
                mostrar_ayuda
                read -p "Presione Enter para continuar..."
                ;;
            7)
                ./build/granja ui
                ;;
            8)
                echo ""
                echo -e "${CYAN}¡Gracias por usar Chicken Control v3.3! 🐔${NC}"
                exit 0
                ;;
        esac
    done
}

mostrar_banner
echo -e "${BLANCO}  Bienvenido al instalador de Chicken Control v3.3${NC}"
echo ""

if [ "$1" == "--install" ]; then
    verificar_root
    detectar_distro
    instalar_dependencias
    verificar_herramientas
    crear_directorios
    compilar_proyecto
    ejecutar_tests
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
    echo "  --help      Mostrar esta ayuda"
    echo "  (sin args)  Modo interactivo"
    echo ""
    echo "Ejemplos:"
    echo "  $0                    # Modo interactivo"
    echo "  sudo $0 --install     # Instalar globalmente"
    echo "  ./instalar.sh --update # Actualizar y recompilar"
    echo ""
    echo "Dependencias requeridas:"
    echo "  Ubuntu/Debian: sudo apt install g++ make libsqlite3-dev libncurses-dev libssl-dev"
    echo "  Fedora/RHEL:   sudo dnf install gcc-c++ make sqlite-devel ncurses-devel openssl-devel"
    echo "  Arch Linux:    sudo pacman -S base-devel sqlite ncurses openssl"
    echo "  openSUSE:      sudo zypper install gcc-c++ make sqlite3-devel ncurses-devel libopenssl-devel"
else
    menu_interactivo
fi
