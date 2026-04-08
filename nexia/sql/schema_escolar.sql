-- ============================================================
-- NEXIA ESCOLAR - Sistema de Gestión Cooperadora Escolar
-- Schema MySQL
-- ============================================================

CREATE DATABASE IF NOT EXISTS CooperadoraEscolar
    CHARACTER SET utf8mb4
    COLLATE utf8mb4_unicode_ci;

USE CooperadoraEscolar;

-- ------------------------------------------------------------
-- FAMILIAS (grupo familiar - reemplaza socios)
-- ------------------------------------------------------------
CREATE TABLE IF NOT EXISTS familias (
    id_familia        INT AUTO_INCREMENT PRIMARY KEY,
    numero_familia    INT UNIQUE NOT NULL,
    direccion         VARCHAR(200),
    telefono          VARCHAR(20),
    estado            ENUM('ACTIVA','INACTIVA','BAJA') DEFAULT 'ACTIVA',
    fecha_alta        DATE NOT NULL,
    fecha_baja        DATE,
    observaciones     TEXT
) ENGINE=InnoDB;

-- ------------------------------------------------------------
-- TUTORES (padre / madre / tutor legal - mínimo 1, máximo 2)
-- ------------------------------------------------------------
CREATE TABLE IF NOT EXISTS tutores (
    id_tutor          INT AUTO_INCREMENT PRIMARY KEY,
    id_familia        INT NOT NULL,
    dni               VARCHAR(20) NOT NULL,
    nombre_completo   VARCHAR(150) NOT NULL,
    telefono          VARCHAR(20),
    email             VARCHAR(150),
    es_principal      TINYINT(1) DEFAULT 1,
    FOREIGN KEY (id_familia) REFERENCES familias(id_familia)
        ON DELETE CASCADE
) ENGINE=InnoDB;

-- ------------------------------------------------------------
-- ALUMNOS (hijos vinculados al grupo familiar)
-- ------------------------------------------------------------
CREATE TABLE IF NOT EXISTS alumnos (
    id_alumno         INT AUTO_INCREMENT PRIMARY KEY,
    id_familia        INT NOT NULL,
    dni               VARCHAR(20),
    nombre_completo   VARCHAR(150) NOT NULL,
    fecha_nacimiento  DATE,
    anio_escolar      VARCHAR(20),
    estado            ENUM('ACTIVO','INACTIVO','EGRESADO') DEFAULT 'ACTIVO',
    FOREIGN KEY (id_familia) REFERENCES familias(id_familia)
        ON DELETE CASCADE
) ENGINE=InnoDB;

-- ------------------------------------------------------------
-- CONCEPTOS (tipos de cobro: cuota mensual, matrícula, viaje...)
-- ------------------------------------------------------------
CREATE TABLE IF NOT EXISTS conceptos (
    id_concepto       INT AUTO_INCREMENT PRIMARY KEY,
    nombre            VARCHAR(100) NOT NULL,
    descripcion       TEXT,
    tipo              ENUM('MENSUAL','EXTRA') NOT NULL DEFAULT 'EXTRA',
    activo            TINYINT(1) DEFAULT 1
) ENGINE=InnoDB;

-- ------------------------------------------------------------
-- CUOTAS CONFIG (monto según cantidad de alumnos)
-- La cooperadora configura: 1 hijo → $X, 2 hijos → $Y, etc.
-- ------------------------------------------------------------
CREATE TABLE IF NOT EXISTS cuotas_config (
    id_config         INT AUTO_INCREMENT PRIMARY KEY,
    cantidad_hijos    INT NOT NULL,
    monto             DECIMAL(10,2) NOT NULL,
    activo            TINYINT(1) DEFAULT 1,
    UNIQUE KEY uk_cantidad (cantidad_hijos)
) ENGINE=InnoDB;

-- ------------------------------------------------------------
-- TALONES (uno por familia+concepto; para MENSUAL incluye mes/año)
-- ------------------------------------------------------------
CREATE TABLE IF NOT EXISTS talones (
    id_talon          INT AUTO_INCREMENT PRIMARY KEY,
    id_familia        INT NOT NULL,
    id_concepto       INT NOT NULL,
    mes               INT,
    anio              INT,
    monto             DECIMAL(10,2) NOT NULL,
    codigo_barra      VARCHAR(60) UNIQUE NOT NULL,
    estado            ENUM('GENERADO','PAGADO','ANULADO') DEFAULT 'GENERADO',
    fecha_generacion  DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    fecha_pago        DATETIME,
    observaciones     TEXT,
    FOREIGN KEY (id_familia)  REFERENCES familias(id_familia),
    FOREIGN KEY (id_concepto) REFERENCES conceptos(id_concepto)
) ENGINE=InnoDB;

-- ------------------------------------------------------------
-- PAGOS (con medio de pago)
-- ------------------------------------------------------------
CREATE TABLE IF NOT EXISTS pagos (
    id_pago           INT AUTO_INCREMENT PRIMARY KEY,
    id_talon          INT NOT NULL,
    fecha_pago        DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    monto             DECIMAL(10,2) NOT NULL,
    medio_pago        ENUM('EFECTIVO','TRANSFERENCIA','MERCADOPAGO','OTRO') NOT NULL DEFAULT 'EFECTIVO',
    observaciones     TEXT,
    FOREIGN KEY (id_talon) REFERENCES talones(id_talon)
) ENGINE=InnoDB;

