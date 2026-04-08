# 🧠 MODELO DE BASE DE DATOS

## Sistema de Socios Jubilados (MySQL)

---

# 🎯 Principios del diseño

Este sistema necesita:

✅ Historial (no borrar información)
✅ Relaciones dinámicas (socios pueden cambiar de situación)
✅ Control financiero mensual
✅ Control territorial (cobradoras)
✅ Generación documental (talones)

Por eso el diseño es **transaccional y temporal**.

---

# 📊 TABLAS PRINCIPALES

---

# 👥 TABLA: socios

Representa a cada socio del club.

<pre class="overflow-visible! px-0!" data-start="778" data-end="795"><div class="relative w-full mt-4 mb-1"><div class=""><div class="relative"><div class="h-full min-h-0 min-w-0"><div class="h-full min-h-0 min-w-0"><div class="border border-token-border-light border-radius-3xl corner-superellipse/1.1 rounded-3xl"><div class="h-full w-full border-radius-3xl bg-token-bg-elevated-secondary corner-superellipse/1.1 overflow-clip rounded-3xl lxnfua_clipPathFallback"><div class="pointer-events-none absolute inset-x-4 top-12 bottom-4"><div class="pointer-events-none sticky z-40 shrink-0 z-1!"><div class="sticky bg-token-border-light"></div></div></div><div class=""><div class="relative z-0 flex max-w-full"><div id="code-block-viewer" dir="ltr" class="q9tKkq_viewer cm-editor z-10 light:cm-light dark:cm-light flex h-full w-full flex-col items-stretch ͼ5 ͼj"><div class="cm-scroller"><div class="cm-content q9tKkq_readonly"><span>socios</span></div></div></div></div></div></div></div></div></div><div class=""><div class=""></div></div></div></div></div></pre>

Campos:

* id\_socio (PK)
* numero\_socio (UNIQUE)
* nombre\_completo
* dni (UNIQUE)
* direccion
* id\_cobradora (FK)
* estado (ACTIVO / INACTIVO / BAJA)
* fecha\_alta
* fecha\_baja
* tiene\_responsable (BOOLEAN)

---

# 👤 TABLA: responsables

Personas que pagan por socios.

<pre class="overflow-visible! px-0!" data-start="1067" data-end="1090"><div class="relative w-full mt-4 mb-1"><div class=""><div class="relative"><div class="h-full min-h-0 min-w-0"><div class="h-full min-h-0 min-w-0"><div class="border border-token-border-light border-radius-3xl corner-superellipse/1.1 rounded-3xl"><div class="h-full w-full border-radius-3xl bg-token-bg-elevated-secondary corner-superellipse/1.1 overflow-clip rounded-3xl lxnfua_clipPathFallback"><div class="pointer-events-none absolute inset-x-4 top-12 bottom-4"><div class="pointer-events-none sticky z-40 shrink-0 z-1!"><div class="sticky bg-token-border-light"></div></div></div><div class=""><div class="relative z-0 flex max-w-full"><div id="code-block-viewer" dir="ltr" class="q9tKkq_viewer cm-editor z-10 light:cm-light dark:cm-light flex h-full w-full flex-col items-stretch ͼ5 ͼj"><div class="cm-scroller"><div class="cm-content q9tKkq_readonly"><span>responsables</span></div></div></div></div></div></div></div></div></div><div class=""><div class=""></div></div></div></div></div></pre>

Campos:

* id\_responsable (PK)
* nombre\_completo
* dni (nullable)
* direccion
* telefono

---

# 🔗 TABLA: socio\_responsable

Relación dinámica (MUY IMPORTANTE)

Permite:

* cambiar responsable en el tiempo
* historial
* múltiples situaciones

