SET FOREIGN_KEY_CHECKS=0;
-- ----------------------------
-- Table structure for `db_version_infinity_update`
-- ----------------------------
DROP TABLE IF EXISTS `db_version_infinity_update`;
CREATE TABLE `db_version_infinity_update` (
  `version` varchar(120) NOT NULL,
  `r00` bit(1) default NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Using YTDB lock system and revision log';

