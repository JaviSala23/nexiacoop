-- Migración 005: Premios de rifas (sorteo)

CREATE TABLE IF NOT EXISTS rifa_premios (
    id_premio        INT AUTO_INCREMENT PRIMARY KEY,
    id_rifa          INT NOT NULL,
    orden            INT NOT NULL DEFAULT 1,          -- 1er premio, 2do, etc.
    numero_ganador   INT DEFAULT NULL,                -- número de rifa ganador
    nombre_ganador   VARCHAR(200) DEFAULT NULL,       -- nombre libre del ganador
    descripcion      VARCHAR(500) DEFAULT NULL,       -- descripción del premio
    valor            DECIMAL(10,2) NOT NULL DEFAULT 0.00,
    fue_donado       TINYINT(1) NOT NULL DEFAULT 0,   -- 1 = donado, no se registra en contabilidad
    id_movimiento    INT DEFAULT NULL,                -- movimiento de egreso si se registró
    FOREIGN KEY (id_rifa)       REFERENCES rifas(id_rifa) ON DELETE CASCADE,
    FOREIGN KEY (id_movimiento) REFERENCES movimientos(id_movimiento) ON DELETE SET NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
