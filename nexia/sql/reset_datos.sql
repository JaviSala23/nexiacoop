-- ============================================================
-- NEXIA - Reset de datos (NO borra usuarios)
-- Ejecutar con: mysql -u usuario -p JubiladoSurgentes < reset_datos.sql
-- ============================================================

USE JubiladoSurgentes;

SET FOREIGN_KEY_CHECKS = 0;

TRUNCATE TABLE pagos;
TRUNCATE TABLE talones;
TRUNCATE TABLE socio_responsable;
TRUNCATE TABLE responsables;
TRUNCATE TABLE socios;
TRUNCATE TABLE cobradoras;
TRUNCATE TABLE localidades;
TRUNCATE TABLE provincias;

SET FOREIGN_KEY_CHECKS = 1;
