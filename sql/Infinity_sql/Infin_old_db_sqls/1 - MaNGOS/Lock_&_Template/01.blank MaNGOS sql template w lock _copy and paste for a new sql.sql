-- FallenAngelX
ALTER TABLE db_version_infinity_update CHANGE COLUMN r00 r01 bit;

-- sql content between the lines --------------  start

-- blah blah blah  i am data that goes here 

-- sql content between the lines -------------- end

REPLACE INTO `db_version_infinity_update` (`version`) VALUES ('r01');

UPDATE db_version SET `cache_id`= '01';
UPDATE db_version SET `version`= '_Infinity_YTDB_590_V1_r01';