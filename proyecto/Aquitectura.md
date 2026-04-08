* código
* actualizar estado

---

### PagoRepository

* insertar pago
* listar pagos por mes

---

# 📦 CAPA 4 → MODELS

Son **estructuras C++ puras**

Ejemplo:

<pre class="overflow-visible! px-0!" data-start="3126" data-end="3284"><div class="relative w-full mt-4 mb-1"><div class=""><div class="relative"><div class="h-full min-h-0 min-w-0"><div class="h-full min-h-0 min-w-0"><div class="border border-token-border-light border-radius-3xl corner-superellipse/1.1 rounded-3xl"><div class="h-full w-full border-radius-3xl bg-token-bg-elevated-secondary corner-superellipse/1.1 overflow-clip rounded-3xl lxnfua_clipPathFallback"><div class="pointer-events-none absolute end-1.5 top-1 z-2 md:end-2 md:top-1"></div><div class="pe-11 pt-3"><div class="relative z-0 flex max-w-full"><div id="code-block-viewer" dir="ltr" class="q9tKkq_viewer cm-editor z-10 light:cm-light dark:cm-light flex h-full w-full flex-col items-stretch ͼ5 ͼj"><div class="cm-scroller"><div class="cm-content q9tKkq_readonly"><span>class Socio {</span><br/><span>    int id;</span><br/><span>    std::string nombre;</span><br/><span>    std::string dni;</span><br/><span>    std::string direccion;</span><br/><span>    int idCobradora;</span><br/><span>    bool activo;</span><br/><span>};</span></div></div></div></div></div></div></div></div></div><div class=""><div class=""></div></div></div></div></div></pre>

---

# 🔌 CAPA DATABASE

### DbManager

Encargado de:

* conexión pool MySQL
* queries preparadas
* manejo de errores

---

# ⚙️ CAPA UTILS

Muy importante.

### BarcodeGenerator

* genera string código barras
* puede generar imagen futura

### FechaUtils

* mes actual
* validaciones

### DineroUtils

* formateo
* cálculos comisión

---

# 🔄 FLUJO REAL DEL SISTEMA

## ✔ Generación mensual

Controller → TalonService → TalonRepository → DB

---

## ✔ Pago con lectora

Controller → PagoService → TalonRepository → PagoRepository → DB

---

## ✔ Padrón

Controller → ReporteService → SocioRepository → HTML template

---

# 📊 Ventajas de esta arquitectura

* Escalable
* Testeable
* Reutilizable
* Limpia
* Fácil mantenimiento
* Permite API móvil futura
* Permite lector QR
* Permite app cobradora

---

# 🚀 Próximo paso (MUY IMPORTANTE)

Ahora tenemos que diseñar:

👉 **Flujo interno del proceso más crítico:
GENERACIÓN MASIVA DE TALONES**

Ese es el corazón del sistema.

Si eso está mal → todo el sistema falla.
