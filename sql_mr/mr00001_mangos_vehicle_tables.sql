-- Commit 3be940faa44326abc801
ALTER TABLE creature_template
  ADD COLUMN spell5 mediumint(8) unsigned NOT NULL default '0' AFTER spell4,
  ADD COLUMN spell6 mediumint(8) unsigned NOT NULL default '0' AFTER spell5,
  ADD COLUMN spell7 mediumint(8) unsigned NOT NULL default '0' AFTER spell6,
  ADD COLUMN spell8 mediumint(8) unsigned NOT NULL default '0' AFTER spell7,
  ADD COLUMN PowerType tinyint(3) unsigned NOT NULL default '0' AFTER MaxHealth;