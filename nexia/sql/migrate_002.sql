-- Migración 002: Agregar columnas id_socio_principal y tiene_responsable a socios
-- Ejecutar en el servidor: mysql -u usuario -p JubiladoSurgentes < migrate_002.sql

USE JubiladoSurgentes;

-- Agregar columna tiene_responsable si no existe
ALTER TABLE socios
    ADD COLUMN IF NOT EXISTS tiene_responsable TINYINT(1) NOT NULL DEFAULT 0;

-- Agregar columna id_socio_principal si no existe
ALTER TABLE socios
    ADD COLUMN IF NOT EXISTS id_socio_principal INT DEFAULT NULL;

-- Agregar foreign key si no existe (ignorar error si ya existe)
ALTER TABLE socios
    ADD CONSTRAINT fk_socio_principal
    FOREIGN KEY (id_socio_principal) REFERENCES socios(id_socio)
    ON DELETE SET NULL;
