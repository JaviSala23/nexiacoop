-- ============================================================
-- NEXIA - Reset tabla de talones
-- Limpia talones, pagos y movimientos vinculados para empezar
-- desde cero. EJECUTAR SOLO EN DESARROLLO / RESET TOTAL.
-- ============================================================

USE CooperadoraEscolar;

SET FOREIGN_KEY_CHECKS = 0;

-- Movimientos contables vinculados a pagos
DELETE FROM movimientos WHERE id_pago IS NOT NULL;

-- Pagos vinculados a talones
TRUNCATE TABLE pagos;

-- Talones
TRUNCATE TABLE talones;

SET FOREIGN_KEY_CHECKS = 1;

SELECT 'Tabla talones, pagos y movimientos de cobro limpiados.' AS resultado;
