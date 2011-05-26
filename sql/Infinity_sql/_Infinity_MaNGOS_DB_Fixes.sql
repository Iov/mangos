-- aldaron the reckless loc fix Blood elf starting area fix
DELETE FROM `creature` WHERE `id`=16294;
INSERT INTO `creature` (`guid`,`id`,`map`,`spawnMask`,`phaseMask`,`modelid`,`equipment_id`,`position_x`,`position_y`,`position_z`,`orientation`,`spawntimesecs`,`spawndist`,`currentwaypoint`,`curhealth`,`curmana`,`DeathState`,`MovementType`) VALUES
(67521, 16294, 530, 1, 1, 0, 0, 8747.7, -6116.85, 68.5645, 5.28401, 300, 0, 0, 148, 825, 0, 0);

-- Quest Fix Noth Special Brew 
UPDATE `quest_template` SET `SpecialFlags` = 0 WHERE `entry` = 12717;

DELETE FROM `creature_questrelation` WHERE `quest` = 12716; 
DELETE FROM `gameobject_questrelation` WHERE `quest` = 12716; 
UPDATE `item_template` SET `StartQuest`=0 WHERE `StartQuest` = 12716; 
INSERT INTO `creature_questrelation` (`id`, `quest`) VALUES (28919, 12716); 
UPDATE `creature_template` SET `npcflag`=`npcflag`|2 WHERE `entry` = 28919; 
DELETE FROM `creature_involvedrelation` WHERE `quest` = 12716; 
DELETE FROM `gameobject_involvedrelation` WHERE `quest` = 12716; 
INSERT INTO `creature_involvedrelation` (`id`, `quest`) VALUES (28919, 12716); 
UPDATE `creature_template` SET `npcflag`=`npcflag`|2 WHERE `entry`=28919; 
UPDATE `quest_template` SET `ExclusiveGroup` = 12716 WHERE `entry` = 12716; 

-- faction fix for durotar starting area
UPDATE `creature_template` SET `faction_A` = 16, `faction_H` = 16 WHERE `entry` = 3101;
UPDATE `creature_template` SET `faction_A` = 16, `faction_H` = 16 WHERE `entry` = 3124;
UPDATE `creature_template` SET `faction_A` = 16, `faction_H` = 16 WHERE `entry` = 3102;

--faction fix for sunstrider island starting area
UPDATE `creature_template` SET `faction_A` = 16, `faction_H` = 16 WHERE `entry` = 15271;
UPDATE `creature_template` SET `faction_A` = 16, `faction_H` = 16 WHERE `entry` = 15294;
UPDATE `creature_template` SET `faction_A` = 16, `faction_H` = 16 WHERE `entry` = 15273;
UPDATE `creature_template` SET `faction_A` = 16, `faction_H` = 16 WHERE `entry` = 15298;

-- might not need to be commented out needs testing 8p
-- disable spell kill command 34260
-- INSERT INTO `spell_disabled` (`entry`) VALUES ('34026 ');

-- spell of light  = insight  
UPDATE `spell_proc_event` SET `SchoolMask`='1', `procFlags`='4' WHERE `entry`='20165'; 

-- req skill fix
UPDATE npc_trainer SET reqskillvalue = 315 WHERE spell = 56991;
UPDATE npc_trainer SET reqskillvalue = 375 WHERE spell = 62162;

-- fixed commander korlug faction on kill according to wowhead.com
UPDATE `creature_onkill_reputation` SET `RewOnKillRepFaction1` = 1068, `RewOnKillRepFaction2` = 1091, `RewOnKillRepValue1` = 63, `RewOnKillRepValue2` = 163 WHERE `creature_id` = 26798;

-- -----------------------------------------------------------------------------------------
/* Scourge Gryphon */ -- to correct rsa typo & Correct gryphon
UPDATE creature_template SET
    spell1 = 57403,
    spell2 = 0,
    spell3 = 0,
    spell4 = 0,
    spell5 = 0,
    spell6 = 0,
    vehicle_id = 25
WHERE entry IN (28864);

