-- Migración 001: Agregar columnas faltantes a socios y responsables
-- Ejecutar una sola vez sobre la DB existente.

USE JubiladoSurgentes;

-- Agregar telefono e id_localidad a socios (si no existen)
ALTER TABLE socios
    ADD COLUMN IF NOT EXISTS telefono    VARCHAR(20) AFTER direccion,
    ADD COLUMN IF NOT EXISTS id_localidad INT         AFTER telefono;

-- Agregar FK de localidad en socios (ignorar si ya existe)
SET @exist := (
    SELECT COUNT(*) FROM information_schema.TABLE_CONSTRAINTS
    WHERE CONSTRAINT_SCHEMA = 'JubiladoSurgentes'
      AND TABLE_NAME = 'socios'
      AND CONSTRAINT_NAME = 'fk_socios_localidad'
);
SET @sql := IF(@exist = 0,
    'ALTER TABLE socios ADD CONSTRAINT fk_socios_localidad FOREIGN KEY (id_localidad) REFERENCES localidades(id_localidad) ON DELETE SET NULL',
    'SELECT "FK fk_socios_localidad ya existe, se omite"'
);
PREPARE stmt FROM @sql; EXECUTE stmt; DEALLOCATE PREPARE stmt;

-- Agregar id_localidad a responsables (si no existe)
ALTER TABLE responsables
    ADD COLUMN IF NOT EXISTS id_localidad INT AFTER direccion;

-- Agregar FK de localidad en responsables (ignorar si ya existe)
SET @exist := (
    SELECT COUNT(*) FROM information_schema.TABLE_CONSTRAINTS
    WHERE CONSTRAINT_SCHEMA = 'JubiladoSurgentes'
      AND TABLE_NAME = 'responsables'
      AND CONSTRAINT_NAME = 'fk_responsables_localidad'
);
SET @sql := IF(@exist = 0,
    'ALTER TABLE responsables ADD CONSTRAINT fk_responsables_localidad FOREIGN KEY (id_localidad) REFERENCES localidades(id_localidad) ON DELETE SET NULL',
    'SELECT "FK fk_responsables_localidad ya existe, se omite"'
);
PREPARE stmt FROM @sql; EXECUTE stmt; DEALLOCATE PREPARE stmt;

SELECT 'Migración 001 aplicada correctamente.' AS resultado;
