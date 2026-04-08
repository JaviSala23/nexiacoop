-- ============================================================
-- NEXIA - Sistema de Gestión de Socios Jubilados
-- Schema MySQL
-- ============================================================

CREATE DATABASE IF NOT EXISTS JubiladoSurgentes
    CHARACTER SET utf8mb4
    COLLATE utf8mb4_unicode_ci;

USE JubiladoSurgentes;

-- ------------------------------------------------------------
-- PROVINCIAS
-- ------------------------------------------------------------
CREATE TABLE IF NOT EXISTS provincias (
    id_provincia  INT AUTO_INCREMENT PRIMARY KEY,
    nombre        VARCHAR(100) NOT NULL UNIQUE
) ENGINE=InnoDB;

-- ------------------------------------------------------------
-- LOCALIDADES
-- ------------------------------------------------------------
CREATE TABLE IF NOT EXISTS localidades (
    id_localidad  INT AUTO_INCREMENT PRIMARY KEY,
    nombre        VARCHAR(150) NOT NULL,
    id_provincia  INT NOT NULL,
    FOREIGN KEY (id_provincia) REFERENCES provincias(id_provincia)
        ON DELETE CASCADE
) ENGINE=InnoDB;

-- ------------------------------------------------------------
-- COBRADORAS
-- ------------------------------------------------------------
CREATE TABLE IF NOT EXISTS cobradoras (
    id_cobradora      INT AUTO_INCREMENT PRIMARY KEY,
    nombre            VARCHAR(100) NOT NULL,
    telefono          VARCHAR(20),
    porcentaje_comision DECIMAL(5,2) DEFAULT 0.00,
    activa            TINYINT(1) DEFAULT 1
) ENGINE=InnoDB;

-- ------------------------------------------------------------
-- SOCIOS
-- ------------------------------------------------------------
CREATE TABLE IF NOT EXISTS socios (
    id_socio          INT AUTO_INCREMENT PRIMARY KEY,
    numero_socio      INT UNIQUE NOT NULL,
    nombre_completo   VARCHAR(150) NOT NULL,
    dni               VARCHAR(20) UNIQUE NOT NULL,
    direccion         VARCHAR(200),
    telefono          VARCHAR(20),
    id_localidad      INT,
    id_cobradora      INT,
    estado            ENUM('ACTIVO','INACTIVO','BAJA') DEFAULT 'ACTIVO',
    fecha_alta        DATE NOT NULL,
    fecha_baja        DATE,
    tiene_responsable TINYINT(1) DEFAULT 0,
    FOREIGN KEY (id_localidad) REFERENCES localidades(id_localidad)
        ON DELETE SET NULL,
    FOREIGN KEY (id_cobradora) REFERENCES cobradoras(id_cobradora)
        ON DELETE SET NULL
) ENGINE=InnoDB;

-- ------------------------------------------------------------
-- RESPONSABLES
-- ------------------------------------------------------------
CREATE TABLE IF NOT EXISTS responsables (
    id_responsable    INT AUTO_INCREMENT PRIMARY KEY,
    nombre_completo   VARCHAR(150) NOT NULL,
    dni               VARCHAR(20),
    direccion         VARCHAR(200),
    telefono          VARCHAR(20),
    id_localidad      INT,
    FOREIGN KEY (id_localidad) REFERENCES localidades(id_localidad)
        ON DELETE SET NULL
) ENGINE=InnoDB;

-- ------------------------------------------------------------
-- SOCIO <-> RESPONSABLE (relación dinámica con historial)
-- ------------------------------------------------------------
CREATE TABLE IF NOT EXISTS socio_responsable (
    id                INT AUTO_INCREMENT PRIMARY KEY,
    id_socio          INT NOT NULL,
    id_responsable    INT NOT NULL,
    tipo_vinculo      ENUM('CONYUGE','HIJO','OTRO') NOT NULL DEFAULT 'CONYUGE',
    fecha_desde       DATE NOT NULL,
    fecha_hasta       DATE,
    activo            TINYINT(1) DEFAULT 1,
    FOREIGN KEY (id_socio)        REFERENCES socios(id_socio),
    FOREIGN KEY (id_responsable)  REFERENCES responsables(id_responsable)
) ENGINE=InnoDB;

