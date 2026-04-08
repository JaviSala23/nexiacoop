-- ============================================================
-- NEXIA - Sistema de Gestión de Socios Jubilados
-- Nuevo schema para padrón sin responsables
-- ============================================================

CREATE DATABASE IF NOT EXISTS JubiladoSurgentes
    CHARACTER SET utf8mb4
    COLLATE utf8mb4_unicode_ci;

USE JubiladoSurgentes;

-- Provincias
CREATE TABLE IF NOT EXISTS provincias (
    id_provincia  INT AUTO_INCREMENT PRIMARY KEY,
    nombre        VARCHAR(100) NOT NULL UNIQUE
) ENGINE=InnoDB;

-- Localidades
CREATE TABLE IF NOT EXISTS localidades (
    id_localidad  INT AUTO_INCREMENT PRIMARY KEY,
    nombre        VARCHAR(150) NOT NULL,
    id_provincia  INT NOT NULL,
    FOREIGN KEY (id_provincia) REFERENCES provincias(id_provincia)
        ON DELETE CASCADE
) ENGINE=InnoDB;

-- Cobradoras
CREATE TABLE IF NOT EXISTS cobradoras (
    id_cobradora      INT AUTO_INCREMENT PRIMARY KEY,
    nombre            VARCHAR(100) NOT NULL,
    telefono          VARCHAR(20),
    porcentaje_comision DECIMAL(5,2) DEFAULT 0.00,
    activa            TINYINT(1) DEFAULT 1
) ENGINE=InnoDB;

-- Socios (todos iguales, sin responsables)
CREATE TABLE IF NOT EXISTS socios (
    id_socio          INT AUTO_INCREMENT PRIMARY KEY,
    numero_socio      INT UNIQUE NOT NULL,
    nombre_completo   VARCHAR(150) NOT NULL,
    dni               VARCHAR(20) UNIQUE NOT NULL,
    direccion         VARCHAR(200),
    telefono          VARCHAR(20),
    id_localidad      INT,
    id_cobradora      INT,
    id_socio_principal INT DEFAULT NULL,
    estado            ENUM('ACTIVO','INACTIVO','BAJA') DEFAULT 'ACTIVO',
    fecha_alta        DATE NOT NULL,
    fecha_baja        DATE,
    FOREIGN KEY (id_localidad) REFERENCES localidades(id_localidad)
        ON DELETE SET NULL,
    FOREIGN KEY (id_cobradora) REFERENCES cobradoras(id_cobradora)
        ON DELETE SET NULL,
    FOREIGN KEY (id_socio_principal) REFERENCES socios(id_socio)
        ON DELETE SET NULL
) ENGINE=InnoDB;

-- Parámetros del sistema
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

-- Talones
CREATE TABLE IF NOT EXISTS talones (
    id_talon          INT AUTO_INCREMENT PRIMARY KEY,
    id_socio          INT NOT NULL,
    id_cobradora      INT NOT NULL,
    mes               INT NOT NULL,
    anio              INT NOT NULL,
    monto             DECIMAL(10,2) NOT NULL,
    FOREIGN KEY (id_socio) REFERENCES socios(id_socio),
    FOREIGN KEY (id_cobradora) REFERENCES cobradoras(id_cobradora)
) ENGINE=InnoDB;