DELETE FROM `creature_template_addon` WHERE (`entry`=28864);
INSERT INTO `creature_template_addon` (`entry`, `mount`, `bytes1`, `b2_0_sheath`, `b2_1_pvp_state`, `emote`, `moveflags`, `auras`) VALUES (28864, 0, 0, 0, 0, 0, 0, 61453);
-- -----------------------------------------------------------------------------------------

 -- fix a graphic and minor pet bug for the dragonhawks around silvermoon
UPDATE `creature_template` SET `InhabitType` = 3 WHERE `entry` = 15649;

-- army of dead ghould 
UPDATE `creature_template` SET `ScriptName` = 'mob_risen_ghoul' WHERE `entry` = 24207;

-- Jack Adams dancing on the table
DELETE FROM creature_addon WHERE guid = 115895;
INSERT INTO creature_addon (guid,mount,bytes1,b2_0_sheath,b2_1_pvp_state,emote) VALUES
(115895,0,0,0,0,10);

UPDATE creature_template SET ScriptName = 'npc_olga', AIName = '' WHERE entry = 24639;
UPDATE creature_template SET ScriptName = 'npc_jack_adams', AIName = '' WHERE entry = 24788;

-- Quest gift of the harvester
UPDATE `creature_template` SET `ScriptName` = 'mob_scarlet_miner' WHERE `entry` = 28822;
UPDATE `creature_template` SET `ScriptName` = 'mob_scarlet_miner' WHERE `entry` = 28819;
UPDATE `creature_template` SET `ScriptName` = 'mob_scarlet_miner' WHERE `entry` = 28891;

UPDATE `item_template` SET `ScriptName` = 'mob_scarlet_miner' WHERE `entry` = 39253;

-- should in theory take away the gift of the harvest item out of player inventory upon quest complete // kinda untested didnt notice bug til a few quest  after i did this quest
UPDATE `quest_template` SET `ReqItemId1` = 39253, `ReqItemCount1` = 1 WHERE `entry` = 12698;

-- How To Win Friends And Influence Enemies
UPDATE `creature_template` SET `ScriptName`='npc_crusade_persuaded' WHERE `entry` IN (28939,28940,28610);

-- Bloody Breakout
DELETE FROM `creature` WHERE `id`=28912;
INSERT INTO `creature` (`guid`,`id`,`map`,`spawnMask`,`phaseMask`,`modelid`,`equipment_id`,`position_x`,`position_y`,`position_z`,`orientation`,`spawntimesecs`,`spawndist`,`currentwaypoint`,`curhealth`,`curmana`,`DeathState`,`MovementType`) VALUES
(116831, 28912, 609, 1, 4, 0, 0, 1656.03, -6038.49, 128.644, 2.96436, 25, 0, 0, 392100, 0, 0, 0);
UPDATE `creature_template` SET `mechanic_immune_mask` = 2146435071 WHERE `entry` = 28912; -- ( suppose to make deathweaver immune to turn but doesnt but it is the correct mask )

UPDATE `creature_template` SET `ScriptName`='mob_high_inquisitor_valroth',minmana=6489,maxmana=6489,unit_flags=32768,ainame="" WHERE `entry`='29001';

DELETE FROM `creature` WHERE `id`=29007;
UPDATE `creature_template` SET `AIName`='EventAI',minmana=1020,maxmana=1058,unit_flags=32768 WHERE (`entry`='29007');

UPDATE `creature_template` SET `AIName` = 'EventAI', `ScriptName` = '' WHERE `entry` = '29007';
DELETE FROM `creature_ai_scripts` WHERE (`id`='2900702');
INSERT INTO `creature_ai_scripts` VALUES ('2900702', '29007', '0', '0', '100', '1', '1000', '4000', '1000', '4000', '11', '15498', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'Crimson Acolyte - Holy Smite');

UPDATE `creature_template` SET `AIName` = 'EventAI', `ScriptName` = '' WHERE `entry` = '29007';
DELETE FROM `creature_ai_scripts` WHERE (`id`='2900701');
INSERT INTO `creature_ai_scripts` VALUES ('2900701', '29007', '4', '0', '100', '0', '0', '0', '0', '0', '11', '15498', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'Crimson Acolyte on aggro cast smite');