-- ------------------------------------------------------------
-- PLAN DE CUENTAS (contabilidad simple)
-- ------------------------------------------------------------
CREATE TABLE IF NOT EXISTS plan_cuentas (
    id_cuenta         INT AUTO_INCREMENT PRIMARY KEY,
    codigo            VARCHAR(20) NOT NULL UNIQUE,
    nombre            VARCHAR(100) NOT NULL,
    tipo              ENUM('INGRESO','EGRESO') NOT NULL,
    activo            TINYINT(1) DEFAULT 1
) ENGINE=InnoDB;

-- ------------------------------------------------------------
-- MOVIMIENTOS CONTABLES
-- ------------------------------------------------------------
CREATE TABLE IF NOT EXISTS movimientos (
    id_movimiento     INT AUTO_INCREMENT PRIMARY KEY,
    id_cuenta         INT NOT NULL,
    fecha             DATE NOT NULL,
    descripcion       TEXT,
    monto             DECIMAL(10,2) NOT NULL,
    tipo              ENUM('INGRESO','EGRESO') NOT NULL,
    medio_pago        ENUM('EFECTIVO','TRANSFERENCIA','MERCADOPAGO','OTRO') NOT NULL DEFAULT 'EFECTIVO',
    id_pago           INT DEFAULT NULL,
    FOREIGN KEY (id_cuenta) REFERENCES plan_cuentas(id_cuenta),
    FOREIGN KEY (id_pago)   REFERENCES pagos(id_pago) ON DELETE SET NULL
) ENGINE=InnoDB;

-- ------------------------------------------------------------
-- PARÁMETROS DEL SISTEMA
-- ------------------------------------------------------------
CREATE TABLE IF NOT EXISTS parametros (
    id_parametro       INT AUTO_INCREMENT PRIMARY KEY,
    mes_cobranza       INT NOT NULL DEFAULT 1,
    anio_cobranza      INT NOT NULL DEFAULT 2026,
    nombre_institucion VARCHAR(200) DEFAULT '',
    domicilio          VARCHAR(200) DEFAULT '',
    telefono           VARCHAR(50)  DEFAULT '',
    localidad          VARCHAR(100) DEFAULT '',
    provincia          VARCHAR(100) DEFAULT '',
    email              VARCHAR(150) DEFAULT '',
    logo_url           MEDIUMTEXT   DEFAULT ''
) ENGINE=InnoDB;

-- ------------------------------------------------------------
-- USUARIOS DEL SISTEMA
-- ------------------------------------------------------------
CREATE TABLE IF NOT EXISTS usuarios (
    id_usuario        INT AUTO_INCREMENT PRIMARY KEY,
    username          VARCHAR(50) UNIQUE NOT NULL,
    password_hash     VARCHAR(255) NOT NULL,
    nombre            VARCHAR(100),
    rol               ENUM('ADMIN','OPERADOR') DEFAULT 'OPERADOR',
    activo            TINYINT(1) DEFAULT 1,
    created_at        DATETIME DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB;

-- ============================================================
-- DATOS INICIALES
-- ============================================================

INSERT INTO parametros (mes_cobranza, anio_cobranza, nombre_institucion)
VALUES (3, 2026, 'Cooperadora Escolar');

-- Conceptos base
INSERT INTO conceptos (nombre, tipo, descripcion, activo) VALUES
('Cuota mensual',        'MENSUAL', 'Cuota mensual de la cooperadora', 1),
('Matrícula anual',      'EXTRA',   'Pago de matrícula al comienzo del ciclo lectivo', 1),
('Merienda',             'EXTRA',   'Colaboración para la merienda escolar', 1),
('Colaboración especial','EXTRA',   'Colaboración especial o evento puntual', 1),
('Viaje escolar',        'EXTRA',   'Pago de viaje o excursión', 1);

-- Configuración de cuotas (1, 2 y 3+ hijos / ejemplos que la cooperadora puede ajustar)
INSERT INTO cuotas_config (cantidad_hijos, monto) VALUES
(1, 60000.00),
(2, 100000.00),
(3, 90000.00);

-- Plan de cuentas base
INSERT INTO plan_cuentas (codigo, nombre, tipo) VALUES
('ING-001', 'Cuotas mensuales',         'INGRESO'),
('ING-002', 'Matrículas',               'INGRESO'),
('ING-003', 'Merienda',                 'INGRESO'),
('ING-004', 'Viajes escolares',         'INGRESO'),
('ING-005', 'Colaboraciones varias',    'INGRESO'),
('EGR-001', 'Materiales escolares',     'EGRESO'),
('EGR-002', 'Reparaciones y mantenimiento', 'EGRESO'),
('EGR-003', 'Actos escolares',          'EGRESO'),
('EGR-004', 'Merienda y alimentos',     'EGRESO'),
('EGR-005', 'Gastos administrativos',   'EGRESO'),
('EGR-006', 'Otros egresos',            'EGRESO');

-- Cursos (configurables desde Admin → Cursos)
CREATE TABLE IF NOT EXISTS cursos (
    id_curso  INT AUTO_INCREMENT PRIMARY KEY,
    nombre    VARCHAR(60) NOT NULL,
    orden     INT NOT NULL DEFAULT 0,
    activo    TINYINT(1) NOT NULL DEFAULT 1
) ENGINE=InnoDB;

INSERT INTO cursos (nombre, orden) VALUES
('Sala de 3 (Jardín)', 1),
('Sala de 4 (Jardín)', 2),
('Sala de 5 (Jardín)', 3),
('1° grado', 10),
('2° grado', 20),
('3° grado', 30),
('4° grado', 40),
('5° grado', 50),
('6° grado', 60),
('7° grado', 70);

-- Usuario admin inicial. Password: admin123 (SHA-256)
INSERT INTO usuarios (username, password_hash, nombre, rol)
VALUES ('admin', SHA2('admin123', 256), 'Administrador', 'ADMIN');
