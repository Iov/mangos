-- VIP system

DROP TABLE IF EXISTS `vips`;
CREATE TABLE `vips` (
  `id` int(8) NOT NULL,
  `joindate` timestamp NOT NULL default CURRENT_TIMESTAMP,
  `untildate` timestamp NOT NULL default '0000-00-00 00:00:00',
  `value` float unsigned NOT NULL,
  `total_value` float unsigned NOT NULL,
  `comment` text NOT NULL,
  `changed` timestamp NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