<pre class="overflow-visible! px-0!" data-start="1336" data-end="1364"><div class="relative w-full mt-4 mb-1"><div class=""><div class="relative"><div class="h-full min-h-0 min-w-0"><div class="h-full min-h-0 min-w-0"><div class="border border-token-border-light border-radius-3xl corner-superellipse/1.1 rounded-3xl"><div class="h-full w-full border-radius-3xl bg-token-bg-elevated-secondary corner-superellipse/1.1 overflow-clip rounded-3xl lxnfua_clipPathFallback"><div class="pointer-events-none absolute inset-x-4 top-12 bottom-4"><div class="pointer-events-none sticky z-40 shrink-0 z-1!"><div class="sticky bg-token-border-light"></div></div></div><div class=""><div class="relative z-0 flex max-w-full"><div id="code-block-viewer" dir="ltr" class="q9tKkq_viewer cm-editor z-10 light:cm-light dark:cm-light flex h-full w-full flex-col items-stretch ͼ5 ͼj"><div class="cm-scroller"><div class="cm-content q9tKkq_readonly"><span>socio_responsable</span></div></div></div></div></div></div></div></div></div><div class=""><div class=""></div></div></div></div></div></pre>

Campos:

* id (PK)
* id\_socio (FK)
* id\_responsable (FK)
* fecha\_desde
* fecha\_hasta (nullable)
* activo (boolean)

---

# 👩‍💼 TABLA: cobradoras

Personas que cobran en la calle.

<pre class="overflow-visible! px-0!" data-start="1548" data-end="1569"><div class="relative w-full mt-4 mb-1"><div class=""><div class="relative"><div class="h-full min-h-0 min-w-0"><div class="h-full min-h-0 min-w-0"><div class="border border-token-border-light border-radius-3xl corner-superellipse/1.1 rounded-3xl"><div class="h-full w-full border-radius-3xl bg-token-bg-elevated-secondary corner-superellipse/1.1 overflow-clip rounded-3xl lxnfua_clipPathFallback"><div class="pointer-events-none absolute inset-x-4 top-12 bottom-4"><div class="pointer-events-none sticky z-40 shrink-0 z-1!"><div class="sticky bg-token-border-light"></div></div></div><div class=""><div class="relative z-0 flex max-w-full"><div id="code-block-viewer" dir="ltr" class="q9tKkq_viewer cm-editor z-10 light:cm-light dark:cm-light flex h-full w-full flex-col items-stretch ͼ5 ͼj"><div class="cm-scroller"><div class="cm-content q9tKkq_readonly"><span>cobradoras</span></div></div></div></div></div></div></div></div></div><div class=""><div class=""></div></div></div></div></div></pre>

Campos:

* id\_cobradora (PK)
* nombre
* telefono
* porcentaje\_comision
* activa (boolean)

---

# 💰 TABLA: parametros

Configuración general del sistema.

<pre class="overflow-visible! px-0!" data-start="1727" data-end="1748"><div class="relative w-full mt-4 mb-1"><div class=""><div class="relative"><div class="h-full min-h-0 min-w-0"><div class="h-full min-h-0 min-w-0"><div class="border border-token-border-light border-radius-3xl corner-superellipse/1.1 rounded-3xl"><div class="h-full w-full border-radius-3xl bg-token-bg-elevated-secondary corner-superellipse/1.1 overflow-clip rounded-3xl lxnfua_clipPathFallback"><div class="pointer-events-none absolute inset-x-4 top-12 bottom-4"><div class="pointer-events-none sticky z-40 shrink-0 z-1!"><div class="sticky bg-token-border-light"></div></div></div><div class=""><div class="relative z-0 flex max-w-full"><div id="code-block-viewer" dir="ltr" class="q9tKkq_viewer cm-editor z-10 light:cm-light dark:cm-light flex h-full w-full flex-col items-stretch ͼ5 ͼj"><div class="cm-scroller"><div class="cm-content q9tKkq_readonly"><span>parametros</span></div></div></div></div></div></div></div></div></div><div class=""><div class=""></div></div></div></div></div></pre>

Campos:

* id\_parametro
* cuota\_simple
* cuota\_con\_responsable
* mes\_cobranza
* anio\_cobranza

---

# 🧾 TABLA: talones

Representa cada cuota mensual generada.

👉 ESTA ES LA TABLA MÁS IMPORTANTE

