# 🐔 Chicken Control - Sistema de Gestión de Granjas de Pollos

<div align="center">

[![C++](https://img.shields.io/badge/C++-17-blue.svg?style=flat-square)](https://isocpp.org/)
[![SQLite](https://img.shields.io/badge/SQLite-3-brightgreen.svg?style=flat-square)](https://www.sqlite.org/)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg?style=flat-square)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Linux-orange.svg?style=flat-square)](https://fedora.org/)
[![ncurses](https://img.shields.io/badge/ncurses-TUI-blue.svg?style=flat-square)](https://invisible-island.net/ncurses/)

**Sistema completo de gestión y control para granjas avícolas**

</div>

---

## 📋 Descripción

**Chicken Control** es un sistema de gestión integral para granjas avícolas, desarrollado en **C++** con base de datos **SQLite**. Ofrece dos modos de uso:

1. **CLI (Línea de comandos)** - Para usuarios avanzados
2. **TUI (Interfaz visual)** - Menú interactivo con ncurses

---

## ✨ Características Principales

### 🖥️ Interfaz TUI (Nueva)
- Menú interactivo con navegación por teclado
- Colores y diseño mejorado
- Paginación en tablas grandes
- Validación de datos en formularios

### 📊 Funcionalidades del Sistema

| Módulo | Descripción |
|--------|-------------|
| **Lotes y Animales** | Crear lotes, agregar animales, registrar muertes y sacrificios |
| **Alimentación** | Inventario de alimentos, consumo por lote, control de stock |
| **Ventas y Clientes** | Registro de ventas, control de deudas, seguimiento de pagos |
| **Granjas y Corrales** | Gestión de múltiples granjas y corrales con especificaciones técnicas |
| **Veterinaria** | Catálogo de vacunas, inventario de medicamentos |
| **Reportes y Métricas** | Dashboard general, métricas por lote, reportes financieros |
| **Alertas** | Notificaciones automáticas de inventario bajo, mortalidad alta, deudas pendientes |

### ⚙️ Características Técnicas
- **Índices SQLite** para queries optimizadas (10-100x más rápidas)
- **Exportar/Importar JSON** para backup y restauración
- **Optimización de base de datos** con VACUUM
- **Validación de datos** en todos los formularios
- **Paginación** en tablas grandes
- **Sistema de Facturación** - Generación de facturas con IVA
- **Contenedores/Refrigeradores** - Almacenamiento de pollos procesados
- **Registro de Beneficio** - Control de matanza (manual/automático)

---

## 🚀 Instalación

### Requisitos
- **Linux** (probado en Fedora/Debian)
- **GCC** con soporte C++17
- **SQLite3** y desarrollo (`libsqlite3-dev`)
- **ncurses** (`libncurses-dev`)
- **Make**

### Compilación

```bash
# Clonar el repositorio
git clone https://github.com/tu-usuario/chicken_control.git
cd chicken_control

# Compilar
make build

# O usar el script de instalación
chmod +x instalar.sh
./instalar.sh
```

---

## 📖 Uso

### Modo CLI (Línea de comandos)

#### Ver ayuda
```bash
./build/granja ayuda
```

#### Comandos principales

| Comando | Descripción |
|---------|-------------|
| **UI y Configuración** | |
| `./granja ui` | Iniciar interfaz gráfica TUI |
| `./granja exportar` | Exportar base de datos a JSON |
| `./granja importar <archivo>` | Importar datos desde JSON |
| `./granja optimizar` | Optimizar base de datos (VACUUM) |
| `./granja alertas` | Ver alertas del sistema |
| `./granja factura generar <venta> [cedula] [dir]` | Generar factura |
| `./granja factura ver <id>` | Ver factura |
| `./granja factura listar` | Listar facturas |
| `./granja factura anular <id>` | Anular factura |
| `./granja contenedor crear <nom> <ub> <cap> [temp]` | Crear contenedor |
| `./granja contenedor listar` | Listar contenedores |
| `./granja beneficio registrar <lote> <cant> <peso> <tipo> <op> <ced>` | Registrar beneficio |
| **Lotes** | |
| `./granja lote nuevo <num>` | Crear nuevo lote |
| `./granja lote listar` | Listar todos los lotes |
| `./granja lote cerrar <id>` | Cerrar un lote |
| **Animales** | |
| `./granja animal agregar <lote> <cant> [--precio P] [--peso KG]` | Agregar animales |
| `./granja animal muerte <lote> <causa> <cant>` | Registrar muerte |
| `./granja animal sacrificar <lote> <cant> <peso>` | Registrar sacrificio |
| **Alimentación** | |
| `./granja alimento agregar <nombre> [--fase fase] [--precio P] [--inventario N]` | Agregar alimento |
| `./granja alimento consumir <lote> <nombre> <cant>` | Registrar consumo |
| `./granja alimento inventario` | Ver inventario |
| **Ventas** | |
| `./granja cliente agregar <nombre> [--telefono T] [--referencia R]` | Agregar cliente |
| `./granja venta nueva <lote> <cliente> <pesos> [--precio P]` | Nueva venta |
| `./granja venta pagar <id> <monto>` | Registrar pago |
| `./granja venta deudas` | Ver deudas pendientes |
| **Reportes** | |
| `./granja metricas lote <id>` | Métricas de un lote |
| `./granja metricas dashboard` | Dashboard general |
| `./granja reporte lote <id>` | Reporte completo del lote |
| `./granja reporte financiero` | Reporte financiero |

#### Ejemplo rápido de uso
```bash
# Iniciar interfaz gráfica
./build/granja ui

# O usar CLI
./build/granja lote nuevo 1
./build/granja animal agregar 1 100 --precio 1.0
./build/granja alimento agregar "Engorde" --fase grande --precio 40 --inventario 50
./build/granja animal agregar 1 100 --precio 1.0
./granja pesaje registrar 1 1 "0.5,0.6,0.55,0.58"
./granja metricas lote 1
```

### Modo TUI (Interfaz Gráfica)

Para iniciar la interfaz visual:
```bash
./build/granja ui
```

**Controles:**
- **Flechas** (↑↓): Navegar por el menú
- **Enter**: Seleccionar opción
- **Esc**: Volver/Salir

---

## 🏗️ Arquitectura del Sistema

```
chicken_control/
├── include/                    # Archivos de cabecera (.h)
│   ├── modelos.h              # Estructuras de datos y enums
│   ├── base_datos.h          # Gestión de SQLite
│   ├── ui.h                   # Interfaz TUI (ncurses)
│   ├── alertas.h             # Sistema de alertas
│   ├── lotes.h               # Gestión de lotes
│   ├── animales.h            # Gestión de animales
│   ├── alimentacion.h        # Gestión de alimentos
│   ├── ventas.h              # Gestión de ventas
│   ├── metricas.h            # Cálculos y reportes
│   └── ...
├── src/                       # Implementación (.cpp)
│   ├── main.cpp              # Punto de entrada
│   ├── ui.cpp                # Interfaz TUI
│   ├── alertas.cpp           # Sistema de alertas
│   ├── base_datos.cpp        # Base de datos SQLite
│   └── ...
├── build/                     # Ejecutable compilado
├── datos/                     # Base de datos SQLite
├── Makefile                   # Configuración de compilación
└── README.md                  # Este archivo
```

### Diagrama de Flujo

```
┌─────────────┐     ┌─────────────┐
│   CLI       │────▶│   TUI       │
│  (main.cpp) │     │  (ncurses)  │
└──────┬──────┘     └──────┬──────┘
       │                  │
       ▼                  ▼
┌──────────────────────────────────┐
│         Base de Datos            │
│       (SQLite + índices)         │
└──────────────────────────────────┘
       │    │    │    │    │
       ▼    ▼    ▼    ▼    ▼
   Lotes  Animales  Ventas  Aliment  ...
```

### Base de Datos

**Tablas principales:**
- `lotes` - Registro de lotes de producción
- `animales` - Animales por lote
- `muertes` - Mortalidad por causa
- `sacrificios` - Sacrificio de pollos
- `alimentos` - Inventario de alimentos
- `consumo_alimento` - Consumo por lote
- `ventas` - Registro de ventas
- `clientes` - Clientes
- `granjas` - Granjas registradas
- `corrales` - Corrales por granja
- `vacunas` - Catálogo de vacunas
- `medicamentos` - Inventario de medicamentos
- `pesaje_semanal` - Pesaje por semana

**Índices optimizados:** 20+ índices para mejorar el rendimiento de consultas.

---

## 🛠️ Tecnologías

| Tecnología | Uso |
|------------|-----|
| **C++17** | Lenguaje de programación |
| **SQLite3** | Base de datos embebida |
| **ncurses** | Interfaz TUI |
| **Make** | Build system |
| **Git** | Control de versiones |

---

## 📝 Licencia

Este proyecto está bajo la Licencia MIT - ver el archivo [LICENSE](LICENSE) para detalles.

---

## 👤 Autor

**Chicken Control**
- GitHub: [tu-usuario](https://github.com/tu-usuario)

---

<div align="center">

⭐️ Si este proyecto te es útil, ¡danos una estrella!

</div>