UPDATE `creature_template` SET `AIName` = 'EventAI', `ScriptName` = '' WHERE `entry` = '29007';
DELETE FROM `creature_ai_scripts` WHERE (`id`='2900703');
INSERT INTO `creature_ai_scripts` VALUES ('2900703', '29007', '11', '0', '100', '1', '0', '0', '0', '0', '11', '34809', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', 'Crimson Acolyte - Cast Holy Fury on Spawn');

-- Ambush At The Overlook
DELETE FROM `creature` WHERE `id`=28957;
INSERT INTO `creature` (`guid`,`id`,`map`,`spawnMask`,`phaseMask`,`modelid`,`equipment_id`,`position_x`,`position_y`,`position_z`,`orientation`,`spawntimesecs`,`spawndist`,`currentwaypoint`,`curhealth`,`curmana`,`DeathState`,`MovementType`) VALUES
(7779004, 28957, 609, 1, 65535, 0, 0, 1453.79, -5998.23, 117.191, 1.11162, 25, 0, 0, 1, 0, 0, 0);
UPDATE `creature_template` SET `ScriptName`='mob_scarlet_courier_controller' WHERE `entry`='28957';
UPDATE `creature_template` SET `ScriptName`='mob_scarlet_courier' WHERE `entry`='29076';

UPDATE `quest_template` SET `ReqItemId3` = 39645, `ReqItemCount3` = 1 WHERE `entry` = 12754; -- Takes back the item used to use to make cover

-- an end to all things
UPDATE `quest_template` SET `ReqItemId1` = 39700, `ReqItemCount1` = 1 WHERE `entry` = 12779; -- make horn of frost a required item in quest this is so players arent left with the item

-- scarlet monastry headless horseman
UPDATE creature_template SET ScriptName = 'boss_headless_horseman' WHERE entry = 23682;
UPDATE creature_template SET ScriptName = 'npc_horsemans_head' WHERE entry = 23775;

-- Head of the Horseman
UPDATE `creature_template` SET 
`minlevel`='70',
`maxlevel`='70',
`minhealth`='63000',
`maxhealth`='63000',
`armor`='1000',
`mindmg`='1200',
`maxdmg`='1600',
`faction_A`='14',
`faction_H`='14',
`mechanic_immune_mask`='2048' 
WHERE `entry`='23775';

UPDATE creature_template SET 
`minlevel` = 70,
`maxlevel` = 70,
`minhealth` = 4541,
`maxhealth` = 4541,
`armor` = 6792,
`mindmg` = 651,
`maxdmg` = 834,
`baseattacktime` = 2000,
`mechanic_immune_mask` = '8388624',
`faction_A` = 14,
`faction_H` = 14
WHERE entry IN (23545,23694);

UPDATE creature_template SET ScriptName = 'mob_pulsing_pumpkin' WHERE entry = 23694;

-- ACID scripts for Scarlet Enclave mobs
-- script_texts
DELETE FROM `creature_ai_texts` WHERE `entry` BETWEEN -286099 AND -286092;
INSERT INTO `creature_ai_texts` VALUES
(-286099, "You don't have to do this! Nobody has to die!", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Scarlet Peasant - Say Fear01"),
(-286098, "Let me live! I'll do whatever you say!", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Scarlet Peasant - Say Fear02"),
(-286097, "Ugh... I... I think I pooped...", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Scarlet Peasant - Say Fear03"),
(-286096, "I picked the wrong day to quit drinkin'!", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Scarlet Peasant - Say Fear04"),
(-286095, "Don't kill me! I only took this job for benefits!", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Scarlet Peasant - Say Fear05"),
(-286094, "Have mercy, sir!", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Citizen of Heavenshire - Say Aggro1"),
(-286093, "You may take my life, but you won't take my freedom!", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Citizen of Heavenshire - Say Enrage1"),
(-286092, "DIE!", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Citizen of Heavenshire - Say Enrage2");

-- scripts
UPDATE `creature_template` SET `AIName` = "EventAI", `scriptname` = "" WHERE entry IN (28834, 28856, 28936, 28850, 29104, 28941, 28942, 28577, 28576, 28557);

DELETE FROM `creature_ai_scripts` WHERE `creature_id` IN (28834, 28856, 28936, 28850, 29104, 28941, 28942, 28577, 28576, 28557);
INSERT INTO `creature_ai_scripts` VALUES
-- Scarlet Defender
(2883401, 28834, 0, 0, 100, 1, 0, 0, 3000, 3000, 11, 52566, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Scarlet Defender - Cast Shoot"),
-- Scarlet Guardian
(2885601, 28856, 4, 0, 100, 0, 0, 0, 0, 0, 21, 0, 0, 0, 22, 2, 0, 0, 0, 0, 0, 0, "Scarlet Guardian - Stop moving and set Phase 2 on aggro"),
(2885602, 28856, 9, 1, 100, 0, 10, 150, 0, 0, 21, 0, 0, 0, 22, 2, 0, 0, 0, 0, 0, 0, "Scarlet Guardian - Stop moving and set Phase 2 at 10yd range"),
(2885603, 28856, 0, 1, 100, 1, 3000, 3000, 3000, 3000, 11, 25710, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Scarlet Guardian - Cast Heroic Strike(Phase 1)"),
(2885604, 28856, 0, 2, 100, 1, 0, 0, 3000, 3000, 11, 52566, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Scarlet Guardian - Cast Shoot(Phase 2)"),
(2885605, 28856, 9, 2, 100, 0, 0, 10, 0, 0, 21, 1, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, "Scarlet Guardian - Set Phase 1 at less then 10yd (Phase 2)"),
(2885606, 28856, 9, 0, 100, 0, 150, 300, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Scarlet Guardian - Evade 150yd"),
-- Scarlet Commander
(2893601, 28936, 4, 0, 100, 0, 0, 0, 0, 0, 21, 0, 0, 0, 22, 2, 0, 0, 0, 0, 0, 0, "Scarlet Commander - Stop moving and set Phase 2 on aggro"),
(2893602, 28936, 9, 1, 100, 0, 10, 150, 0, 0, 21, 0, 0, 0, 22, 2, 0, 0, 0, 0, 0, 0, "Scarlet Commander - Stop moving and set Phase 2 at 10yd range"),
(2893603, 28936, 0, 1, 100, 1, 3000, 3000, 3000, 3000, 11, 25710, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Scarlet Commander - Cast Heroic Strike(Phase 1)"),
(2893604, 28936, 0, 2, 100, 1, 0, 0, 3000, 3000, 11, 52566, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Scarlet Commander - Cast Shoot(Phase 2)"),
(2893605, 28936, 9, 2, 100, 0, 0, 10, 0, 0, 21, 1, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, "Scarlet Commander - Set Phase 1 at less then 10yd (Phase 2)"),
(2893606, 28936, 9, 0, 100, 0, 150, 300, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Scarlet Commander - Evade 150yd"),
-- Scarlet Cannon
(2885001, 28850, 0, 0, 100, 1, 0, 0, 3000, 3000, 11, 52539, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Scarlet Cannon - Cast Cannonball"),
(2885002, 28850, 9, 0, 100, 0, 150, 300, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Scarlet Cannon - Evade 150yd"),
-- Scarlet Balista
(2910401, 29104, 0, 0, 100, 1, 0, 0, 3000, 3000, 11, 53117, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Scarlet Balista - Cast Ballista assault"),
(2910402, 29104, 9, 0, 100, 0, 150, 300, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Scarlet Balista - Evade 150yd"),
-- Citizens of New Avalon
(2894101, 28941, 11, 0, 100, 1, 0, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Citizen of New Avalon - Spawn set Phase 1"),
(2894102, 28941, 0, 0, 80, 1, 0, 0, 10000, 10000, 11, 52716, 1, 1, 22, 2, 0, 0, 1, -286094, -286099, -286095, "Citizen of New Avalon - On Aggro: Escape in fear and set Phase 2"),
(2894103, 28941, 0, 0, 20, 1, 0, 0, 10000, 10000, 11, 52262, 1, 1, 22, 2, 0, 0, 1, -286093, -286092, 0, "Citizens of New Avalon - On Aggro: Enrage and set Phase 2"),
(2894201, 28942, 11, 0, 100, 1, 0, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Citizen of New Avalon - Spawn set Phase 1"),
(2894202, 28942, 0, 0, 80, 1, 0, 0, 10000, 10000, 11, 52716, 1, 1, 22, 2, 0, 0, 1, -286094, -286099, -286096, "Citizen of New Avalon - On Aggro: Escape in fear and set Phase 2"),
(2894203, 28942, 0, 0, 20, 1, 0, 0, 10000, 10000, 11, 52262, 1, 1, 22, 2, 0, 0, 1, -286093, -286092, 0, "Citizens of New Avalon - On Aggro: Enrage and set Phase 2"),
-- Citizens of Havenshire
(2857601, 28576, 11, 0, 100, 1, 0, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Citizen of New Havenshire - Spawn set Phase 1"),
(2857602, 28576, 4, 0, 80, 0, 0, 0, 0, 0, 11, 52716, 1, 1, 1, -286094, -286099, -286096, 0, 0, 0, 0, "Citizen of Havenshire - On Aggro: Beg for mercy"),
(2857603, 28576, 0, 0, 100, 1, 0, 0, 1000, 1000, 20, 0, 0, 0, 21, 0, 0, 0, 5, 20, 0, 0, "Citizen of Havenshire - Cower (Phase 3)"),
(2857604, 28576, 4, 0, 20, 0, 0, 0, 0, 0, 11, 52262, 1, 1, 1, -286093, -286092, 0, 0, 0, 0, 0, "Citizens of Havenshire - On Aggro: Enrage"),
(2857605, 28576, 7, 0, 100, 0, 0, 0, 0, 0, 5, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, "Citizens of Havenshire - Reset State"),

(2857701, 28577, 11, 0, 100, 1, 0, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Citizen of Havenshire - Spawn set Phase 1"),
(2857702, 28577, 4, 0, 80, 0, 0, 0, 0, 0, 11, 52716, 1, 1, 1, -286094, -286099, -286095, 22, 3, 0, 0, "Citizen of Havenshire - On Aggro: Beg for mercy"),
(2857703, 28577, 0, 0, 100, 1, 0, 0, 1000, 1000, 20, 0, 0, 0, 5, 431, 0, 0, 21, 0, 0, 0, "Citizen of Havenshire - Cower (Phase 3)"),
(2857704, 28577, 4, 0, 20, 0, 0, 0, 0, 0, 11, 52262, 1, 1, 1, -286093, -286092, 0, 0, 0, 0, 0, "Citizens of Havenshire - On Aggro: Enrage"),
(2857705, 28577, 7, 0, 100, 0, 0, 0, 0, 0, 5, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, "Citizens of Havenshire - Reset State"),
-- Scarlet Peasant
(2855701, 28557, 11, 0, 100, 1, 0, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Scarlet Peasant - Spawn set Phase 1"),
(2855702, 28557, 4, 1, 80, 0, 0, 0, 0, 0, 11, 52716, 1, 1, 1, -286099, -286098, -286097, 22, 2, 0, 0, "Scarlet Peasant - On Aggro: Beg for mercy"),
(2855703, 28557, 0, 0, 100, 1, 0, 0, 1000, 1000, 20, 0, 0, 0, 5, 20, 0, 0, 21, 0, 0, 0, "Scarlet Peasant - Cower (Phase 2)"),
(2855704, 28557, 7, 0, 100, 0, 0, 0, 0, 0, 5, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, "Scarlet Peasant - Reset State"),
(2889201, 28892, 11, 0, 100, 1, 0, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Scarlet Peasant - Spawn set Phase 1"),
(2889202, 28892, 4, 1, 80, 0, 0, 0, 0, 0, 11, 52716, 1, 1, 1, -286099, -286098, -286097, 22, 2, 0, 0, "Scarlet Peasant - On Aggro: Beg for mercy"),
(2889203, 28892, 0, 0, 100, 1, 0, 0, 1000, 1000, 20, 0, 0, 0, 5, 20, 0, 0, 21, 0, 0, 0, "Scarlet Peasant - Cower (Phase 2)"),
(2889204, 28892, 7, 0, 100, 0, 0, 0, 0, 0, 5, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, "Scarlet Peasant - Reset State");

-- Rite of Vision quest

UPDATE `creature_template` SET `MovementType`='0', `flags_extra`='64', `speed_walk`='1' WHERE (`entry`='2983');
UPDATE `quest_template` SET `SpecialFlags`='2' WHERE (`entry`='772');
UPDATE `creature_template` SET `ScriptName`='npc_plains_vision' WHERE (`entry`='2983');