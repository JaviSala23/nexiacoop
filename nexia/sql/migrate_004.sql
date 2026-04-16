-- ============================================================
-- Migración 004: Módulo de Rifas
-- ============================================================

-- Tabla principal de rifas
CREATE TABLE IF NOT EXISTS rifas (
    id_rifa           INT AUTO_INCREMENT PRIMARY KEY,
    nombre            VARCHAR(200) NOT NULL,
    descripcion       TEXT,
    fecha_sorteo      DATE DEFAULT NULL,
    cantidad_numeros  INT NOT NULL DEFAULT 100,
    cant_cuotas       INT NOT NULL DEFAULT 1,
    monto_total       DECIMAL(10,2) NOT NULL DEFAULT 0.00,
    monto_cuota       DECIMAL(10,2) NOT NULL DEFAULT 0.00,
    id_cuenta         INT DEFAULT NULL,
    estado            ENUM('ACTIVA','CERRADA','SORTEADA') DEFAULT 'ACTIVA',
    fecha_creacion    DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (id_cuenta) REFERENCES plan_cuentas(id_cuenta) ON DELETE SET NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Números de rifa asignados (a familias o compradores externos)
CREATE TABLE IF NOT EXISTS rifa_numeros (
    id_numero         INT AUTO_INCREMENT PRIMARY KEY,
    id_rifa           INT NOT NULL,
    numero            INT NOT NULL,
    id_familia        INT DEFAULT NULL,
    nombre_externo    VARCHAR(200) DEFAULT NULL,
    telefono_externo  VARCHAR(50)  DEFAULT NULL,
    saldo_pendiente   DECIMAL(10,2) NOT NULL DEFAULT 0.00,
    estado            ENUM('ASIGNADO','PAGADO_PARCIAL','PAGADO','ANULADO') DEFAULT 'ASIGNADO',
    fecha_asignacion  DATETIME DEFAULT CURRENT_TIMESTAMP,
    UNIQUE KEY uq_rifa_numero (id_rifa, numero),
    FOREIGN KEY (id_rifa)    REFERENCES rifas(id_rifa) ON DELETE CASCADE,
    FOREIGN KEY (id_familia) REFERENCES familias(id_familia) ON DELETE SET NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Cuotas de cada número de rifa
CREATE TABLE IF NOT EXISTS rifa_cuotas (
    id_cuota          INT AUTO_INCREMENT PRIMARY KEY,
    id_numero         INT NOT NULL,
    nro_cuota         INT NOT NULL,
    monto             DECIMAL(10,2) NOT NULL,
    estado            ENUM('PENDIENTE','PAGADA','ANULADA') DEFAULT 'PENDIENTE',
    fecha_pago        DATETIME DEFAULT NULL,
    medio_pago        ENUM('EFECTIVO','TRANSFERENCIA','MERCADOPAGO','OTRO') DEFAULT NULL,
    id_movimiento     INT DEFAULT NULL,
    FOREIGN KEY (id_numero)     REFERENCES rifa_numeros(id_numero) ON DELETE CASCADE,
    FOREIGN KEY (id_movimiento) REFERENCES movimientos(id_movimiento) ON DELETE SET NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
