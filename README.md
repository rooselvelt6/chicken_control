# 🐔 Chicken Control - Sistema de Gestión de Granjas de Pollos

<div align="center">

[![C++](https://img.shields.io/badge/C++-17-blue.svg?style=flat-square)](https://isocpp.org/)
[![SQLite](https://img.shields.io/badge/SQLite-3-brightgreen.svg?style=flat-square)](https://www.sqlite.org/)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg?style=flat-square)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Linux-orange.svg?style=flat-square)](https://fedora.org/)

**Sistema completo de gestión y control para granjas de pollos de engorde**

</div>

---

## 📋 Descripción

**Chicken Control** es un sistema de gestión integral para granjas avícolas, desarrollado en **C++** con base de datos **SQLite**. Permite controlar todo el ciclo de producción de pollos de engorde (45 días), desde la recepción de animales hasta la venta final, con seguimiento de peso semanal, mortalidad, alimentación y métricas financieras.

---

## ✨ Características Principales

### 🏠 Gestión de Granjas
- Múltiples tipos de granja: **Terrestre**, **Aérea**, **Marítima**
- Múltiples galpones/corrales por granja
- Especificaciones detalladas de cada corral:
  - Dimensiones (largo, ancho, altura)
  - Tipo de techo y protección
  - Sistema de ventilación y orientación
  - Comederos y bebederos
  - Iluminación y calefacción

### 🐣 Gestión de Animales
- Registro de lotes con control de fechas
- Seguimiento individual por fase (Bebé → Intermedio → Grande)
- Control de mortalidad por causa:
  - Aplastamiento, Ahogamiento, Gases tóxicos, Infarto, Retención de líquidos
- Sacrificio con registro de peso

### ⚖️ Pesaje Semanal
- Registro de peso durante el ciclo de 45 días
- Peso objetivo por semana
- Comparación automática: ✓ En peso / ↑ Por encima / ↓ Por debajo
- Reporte visual de evolución de peso

### 🍞 Alimentación
- Inventario de alimentos por fase
- Control de consumo por lote
- Cálculo de conversión alimenticia

### 💰 Ventas y Finanzas
- Registro de clientes y ventas
- Control de métodos de pago
- Seguimiento de deudas pendientes
- Métricas por lote:
  - Costo por kg producido
  - Rentabilidad porcentual
  - Ganancia neta

### 🏥 Veterinaria
- Catálogo de vacunas (aplicación por día)
- Inventario de medicamentos
- Control de proveedores

---

## 🚀 Instalación

### Requisitos
- **Linux** (probado en Fedora)
- **GCC** con soporte C++17
- **SQLite3**
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

### Ver ayuda
```bash
./build/granja ayuda
```

### Ejemplo rápido
```bash
# Cargar datos de ejemplo
./build/granja ejemplo

# Listar lotes
./build/granja lote listar

# Agregar animales
./build/granja animal agregar 1 100 --precio 1.0

# Registrar pesaje semanal
./build/granja pesaje registrar 1 1 "0.5,0.6,0.55,0.58"

# Ver métricas del lote
./build/granja metricas lote 1
```

### Comandos disponibles

| Comando | Descripción |
|---------|-------------|
| `ayuda` | Mostrar todos los comandos |
| `ejemplo` | Cargar datos de demostración |
| `granja crear <nombre> <tipo_id>` | Crear nueva granja |
| `granja listar` | Listar todas las granjas |
| `corral crear <granja> <num> <nombre> <capacidad>` | Crear corral |
| `lote nuevo <num>` | Crear nuevo lote |
| `animal agregar <lote> <cant>` | Agregar animales |
| `animal muerte <lote> <causa> <cant>` | Registrar muerte |
| `animal sacrificar <lote> <cant> <peso>` | Registrar sacrificio |
| `alimento agregar <nombre>` | Agregar alimento |
| `alimento consumir <lote> <nombre> <cant>` | Registrar consumo |
| `venta nueva <lote> <cliente> <pesos>` | Nueva venta |
| `proveedor agregar <nombre>` | Agregar proveedor |
| `vacuna agregar <nombre> <dias>` | Agregar vacuna |
| `medicamento agregar <nombre>` | Agregar medicamento |
| `pesaje registrar <lote> <semana> <pesos>` | Registrar peso semanal |
| `metricas lote <id>` | Ver métricas del lote |
| `metricas dashboard` | Ver dashboard general |
| `reporte lote <id>` | Reporte completo del lote |
| `reporte financiero` | Reporte financiero |

---

## 📊 Estructura del Proyecto

```
chicken_control/
├── include/           # Archivos de cabecera (.h)
│   ├── modelos.h      # Estructuras de datos y enums
│   ├── base_datos.h   # Gestión de SQLite
│   ├── lotes.h        # Gestión de lotes
│   ├── animales.h     # Gestión de animales
│   ├── alimentacion.h # Gestión de alimentos
│   ├── ventas.h       # Gestión de ventas
│   ├── pesaje.h       # Control de peso semanal
│   ├── metricas.h     # Cálculos y reportes
│   └── ...
├── src/               # Implementación (.cpp)
│   ├── main.cpp       # Punto de entrada
│   ├── modelos.cpp    # Funciones de modelos
│   ├── base_datos.cpp # Base de datos SQLite
│   └── ...
├── build/             # Ejecutable compilado
├── datos/             # Base de datos SQLite
├── Makefile          # Configuración de compilación
└── README.md         # Este archivo
```

---

## 🛠️ Tecnologías

- **Lenguaje:** C++17
- **Base de datos:** SQLite3
- **Build system:** Make
- **Librerías:** sqlite3, pthreads

---

## 📝 Licencia

Este proyecto está bajo la Licencia MIT - ver el archivo [LICENSE](LICENSE) para detalles.

---

## 👤 Autor

**Tu Nombre**
- GitHub: [@tu-usuario](https://github.com/tu-usuario)

---

<div align="center">

⭐️ Si este proyecto te es útil, ¡danos una estrella!

</div>