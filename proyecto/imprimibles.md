# 🧾 MÓDULO DE IMPRIMIBLES DEL SISTEMA

El sistema debe poder generar **3 grandes tipos de documentos PDF**:

1️⃣ Talones de cobro
2️⃣ Padrón general institucional
3️⃣ Padrón organizado por responsables

Todos deben poder:

* imprimirse por socio
* imprimirse por cobradora
* imprimirse por filtros
* imprimirse completos
* imprimirse paginados

---

# 🎟️ TALONES DE COBRO (Documento principal)

Este es el documento más importante del sistema.

## 🎯 Objetivo

* Entregar comprobante al socio
* Entregar comprobante a la cobradora
* Permitir registro posterior mediante código de barras
* Permitir control contable

---

## 🧠 Concepto de diseño

Cada hoja puede contener:

* 1 talón grande
  o
* 2 talones por hoja (recomendado)

Cada talón debe estar **duplicado**:

👉 Parte superior → SOCIO
👉 Parte inferior → COBRADORA

Separadas por:

* línea punteada
* marca de corte

---

## 📄 Contenido del talón

Debe incluir SIEMPRE:

* Nombre del socio principal
* DNI del socio
* Dirección
* Nombre del responsable (si existe)
* DNI responsable (si existe)
* Nombre de la cobradora
* Mes y año
* Monto a pagar
* Código de barras
* Número de socio

---

## 🧾 Estructura visual sugerida

<pre class="overflow-visible! px-0!" data-start="1556" data-end="2123"><div class="relative w-full mt-4 mb-1"><div class=""><div class="relative"><div class="h-full min-h-0 min-w-0"><div class="h-full min-h-0 min-w-0"><div class="border border-token-border-light border-radius-3xl corner-superellipse/1.1 rounded-3xl"><div class="h-full w-full border-radius-3xl bg-token-bg-elevated-secondary corner-superellipse/1.1 overflow-clip rounded-3xl lxnfua_clipPathFallback"><div class="pointer-events-none absolute end-1.5 top-1 z-2 md:end-2 md:top-1"></div><div class="pe-11 pt-3"><div class="relative z-0 flex max-w-full"><div id="code-block-viewer" dir="ltr" class="q9tKkq_viewer cm-editor z-10 light:cm-light dark:cm-light flex h-full w-full flex-col items-stretch ͼ5 ͼj"><div class="cm-scroller"><div class="cm-content q9tKkq_readonly"><span>-----------------------------------------</span><br/><span>CLUB JUBILADOS - TALÓN DE COBRO</span><br/><br/><span>Socio: Juan Perez</span><br/><span>DNI: 12.345.678</span><br/><span>Dirección: Calle 123</span><br/><br/><span>Responsable: Maria Perez</span><br/><span>DNI Responsable: 23.456.789</span><br/><br/><span>Cobradora: Marta Lopez</span><br/><br/><span>Mes: Mayo 2026</span><br/><span>Importe: $6000</span><br/><br/><span>[CODIGO DE BARRAS]</span><br/><br/><span>-----------------------------------------</span><br/><span>------ cortar por aquí ------------------</span><br/><span>-----------------------------------------</span><br/><br/><span>CLUB JUBILADOS - TALÓN COBRADORA</span><br/><br/><span>Socio: Juan Perez</span><br/><span>Dirección: Calle 123</span><br/><span>Importe: $6000</span><br/><span>Cobradora: Marta Lopez</span><br/><br/><span>[CODIGO DE BARRAS]</span><br/><span>-----------------------------------------</span></div></div></div></div></div></div></div></div></div><div class=""><div class=""></div></div></div></div></div></pre>

---

# 📦 TIPOS DE GENERACIÓN DE TALONES

El sistema debe permitir:

### ✔ Generar talones por:

* Todos los socios
* Solo activos
* Por cobradora
* Por rango de socios
* Por búsqueda
* Individual

### ✔ Generación paginada

Ejemplo:

* Página 1 → socios 1 al 30
* Página 2 → socios 31 al 60

Esto es MUY importante cuando:

* se imprime en tandas
* se reimprimen errores

---

# 📊 PADRÓN GENERAL (PDF)

Este padrón NO muestra responsables.

## Debe contener columnas:

* Número socio
* Nombre completo
* DNI
* Dirección
* Cobradora
* Estado

Debe poder:

* ordenarse por apellido
* ordenarse por DNI
* filtrarse por estado

Formato tipo:

