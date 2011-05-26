-- NPC on transport

DROP TABLE IF EXISTS `creature_transport`;
CREATE TABLE `creature_transport` (
    `npc_entry` mediumint(8) unsigned NOT NULL default 0,
    `transport_entry` mediumint(8) unsigned NOT NULL default 0,
    `TransOffsetX` float NOT NULL DEFAULT '0',
    `TransOffsetY` float NOT NULL DEFAULT '0',
    `TransOffsetZ` float NOT NULL DEFAULT '0',
    `TransOffsetO` float NOT NULL DEFAULT '0',
    `emote` mediumint(8) unsigned NOT NULL default 0,
    PRIMARY KEY (`npc_entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='NPC on Transports';