-- ------------------------------------------------------------
-- PARÁMETROS DEL SISTEMA
-- ------------------------------------------------------------
CREATE TABLE IF NOT EXISTS parametros (
    id_parametro           INT AUTO_INCREMENT PRIMARY KEY,
    cuota_simple           DECIMAL(10,2) NOT NULL DEFAULT 5000.00,
    cuota_con_responsable  DECIMAL(10,2) NOT NULL DEFAULT 8000.00,
    mes_cobranza           INT NOT NULL DEFAULT 1,
    anio_cobranza          INT NOT NULL DEFAULT 2026,
    nombre_asociacion      VARCHAR(200) DEFAULT '',
    domicilio              VARCHAR(200) DEFAULT '',
    telefono               VARCHAR(50)  DEFAULT '',
    localidad              VARCHAR(100) DEFAULT '',
    provincia              VARCHAR(100) DEFAULT '',
    email                  VARCHAR(150) DEFAULT '',
    logo_url               MEDIUMTEXT   DEFAULT ''
) ENGINE=InnoDB;

-- ------------------------------------------------------------
-- TALONES (tabla central)
-- ------------------------------------------------------------
CREATE TABLE IF NOT EXISTS talones (
    id_talon          INT AUTO_INCREMENT PRIMARY KEY,
    id_socio          INT NOT NULL,
    id_cobradora      INT NOT NULL,
    mes               INT NOT NULL,
    anio              INT NOT NULL,
    monto             DECIMAL(10,2) NOT NULL,
    tipo              ENUM('SIMPLE','CON_CARGO') NOT NULL,
    codigo_barra      VARCHAR(50) UNIQUE NOT NULL,
    estado            ENUM('GENERADO','PAGADO','ANULADO') DEFAULT 'GENERADO',
    fecha_generacion  DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    fecha_pago        DATETIME,
    FOREIGN KEY (id_socio)      REFERENCES socios(id_socio),
    FOREIGN KEY (id_cobradora)  REFERENCES cobradoras(id_cobradora)
) ENGINE=InnoDB;

-- ------------------------------------------------------------
-- PAGOS
-- ------------------------------------------------------------
CREATE TABLE IF NOT EXISTS pagos (
    id_pago           INT AUTO_INCREMENT PRIMARY KEY,
    id_talon          INT NOT NULL,
    id_cobradora      INT NOT NULL,
    fecha_pago        DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    monto             DECIMAL(10,2) NOT NULL,
    observaciones     TEXT,
    FOREIGN KEY (id_talon)       REFERENCES talones(id_talon),
    FOREIGN KEY (id_cobradora)   REFERENCES cobradoras(id_cobradora)
) ENGINE=InnoDB;

-- ------------------------------------------------------------
-- USUARIOS DEL SISTEMA (login + admin)
-- ------------------------------------------------------------
CREATE TABLE IF NOT EXISTS usuarios (
    id_usuario    INT AUTO_INCREMENT PRIMARY KEY,
    username      VARCHAR(50) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    nombre        VARCHAR(100),
    rol           ENUM('ADMIN','OPERADOR') DEFAULT 'OPERADOR',
    activo        TINYINT(1) DEFAULT 1,
    created_at    DATETIME DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB;

-- ------------------------------------------------------------
-- DATOS INICIALES
-- ------------------------------------------------------------
INSERT INTO parametros (cuota_simple, cuota_con_responsable, mes_cobranza, anio_cobranza)
VALUES (5000.00, 8000.00, 3, 2026);

-- Usuario admin inicial. Password: admin123 (SHA-256)
INSERT INTO usuarios (username, password_hash, nombre, rol)
VALUES ('admin',
        SHA2('admin123', 256),
        'Administrador',
        'ADMIN');