<pre class="overflow-visible! px-0!" data-start="2778" data-end="2857"><div class="relative w-full mt-4 mb-1"><div class=""><div class="relative"><div class="h-full min-h-0 min-w-0"><div class="h-full min-h-0 min-w-0"><div class="border border-token-border-light border-radius-3xl corner-superellipse/1.1 rounded-3xl"><div class="h-full w-full border-radius-3xl bg-token-bg-elevated-secondary corner-superellipse/1.1 overflow-clip rounded-3xl lxnfua_clipPathFallback"><div class="pointer-events-none absolute end-1.5 top-1 z-2 md:end-2 md:top-1"></div><div class="pe-11 pt-3"><div class="relative z-0 flex max-w-full"><div id="code-block-viewer" dir="ltr" class="q9tKkq_viewer cm-editor z-10 light:cm-light dark:cm-light flex h-full w-full flex-col items-stretch ͼ5 ͼj"><div class="cm-scroller"><div class="cm-content q9tKkq_readonly"><span>PADRÓN GENERAL CLUB</span><br/><br/><span>N° | Nombre | DNI | Dirección | Cobradora | Estado</span></div></div></div></div></div></div></div></div></div><div class=""><div class=""></div></div></div></div></div></pre>

---

# 👥 PADRÓN ORGANIZADO POR RESPONSABLES

Este es más “operativo”.

## 🎯 Objetivo

* Entender grupos familiares
* Controlar cobranza
* Entender cargas económicas

---

## 🧠 Diseño sugerido

Orden:

👉 Responsable (orden alfabético)

Debajo:

👉 Socios a cargo

Visualmente:

* Responsable → letra normal
* Socios a cargo → sangría + gris

---

## Ejemplo

<pre class="overflow-visible! px-0!" data-start="3221" data-end="3323"><div class="relative w-full mt-4 mb-1"><div class=""><div class="relative"><div class="h-full min-h-0 min-w-0"><div class="h-full min-h-0 min-w-0"><div class="border border-token-border-light border-radius-3xl corner-superellipse/1.1 rounded-3xl"><div class="h-full w-full border-radius-3xl bg-token-bg-elevated-secondary corner-superellipse/1.1 overflow-clip rounded-3xl lxnfua_clipPathFallback"><div class="pointer-events-none absolute end-1.5 top-1 z-2 md:end-2 md:top-1"></div><div class="pe-11 pt-3"><div class="relative z-0 flex max-w-full"><div id="code-block-viewer" dir="ltr" class="q9tKkq_viewer cm-editor z-10 light:cm-light dark:cm-light flex h-full w-full flex-col items-stretch ͼ5 ͼj"><div class="cm-scroller"><div class="cm-content q9tKkq_readonly"><span>MARIA PEREZ</span><br/><br/><span>   - Juan Perez</span><br/><span>   - Carlos Perez</span><br/><br/><span>ROBERTO LOPEZ</span><br/><br/><span>   - Marta Gomez</span><br/><span>   - Ana Gomez</span></div></div></div></div></div></div></div></div></div><div class=""><div class=""></div></div></div></div></div></pre>

También puede agregarse:

* dirección
* cobradora

---

# 📄 PADRÓN DE COBRANZA (extra recomendable)

Por cobradora:

<pre class="overflow-visible! px-0!" data-start="3443" data-end="3516"><div class="relative w-full mt-4 mb-1"><div class=""><div class="relative"><div class="h-full min-h-0 min-w-0"><div class="h-full min-h-0 min-w-0"><div class="border border-token-border-light border-radius-3xl corner-superellipse/1.1 rounded-3xl"><div class="h-full w-full border-radius-3xl bg-token-bg-elevated-secondary corner-superellipse/1.1 overflow-clip rounded-3xl lxnfua_clipPathFallback"><div class="pointer-events-none absolute end-1.5 top-1 z-2 md:end-2 md:top-1"></div><div class="pe-11 pt-3"><div class="relative z-0 flex max-w-full"><div id="code-block-viewer" dir="ltr" class="q9tKkq_viewer cm-editor z-10 light:cm-light dark:cm-light flex h-full w-full flex-col items-stretch ͼ5 ͼj"><div class="cm-scroller"><div class="cm-content q9tKkq_readonly"><span>COBRADORA: MARTA</span><br/><br/><span>Juan Perez - Calle 123</span><br/><span>Carlos Gomez - Calle 456</span></div></div></div></div></div></div></div></div></div><div class=""><div class=""></div></div></div></div></div></pre>

---

# 🧠 RECOMENDACIONES TÉCNICAS PARA DROGON

Para generar estos PDFs:

👉 Opción profesional:

* HTML template + CSS de impresión
* renderizar con wkhtmltopdf
* o usar librería tipo:
  * libharu
  * PoDoFo
  * Cairo PDF

👉 Opción más simple:

* generar HTML imprimible
* dejar que el navegador genere PDF

---

# 🎯 RECOMENDACIÓN PROFESIONAL

En sistemas administrativos:

👉 **Los talones SIEMPRE se generan desde HTML**

Porque:

* es más fácil diseñar
* se pueden ajustar tamaños
* permite CSS de corte
* permite paginado automático
* permite reimpresión fácil