<pre class="overflow-visible! px-0!" data-start="1948" data-end="1966"><div class="relative w-full mt-4 mb-1"><div class=""><div class="relative"><div class="h-full min-h-0 min-w-0"><div class="h-full min-h-0 min-w-0"><div class="border border-token-border-light border-radius-3xl corner-superellipse/1.1 rounded-3xl"><div class="h-full w-full border-radius-3xl bg-token-bg-elevated-secondary corner-superellipse/1.1 overflow-clip rounded-3xl lxnfua_clipPathFallback"><div class="pointer-events-none absolute inset-x-4 top-12 bottom-4"><div class="pointer-events-none sticky z-40 shrink-0 z-1!"><div class="sticky bg-token-border-light"></div></div></div><div class=""><div class="relative z-0 flex max-w-full"><div id="code-block-viewer" dir="ltr" class="q9tKkq_viewer cm-editor z-10 light:cm-light dark:cm-light flex h-full w-full flex-col items-stretch ͼ5 ͼj"><div class="cm-scroller"><div class="cm-content q9tKkq_readonly"><span>talones</span></div></div></div></div></div></div></div></div></div><div class=""><div class=""></div></div></div></div></div></pre>

Campos:

* id\_talon (PK)
* id\_socio (FK)
* id\_cobradora (FK)
* mes
* anio
* monto
* tipo (SIMPLE / CON\_CARGO)
* codigo\_barra (UNIQUE)
* estado (GENERADO / PAGADO / ANULADO)
* fecha\_generacion
* fecha\_pago (nullable)

---

# 💳 TABLA: pagos

Historial financiero real.

<pre class="overflow-visible! px-0!" data-start="2237" data-end="2253"><div class="relative w-full mt-4 mb-1"><div class=""><div class="relative"><div class="h-full min-h-0 min-w-0"><div class="h-full min-h-0 min-w-0"><div class="border border-token-border-light border-radius-3xl corner-superellipse/1.1 rounded-3xl"><div class="h-full w-full border-radius-3xl bg-token-bg-elevated-secondary corner-superellipse/1.1 overflow-clip rounded-3xl lxnfua_clipPathFallback"><div class="pointer-events-none absolute inset-x-4 top-12 bottom-4"><div class="pointer-events-none sticky z-40 shrink-0 z-1!"><div class="sticky bg-token-border-light"></div></div></div><div class=""><div class="relative z-0 flex max-w-full"><div id="code-block-viewer" dir="ltr" class="q9tKkq_viewer cm-editor z-10 light:cm-light dark:cm-light flex h-full w-full flex-col items-stretch ͼ5 ͼj"><div class="cm-scroller"><div class="cm-content q9tKkq_readonly"><span>pagos</span></div></div></div></div></div></div></div></div></div><div class=""><div class=""></div></div></div></div></div></pre>

Campos:

* id\_pago
* id\_talon (FK)
* id\_cobradora
* fecha\_pago
* monto
* metodo (EFECTIVO)

---

# 📉 TABLA: deuda\_historica (opcional pero profesional)

Permite congelar deuda por mes.

<pre class="overflow-visible! px-0!" data-start="2442" data-end="2468"><div class="relative w-full mt-4 mb-1"><div class=""><div class="relative"><div class="h-full min-h-0 min-w-0"><div class="h-full min-h-0 min-w-0"><div class="border border-token-border-light border-radius-3xl corner-superellipse/1.1 rounded-3xl"><div class="h-full w-full border-radius-3xl bg-token-bg-elevated-secondary corner-superellipse/1.1 overflow-clip rounded-3xl lxnfua_clipPathFallback"><div class="pointer-events-none absolute inset-x-4 top-12 bottom-4"><div class="pointer-events-none sticky z-40 shrink-0 z-1!"><div class="sticky bg-token-border-light"></div></div></div><div class=""><div class="relative z-0 flex max-w-full"><div id="code-block-viewer" dir="ltr" class="q9tKkq_viewer cm-editor z-10 light:cm-light dark:cm-light flex h-full w-full flex-col items-stretch ͼ5 ͼj"><div class="cm-scroller"><div class="cm-content q9tKkq_readonly"><span>deuda_historica</span></div></div></div></div></div></div></div></div></div><div class=""><div class=""></div></div></div></div></div></pre>

