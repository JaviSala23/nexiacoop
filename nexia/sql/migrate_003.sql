-- ============================================================
-- NEXIA - Migración 003: Módulo de Inventario y Préstamos
-- ============================================================

USE JubiladoSurgentes;

-- ------------------------------------------------------------
-- ELEMENTOS DE INVENTARIO
-- Catálogo de elementos que la asociación puede prestar
-- ------------------------------------------------------------
CREATE TABLE IF NOT EXISTS elementos_inventario (
    id_elemento       INT AUTO_INCREMENT PRIMARY KEY,
    nombre            VARCHAR(150) NOT NULL,
    descripcion       TEXT,
    stock_total       INT NOT NULL DEFAULT 0,
    stock_disponible  INT NOT NULL DEFAULT 0,
    activo            TINYINT(1) DEFAULT 1,
    CONSTRAINT ck_stock_positivo CHECK (stock_total >= 0),
    CONSTRAINT ck_disponible_valido CHECK (stock_disponible >= 0 AND stock_disponible <= stock_total)
) ENGINE=InnoDB;

-- ------------------------------------------------------------
-- PRÉSTAMOS
-- Registro de cada préstamo y devolución
-- ------------------------------------------------------------
CREATE TABLE IF NOT EXISTS prestamos (
    id_prestamo       INT AUTO_INCREMENT PRIMARY KEY,
    id_elemento       INT NOT NULL,
    id_socio          INT NOT NULL,
    fecha_prestamo    DATE NOT NULL DEFAULT (CURDATE()),
    fecha_devolucion  DATE NULL,
    observaciones     TEXT,
    estado            ENUM('PRESTADO','DEVUELTO') NOT NULL DEFAULT 'PRESTADO',
    FOREIGN KEY (id_elemento) REFERENCES elementos_inventario(id_elemento)
        ON DELETE RESTRICT,
    FOREIGN KEY (id_socio)    REFERENCES socios(id_socio)
        ON DELETE RESTRICT
) ENGINE=InnoDB;

-- Índices para búsquedas frecuentes
CREATE INDEX idx_prestamos_socio    ON prestamos(id_socio);
CREATE INDEX idx_prestamos_elemento ON prestamos(id_elemento);
CREATE INDEX idx_prestamos_estado   ON prestamos(estado);
