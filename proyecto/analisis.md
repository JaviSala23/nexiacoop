# 🧠 SISTEMA DE GESTIÓN DE SOCIOS JUBILADOS

## (Análisis funcional completo)

---

# 🎯 OBJETIVO DEL SISTEMA

Desarrollar un sistema informático para el club que permita:

* Administrar el padrón de socios
* Gestionar cobradoras y zonas
* Generar talones mensuales de cobro con código de barras
* Registrar pagos mediante lectora
* Controlar morosidad
* Calcular deuda total del club
* Liquidar comisiones de cobradoras
* Obtener reportes institucionales y contables

El sistema será **operativo, administrativo y financiero**.

---

# 👥 CONCEPTO DE SOCIO

El club posee **socios jubilados** que pueden encontrarse en dos situaciones:

### ✔ Socio individual (sin persona a cargo)

* Paga su cuota mensual normal
* Recibe talón simple

### ✔ Socio con persona a cargo

* Existe un responsable que paga por él
* Se genera un único talón con importe mayor

Esta relación **puede cambiar en cualquier momento**, por lo tanto no debe ser rígida.

---

# 📊 DATOS DEL SOCIO (Definitivo)

Cada socio debe tener:

* Número de socio (ID interno)
* Nombre completo
* **Número de Documento (DNI obligatorio)**
* Dirección
* Cobradora asignada
* Estado
* Fecha de alta
* Fecha de baja
* Indicador: tiene responsable (SI / NO)

---

# 👤 PERSONA RESPONSABLE (A CARGO)

Entidad independiente.

Puede ser:

* Otro socio
* Familiar
* Tercero externo

Datos:

* Nombre completo
* Documento (opcional)
* Dirección
* Teléfono (futuro)

Relación:

* Un responsable puede tener varios socios a cargo

---

# 🔄 ESTADOS DEL SOCIO

El sistema debe contemplar:

* ACTIVO
* INACTIVO
* BAJA DEFINITIVA
* SUSPENDIDO (opcional futuro)

Los estados afectan:

* Generación de talones
* Aparición en padrones
* Cálculo de deuda

---

# 📄 TIPOS DE PADRÓN

Este punto es **clave en el sistema real**.

---

## 📋 1️⃣ PADRÓN GENERAL INSTITUCIONAL

Debe permitir imprimir:

* Número de socio
* Nombre completo
* DNI
* Dirección
* Cobradora
* Estado

❗ IMPORTANTE:

👉 **NO debe mostrar si tiene persona a cargo**

Este padrón sirve para:

* Asambleas
* Control administrativo
* Presentaciones legales
* Estadísticas institucionales

Debe permitir filtros:

* Solo activos
* Solo inactivos
* Todos
* Orden por apellido
* Orden por DNI
* Orden por cobradora

---

## 📋 2️⃣ PADRÓN DE COBRANZA

Debe generarse:

* Agrupado por cobradora
* Con listado de domicilios

Sirve para:

* Trabajo en calle
* Organización de recorridos
* Control de entrega de talones

Debe mostrar:

* Nombre socio
* Dirección
* Responsable (si tiene)
* Monto a cobrar

---

## 📋 3️⃣ PADRÓN DE MOROSIDAD

Debe mostrar:

* Socios con deuda
* Meses adeudados
* Total deuda
* Cobradora

---

# 💰 SISTEMA DE TALONES DE COBRO

Cada mes el sistema debe:

1. Generar talones automáticamente
2. Asociarlos a:
   * socio
   * mes
   * año
   * cobradora
   * monto
3. Crear un **código de barras único**

---

## 🎟️ TIPOS DE TALÓN

### ✔ Talón simple

* Para socios sin responsable
* Monto cuota normal

### ✔ Talón con cargo

* Para socios con responsable
* Monto configurable (ej doble cuota)

Debe decir:

> “Socio a cargo de: XXXXX”

---

# 🔢 CÓDIGO DE BARRAS

Debe identificar:

* Número socio
* Año
* Mes
* Tipo cuota

Ejemplo conceptual:

<pre class="overflow-visible! px-0!" data-start="3443" data-end="3467"><div class="relative w-full mt-4 mb-1"><div class=""><div class="relative"><div class="h-full min-h-0 min-w-0"><div class="h-full min-h-0 min-w-0"><div class="border border-token-border-light border-radius-3xl corner-superellipse/1.1 rounded-3xl"><div class="h-full w-full border-radius-3xl bg-token-bg-elevated-secondary corner-superellipse/1.1 overflow-clip rounded-3xl lxnfua_clipPathFallback"><div class="pointer-events-none absolute end-1.5 top-1 z-2 md:end-2 md:top-1"></div><div class="pe-11 pt-3"><div class="relative z-0 flex max-w-full"><div id="code-block-viewer" dir="ltr" class="q9tKkq_viewer cm-editor z-10 light:cm-light dark:cm-light flex h-full w-full flex-col items-stretch ͼ5 ͼj"><div class="cm-scroller"><div class="cm-content q9tKkq_readonly"><span>SSSSSS AAAA MM T</span></div></div></div></div></div></div></div></div></div><div class=""><div class=""></div></div></div></div></div></pre>

Esto permite:

* Registrar pagos rápidamente
* Evitar errores humanos
* Automatizar contabilidad

---

# 📥 REGISTRO DE PAGOS

Proceso:

1. Cobradora vuelve al club
2. Se escanean talones
3. Sistema marca cuota como PAGADA
4. Registra:
   * fecha
   * cobradora
   * importe

---

# 📉 CONTROL DE MOROSIDAD

El sistema debe calcular automáticamente:

* Meses adeudados
* Deuda total por socio
* Deuda total general
* Porcentaje de cobranza mensual

---

# 📊 CONTABILIDAD DE COBRANZA

Debe permitir saber:

* Cuánto dinero ingresó
* Cuánto falta cobrar
* Cuánto debe rendir cada cobradora

---

# 💼 LIQUIDACIÓN DE COBRADORAS

Si trabajan con comisión:

El sistema debe calcular:

* Total talones entregados
* Total cobrados
* Total pendiente
* Comisión
* Dinero a rendir

---

# ⚙️ PARÁMETROS CONFIGURABLES

El cliente necesita poder cambiar:

* Valor cuota simple
* Valor cuota con responsable
* Porcentaje comisión cobradora
* Mes de cobranza activo

Sin tocar código.

---

# 📊 REPORTES NECESARIOS

* Padrón general
* Padrón por cobradora
* Listado de morosos
* Informe mensual
* Liquidación cobradoras
* Recaudación histórica

---

# 🧠 VISIÓN PROFESIONAL DEL SISTEMA

Este sistema es:

👉 **Un sistema de gestión mutual real**

Tiene:

* Operación territorial
* Gestión financiera
* Gestión de deuda
* Generación documental
* Control institucional

No es solo “imprimir tickets”.