Campos:

* id
* id\_socio
* mes
* anio
* monto
* pagado (boolean)

---

# 📊 RELACIONES IMPORTANTES

* Un socio → tiene una cobradora
* Un socio → puede tener responsable
* Un socio → tiene muchos talones
* Un talón → puede tener un pago
* Una cobradora → cobra muchos talones

---

# 🔢 LÓGICA DEL CÓDIGO DE BARRAS

Debe codificar:

<pre class="overflow-visible! px-0!" data-start="2803" data-end="2843"><div class="relative w-full mt-4 mb-1"><div class=""><div class="relative"><div class="h-full min-h-0 min-w-0"><div class="h-full min-h-0 min-w-0"><div class="border border-token-border-light border-radius-3xl corner-superellipse/1.1 rounded-3xl"><div class="h-full w-full border-radius-3xl bg-token-bg-elevated-secondary corner-superellipse/1.1 overflow-clip rounded-3xl lxnfua_clipPathFallback"><div class="pointer-events-none absolute end-1.5 top-1 z-2 md:end-2 md:top-1"></div><div class="pe-11 pt-3"><div class="relative z-0 flex max-w-full"><div id="code-block-viewer" dir="ltr" class="q9tKkq_viewer cm-editor z-10 light:cm-light dark:cm-light flex h-full w-full flex-col items-stretch ͼ5 ͼj"><div class="cm-scroller"><div class="cm-content q9tKkq_readonly"><span>numero_socio + anio + mes + tipo</span></div></div></div></div></div></div></div></div></div><div class=""><div class=""></div></div></div></div></div></pre>

Ejemplo:

<pre class="overflow-visible! px-0!" data-start="2855" data-end="2876"><div class="relative w-full mt-4 mb-1"><div class=""><div class="relative"><div class="h-full min-h-0 min-w-0"><div class="h-full min-h-0 min-w-0"><div class="border border-token-border-light border-radius-3xl corner-superellipse/1.1 rounded-3xl"><div class="h-full w-full border-radius-3xl bg-token-bg-elevated-secondary corner-superellipse/1.1 overflow-clip rounded-3xl lxnfua_clipPathFallback"><div class="pointer-events-none absolute end-1.5 top-1 z-2 md:end-2 md:top-1"></div><div class="pe-11 pt-3"><div class="relative z-0 flex max-w-full"><div id="code-block-viewer" dir="ltr" class="q9tKkq_viewer cm-editor z-10 light:cm-light dark:cm-light flex h-full w-full flex-col items-stretch ͼ5 ͼj"><div class="cm-scroller"><div class="cm-content q9tKkq_readonly"><span>0001232026051</span></div></div></div></div></div></div></div></div></div><div class=""><div class=""></div></div></div></div></div></pre>

---

# 🔄 PROCESO MENSUAL DEL SISTEMA

1. Se configura mes
2. Sistema genera talones
3. Se imprimen talones
4. Cobradoras cobran
5. Se escanean pagos
6. Sistema calcula:
   * recaudación
   * morosidad
   * liquidación cobradoras

---

# 📊 REPORTES QUE SALEN DE ESTA BASE

* Padrón general
* Padrón por cobradora
* Listado de morosos
* Total recaudado
* Liquidación cobradoras
* Historial socio

---

# 🧠 Nivel profesional

Este modelo permite:

✅ Auditoría
✅ Historial
✅ Escalabilidad
✅ Evitar corrupción de datos
✅ Cambios sin romper sistema
✅ Automatización total

---

# 🚀 SIGUIENTE PASO CRÍTICO

Ahora tenemos que diseñar:

👉 **Arquitectura del sistema en Drogon**

* Controllers
* Services
* Repositorios
* Flujo generación talones
* Flujo pago lector código barras

Eso es lo que te va a permitir empezar a programar.
