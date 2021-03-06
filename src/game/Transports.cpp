/*
 * Copyright (C) 2005-2011 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "Common.h"

#include "Transports.h"
#include "MapManager.h"
#include "ObjectMgr.h"
#include "ObjectGuid.h"
#include "Path.h"
#include "Unit.h"

#include "WorldPacket.h"
#include "DBCStores.h"
#include "ProgressBar.h"
#include "ScriptMgr.h"

void MapManager::LoadTransports()
{
    QueryResult *result = WorldDatabase.Query("SELECT entry, name, period FROM transports");

    uint32 count = 0;

    if( !result )
    {
        BarGoLink bar(1);
        bar.step();

        sLog.outString();
        sLog.outString( ">> Loaded %u transports", count );
        return;
    }

    BarGoLink bar(result->GetRowCount());

    do
    {
        bar.step();

        Transport *t = new Transport;

        Field *fields = result->Fetch();

        uint32 entry = fields[0].GetUInt32();
        std::string name = fields[1].GetCppString();
        t->m_period = fields[2].GetUInt32();

        const GameObjectInfo *goinfo = ObjectMgr::GetGameObjectInfo(entry);

        if(!goinfo)
        {
            sLog.outErrorDb("Transport ID:%u, Name: %s, will not be loaded, gameobject_template missing", entry, name.c_str());
            delete t;
            continue;
        }

        if(goinfo->type != GAMEOBJECT_TYPE_MO_TRANSPORT)
        {
            sLog.outErrorDb("Transport ID:%u, Name: %s, will not be loaded, gameobject_template type wrong", entry, name.c_str());
            delete t;
            continue;
        }

        // setting mapID's, binded to transport GO
        if (goinfo->moTransport.mapID)
        {
            m_mapOnTransportGO.insert(std::make_pair(goinfo->moTransport.mapID,t));
            DEBUG_LOG("Loading transport %u between %s, %s map id %u", entry, name.c_str(), goinfo->name, goinfo->moTransport.mapID);
        }

        // sLog.outString("Loading transport %d between %s, %s", entry, name.c_str(), goinfo->name);

        std::set<uint32> mapsUsed;

        if(!t->GenerateWaypoints(goinfo->moTransport.taxiPathId, mapsUsed))
            // skip transports with empty waypoints list
        {
            sLog.outErrorDb("Transport (path id %u) path size = 0. Transport ignored, check DBC files or transport GO data0 field.",goinfo->moTransport.taxiPathId);
            delete t;
            continue;
        }

        float x, y, z, o;
        uint32 mapid;
        x = t->m_WayPoints[0].x; y = t->m_WayPoints[0].y; z = t->m_WayPoints[0].z; mapid = t->m_WayPoints[0].mapid; o = 1;

        //current code does not support transports in dungeon!
        const MapEntry* pMapInfo = sMapStore.LookupEntry(mapid);
        if(!pMapInfo || pMapInfo->Instanceable())
        {
            delete t;
            continue;
        }

        // creates the Gameobject
        if (!t->Create(entry, mapid, x, y, z, o, GO_ANIMPROGRESS_DEFAULT, 0))
        {
            delete t;
            continue;
        }

        m_Transports.insert(t);

        for (std::set<uint32>::const_iterator i = mapsUsed.begin(); i != mapsUsed.end(); ++i)
            m_TransportsByMap[*i].insert(t);

        //If we someday decide to use the grid to track transports, here:
        t->SetMap(sMapMgr.CreateMap(mapid, t));

        //t->GetMap()->Add<GameObject>((GameObject *)t);
        ++count;
    } while(result->NextRow());
    delete result;

    sLog.outString();
    sLog.outString( ">> Loaded %u transports", count );
    sLog.outString( ">> Loaded %u transports with mapID's", m_mapOnTransportGO.size() );

    // check transport data DB integrity
    result = WorldDatabase.Query("SELECT gameobject.guid,gameobject.id,transports.name FROM gameobject,transports WHERE gameobject.id = transports.entry");
    if(result)                                              // wrong data found
    {
        do
        {
            Field *fields = result->Fetch();

            uint32 guid  = fields[0].GetUInt32();
            uint32 entry = fields[1].GetUInt32();
            std::string name = fields[2].GetCppString();
            sLog.outErrorDb("Transport %u '%s' have record (GUID: %u) in `gameobject`. Transports DON'T must have any records in `gameobject` or its behavior will be unpredictable/bugged.",entry,name.c_str(),guid);
        }
        while(result->NextRow());

        delete result;
    }
}

void MapManager::LoadTransportNPCs()
{
    //                                                      0          1                2             3             4             5          6
    QueryResult *result = WorldDatabase.PQuery("SELECT npc_entry, transport_entry, TransOffsetX, TransOffsetY, TransOffsetZ, TransOffsetO, emote FROM creature_transport");

    if (!result)
    {
        sLog.outString(">> Loaded 0 transport NPCs. DB table `creature_transport` is empty!");
        sLog.outString();
        return;
    }

    uint32 count = 0;

    do
    {
        Field *fields = result->Fetch();
        uint32 entry = fields[0].GetUInt32();
        uint32 transportEntry = fields[1].GetUInt32();
        float tX = fields[2].GetFloat();
        float tY = fields[3].GetFloat();
        float tZ = fields[4].GetFloat();
        float tO = fields[5].GetFloat();
        uint32 anim = fields[6].GetUInt32();

        for (MapManager::TransportSet::iterator itr = m_Transports.begin(); itr != m_Transports.end(); ++itr)
        {
            if ((*itr)->GetEntry() == transportEntry)
            {
                (*itr)->AddNPCPassenger(entry, tX, tY, tZ, tO, anim);
                break;
            }
        }

        ++count;
    }
    while (result->NextRow());

    sLog.outString(">> Loaded %u transport npcs", count);
    sLog.outString();
}

bool MapManager::IsTransportMap(uint32 mapid)
{
    TransportGOMap::const_iterator itr = m_mapOnTransportGO.find(mapid);
    if (itr != m_mapOnTransportGO.end())
        return true;
    return false;
}

Transport* MapManager::GetTransportByGOMapId(uint32 mapid)
{
    TransportGOMap::const_iterator itr = m_mapOnTransportGO.find(mapid);
    if (itr != m_mapOnTransportGO.end())
        return itr->second;
    return NULL;
}

Transport::Transport() : GameObject()
{
    m_updateFlag = (UPDATEFLAG_TRANSPORT | UPDATEFLAG_HIGHGUID | UPDATEFLAG_HAS_POSITION | UPDATEFLAG_ROTATION);
}

bool Transport::Create(uint32 guidlow, uint32 mapid, float x, float y, float z, float ang, uint8 animprogress, uint16 dynamicHighValue)
{
    Relocate(x,y,z,ang);
    // instance id and phaseMask isn't set to values different from std.

    if(!IsPositionValid())
    {
        sLog.outError("Transport (GUID: %u) not created. Suggested coordinates isn't valid (X: %f Y: %f)",
            guidlow,x,y);
        return false;
    }

    Object::_Create(ObjectGuid(HIGHGUID_MO_TRANSPORT, guidlow));

    GameObjectInfo const* goinfo = ObjectMgr::GetGameObjectInfo(guidlow);

    if (!goinfo)
    {
        sLog.outErrorDb("Transport not created: entry in `gameobject_template` not found, guidlow: %u map: %u  (X: %f Y: %f Z: %f) ang: %f",guidlow, mapid, x, y, z, ang);
        return false;
    }

    m_goInfo = goinfo;

    SetObjectScale(goinfo->size);

    SetUInt32Value(GAMEOBJECT_FACTION, goinfo->faction);
    //SetUInt32Value(GAMEOBJECT_FLAGS, goinfo->flags);
    SetUInt32Value(GAMEOBJECT_FLAGS, (GO_FLAG_TRANSPORT | GO_FLAG_NODESPAWN));
    SetUInt32Value(GAMEOBJECT_LEVEL, m_period);
    SetEntry(goinfo->id);

    SetDisplayId(goinfo->displayId);

    SetGoState(GO_STATE_READY);
    SetGoType(GameobjectTypes(goinfo->type));
    SetGoArtKit(0);
    SetGoAnimProgress(animprogress);

    // low part always 0, dynamicHighValue is some kind of progression (not implemented)
    SetUInt16Value(GAMEOBJECT_DYNAMIC, 0, 0);
    SetUInt16Value(GAMEOBJECT_DYNAMIC, 1, dynamicHighValue);

    SetName(goinfo->name);

    return true;
}

struct keyFrame
{
    explicit keyFrame(TaxiPathNodeEntry const& _node) : node(&_node),
        distSinceStop(-1.0f), distUntilStop(-1.0f), distFromPrev(-1.0f), tFrom(0.0f), tTo(0.0f)
    {
    }

    TaxiPathNodeEntry const* node;

    float distSinceStop;
    float distUntilStop;
    float distFromPrev;
    float tFrom, tTo;
};

bool Transport::GenerateWaypoints(uint32 pathid, std::set<uint32> &mapids)
{
    if (pathid >= sTaxiPathNodesByPath.size())
        return false;

    TaxiPathNodeList const& path = sTaxiPathNodesByPath[pathid];

    std::vector<keyFrame> keyFrames;
    int mapChange = 0;
    mapids.clear();
    for (size_t i = 1; i < path.size() - 1; ++i)
    {
        if (mapChange == 0)
        {
            TaxiPathNodeEntry const& node_i = path[i];
            if (node_i.mapid == path[i+1].mapid)
            {
                keyFrame k(node_i);
                keyFrames.push_back(k);
                mapids.insert(k.node->mapid);
            }
            else
            {
                mapChange = 1;
            }
        }
        else
        {
            --mapChange;
        }
    }

    int lastStop = -1;
    int firstStop = -1;

    // first cell is arrived at by teleportation :S
    keyFrames[0].distFromPrev = 0;
    if (keyFrames[0].node->actionFlag == 2)
    {
        lastStop = 0;
    }

    // find the rest of the distances between key points
    for (size_t i = 1; i < keyFrames.size(); ++i)
    {
        if ((keyFrames[i].node->actionFlag == 1) || (keyFrames[i].node->mapid != keyFrames[i-1].node->mapid))
        {
            keyFrames[i].distFromPrev = 0;
        }
        else
        {
            keyFrames[i].distFromPrev =
                sqrt(pow(keyFrames[i].node->x - keyFrames[i - 1].node->x, 2) +
                    pow(keyFrames[i].node->y - keyFrames[i - 1].node->y, 2) +
                    pow(keyFrames[i].node->z - keyFrames[i - 1].node->z, 2));
        }
        if (keyFrames[i].node->actionFlag == 2)
        {
            // remember first stop frame
            if(firstStop == -1)
                firstStop = i;
            lastStop = i;
        }
    }

    float tmpDist = 0;
    for (size_t i = 0; i < keyFrames.size(); ++i)
    {
        int j = (i + lastStop) % keyFrames.size();
        if (keyFrames[j].node->actionFlag == 2)
            tmpDist = 0;
        else
            tmpDist += keyFrames[j].distFromPrev;
        keyFrames[j].distSinceStop = tmpDist;
    }

    for (int i = int(keyFrames.size()) - 1; i >= 0; i--)
    {
        int j = (i + (firstStop+1)) % keyFrames.size();
        tmpDist += keyFrames[(j + 1) % keyFrames.size()].distFromPrev;
        keyFrames[j].distUntilStop = tmpDist;
        if (keyFrames[j].node->actionFlag == 2)
            tmpDist = 0;
    }

    for (size_t i = 0; i < keyFrames.size(); ++i)
    {
        if (keyFrames[i].distSinceStop < (30 * 30 * 0.5f))
            keyFrames[i].tFrom = sqrt(2 * keyFrames[i].distSinceStop);
        else
            keyFrames[i].tFrom = ((keyFrames[i].distSinceStop - (30 * 30 * 0.5f)) / 30) + 30;

        if (keyFrames[i].distUntilStop < (30 * 30 * 0.5f))
            keyFrames[i].tTo = sqrt(2 * keyFrames[i].distUntilStop);
        else
            keyFrames[i].tTo = ((keyFrames[i].distUntilStop - (30 * 30 * 0.5f)) / 30) + 30;

        keyFrames[i].tFrom *= 1000;
        keyFrames[i].tTo *= 1000;
    }

    //    for (int i = 0; i < keyFrames.size(); ++i) {
    //        sLog.outString("%f, %f, %f, %f, %f, %f, %f", keyFrames[i].x, keyFrames[i].y, keyFrames[i].distUntilStop, keyFrames[i].distSinceStop, keyFrames[i].distFromPrev, keyFrames[i].tFrom, keyFrames[i].tTo);
    //    }

    // Now we're completely set up; we can move along the length of each waypoint at 100 ms intervals
    // speed = max(30, t) (remember x = 0.5s^2, and when accelerating, a = 1 unit/s^2
    int t = 0;
    bool teleport = false;
    if (keyFrames[keyFrames.size() - 1].node->mapid != keyFrames[0].node->mapid)
        teleport = true;

    WayPoint pos(keyFrames[0].node->mapid, keyFrames[0].node->x, keyFrames[0].node->y, keyFrames[0].node->z, teleport,
        keyFrames[0].node->arrivalEventID, keyFrames[0].node->departureEventID);
    m_WayPoints[0] = pos;
    t += keyFrames[0].node->delay * 1000;

    uint32 cM = keyFrames[0].node->mapid;
    for (size_t i = 0; i < keyFrames.size() - 1; ++i)
    {
        float d = 0;
        float tFrom = keyFrames[i].tFrom;
        float tTo = keyFrames[i].tTo;

        // keep the generation of all these points; we use only a few now, but may need the others later
        if (((d < keyFrames[i + 1].distFromPrev) && (tTo > 0)))
        {
            while ((d < keyFrames[i + 1].distFromPrev) && (tTo > 0))
            {
                tFrom += 100;
                tTo -= 100;

                if (d > 0)
                {
                    float newX, newY, newZ;
                    newX = keyFrames[i].node->x + (keyFrames[i + 1].node->x - keyFrames[i].node->x) * d / keyFrames[i + 1].distFromPrev;
                    newY = keyFrames[i].node->y + (keyFrames[i + 1].node->y - keyFrames[i].node->y) * d / keyFrames[i + 1].distFromPrev;
                    newZ = keyFrames[i].node->z + (keyFrames[i + 1].node->z - keyFrames[i].node->z) * d / keyFrames[i + 1].distFromPrev;

                    bool teleport = false;
                    if (keyFrames[i].node->mapid != cM)
                    {
                        teleport = true;
                        cM = keyFrames[i].node->mapid;
                    }

                    //                    sLog.outString("T: %d, D: %f, x: %f, y: %f, z: %f", t, d, newX, newY, newZ);
                    WayPoint pos(keyFrames[i].node->mapid, newX, newY, newZ, teleport);
                    if (teleport)
                        m_WayPoints[t] = pos;
                }

                if (tFrom < tTo)                            // caught in tFrom dock's "gravitational pull"
                {
                    if (tFrom <= 30000)
                    {
                        d = 0.5f * (tFrom / 1000) * (tFrom / 1000);
                    }
                    else
                    {
                        d = 0.5f * 30 * 30 + 30 * ((tFrom - 30000) / 1000);
                    }
                    d = d - keyFrames[i].distSinceStop;
                }
                else
                {
                    if (tTo <= 30000)
                    {
                        d = 0.5f * (tTo / 1000) * (tTo / 1000);
                    }
                    else
                    {
                        d = 0.5f * 30 * 30 + 30 * ((tTo - 30000) / 1000);
                    }
                    d = keyFrames[i].distUntilStop - d;
                }
                t += 100;
            }
            t -= 100;
        }

        if (keyFrames[i + 1].tFrom > keyFrames[i + 1].tTo)
            t += 100 - ((long)keyFrames[i + 1].tTo % 100);
        else
            t += (long)keyFrames[i + 1].tTo % 100;

        bool teleport = false;
        if ((keyFrames[i + 1].node->actionFlag == 1) || (keyFrames[i + 1].node->mapid != keyFrames[i].node->mapid))
        {
            teleport = true;
            cM = keyFrames[i + 1].node->mapid;
        }

        WayPoint pos(keyFrames[i + 1].node->mapid, keyFrames[i + 1].node->x, keyFrames[i + 1].node->y, keyFrames[i + 1].node->z, teleport,
            keyFrames[i + 1].node->arrivalEventID, keyFrames[i + 1].node->departureEventID);

        //        sLog.outString("T: %d, x: %f, y: %f, z: %f, t:%d", t, pos.x, pos.y, pos.z, teleport);

        //if (teleport)
        m_WayPoints[t] = pos;

        t += keyFrames[i + 1].node->delay * 1000;
        //        sLog.outString("------");
    }

    uint32 timer = t;

    //    sLog.outDetail("    Generated %lu waypoints, total time %u.", (unsigned long)m_WayPoints.size(), timer);

    m_next = m_WayPoints.begin();                           // will used in MoveToNextWayPoint for init m_curr
    MoveToNextWayPoint();                                   // m_curr -> first point
    MoveToNextWayPoint();                                   // skip first point

    m_pathTime = timer;

    m_nextNodeTime = m_curr->first;

    return true;
}

void Transport::MoveToNextWayPoint()
{
    m_curr = m_next;

    ++m_next;
    if (m_next == m_WayPoints.end())
        m_next = m_WayPoints.begin();
}

void Transport::TeleportTransport(uint32 newMapid, float x, float y, float z)
{
    Map const* oldMap = GetMap();
    Relocate(x, y, z);

    for (UnitSet::iterator itr = _passengers.begin(); itr != _passengers.end();)
    {
        Unit* UnitOnTransport = *itr;
        ++itr;

        if (!UnitOnTransport)
        {
            _passengers.erase(itr);
            continue;
        }

        if(UnitOnTransport->GetTypeId() == TYPEID_UNIT)
            continue;

        Player* PlayerOnTransport = (Player*)UnitOnTransport;
        if (PlayerOnTransport->isDead() && !PlayerOnTransport->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_GHOST))
            PlayerOnTransport->ResurrectPlayer(1.0f);
        PlayerOnTransport->TeleportTo(newMapid, x, y, z, GetOrientation(), TELE_TO_NOT_LEAVE_TRANSPORT);
    }

    //we need to create and save new Map object with 'newMapid' because if not done -> lead to invalid Map object reference...
    //player far teleport would try to create same instance, but we need it NOW for transport...
    //correct me if I'm wrong O.o

    RemoveFromWorld();
    ResetMap();
    Map * newMap = sMapMgr.CreateMap(newMapid, this);
    SetMap(newMap);
    MANGOS_ASSERT (GetMap());
    AddToWorld();

    if(oldMap != newMap)
    {
        UpdateForMap(oldMap);
        UpdateForMap(newMap);
    }

    for (UnitSet::iterator itr = _passengers.begin(); itr != _passengers.end();)
    {
        Unit* UnitOnTransport = *itr;
        ++itr;

        if (!UnitOnTransport)
        {
            _passengers.erase(itr);
            continue;
        }

        if(UnitOnTransport->GetTypeId() == TYPEID_PLAYER)
            continue;

        Creature* npc = (Creature*)UnitOnTransport;
        npc->InterruptNonMeleeSpells(true);
        npc->CombatStop();
        npc->ClearComboPointHolders();
        npc->DeleteThreatList();
        npc->GetMotionMaster()->Clear(false);
        //npc->DestroyForNearbyPlayers();

        npc->RemoveFromWorld();
        npc->ResetMap();
        npc->SetMap(newMap);
        npc->AddToWorld();

        npc->SetPosition(x, y, z, GetOrientation());
    }
}

bool Transport::AddPassenger(Unit* passenger)
{
    if (_passengers.find(passenger) != _passengers.end())
        return false;

    _passengers.insert(passenger);

    DEBUG_LOG("Unit %s boarded transport %s.", passenger->GetName(), GetName());
    return true;
}

bool Transport::RemovePassenger(Unit* passenger)
{
    if (!_passengers.erase(passenger))
        return false;

    DEBUG_LOG("Unit %s removed from transport %s.", passenger->GetName(), GetName());
    return true;
}

void Transport::BuildCreateUpdateBlockForPlayer(UpdateData* data, Player* target)
{
    for (UnitSet::const_iterator itr = _passengers.begin(); itr != _passengers.end(); ++itr)
    {
        if ((*itr)->GetTypeId() == TYPEID_UNIT)
        {
            if (!(*itr))
            {
                _passengers.erase(itr);
                continue;
            }

            (*itr)->BuildCreateUpdateBlockForPlayer(data, target);
        }
    }

    Object::BuildCreateUpdateBlockForPlayer(data, target);
}

void Transport::Update(uint32 update_diff, uint32 /*p_time*/)
{
    if (m_WayPoints.size() <= 1)
        return;

    m_timer = WorldTimer::getMSTime() % m_period;
    while (((m_timer - m_curr->first) % m_pathTime) > ((m_next->first - m_curr->first) % m_pathTime))
    {

        DoEventIfAny(*m_curr,true);

        MoveToNextWayPoint();

        DoEventIfAny(*m_curr,false);

        // first check help in case client-server transport coordinates de-synchronization
        if (m_curr->second.mapid != GetMapId() || m_curr->second.teleport)
        {
            TeleportTransport(m_curr->second.mapid, m_curr->second.x, m_curr->second.y, m_curr->second.z);
        }
        else
        {
            Relocate(m_curr->second.x, m_curr->second.y, m_curr->second.z);
            UpdateNPCPositions(); // COME BACK MARKER
        }

        /*
        for(PlayerSet::const_iterator itr = m_passengers.begin(); itr != m_passengers.end();)
        {
            PlayerSet::const_iterator it2 = itr;
            ++itr;
            //(*it2)->SetPosition( m_curr->second.x + (*it2)->GetTransOffsetX(), m_curr->second.y + (*it2)->GetTransOffsetY(), m_curr->second.z + (*it2)->GetTransOffsetZ(), (*it2)->GetTransOffsetO() );
        }
        */

        m_nextNodeTime = m_curr->first;

        if (m_curr == m_WayPoints.begin())
            DETAIL_FILTER_LOG(LOG_FILTER_TRANSPORT_MOVES, " ************ BEGIN ************** %s", GetName());

        DETAIL_FILTER_LOG(LOG_FILTER_TRANSPORT_MOVES, "%s moved to %f %f %f %d", GetName(), m_curr->second.x, m_curr->second.y, m_curr->second.z, m_curr->second.mapid);
    }
}

void Transport::UpdateForMap(Map const* targetMap)
{
    Map::PlayerList const& pl = targetMap->GetPlayers();
    if(pl.isEmpty())
        return;

    if(GetMapId()==targetMap->GetId())
    {
        for(Map::PlayerList::const_iterator itr = pl.begin(); itr != pl.end(); ++itr)
        {
            if(this != itr->getSource()->GetTransport())
            {
                UpdateData transData;
                BuildCreateUpdateBlockForPlayer(&transData, itr->getSource());
                WorldPacket packet;
                transData.BuildPacket(&packet);
                itr->getSource()->SendDirectMessage(&packet);
            }
        }
    }
    else
    {
        UpdateData transData;
        BuildOutOfRangeUpdateBlock(&transData);
        WorldPacket out_packet;
        transData.BuildPacket(&out_packet);

        for(Map::PlayerList::const_iterator itr = pl.begin(); itr != pl.end(); ++itr)
            if(this != itr->getSource()->GetTransport())
                itr->getSource()->SendDirectMessage(&out_packet);
    }
}

void Transport::DoEventIfAny(WayPointMap::value_type const& node, bool departure)
{
    if (uint32 eventid = departure ? node.second.departureEventID : node.second.arrivalEventID)
    {
        DEBUG_FILTER_LOG(LOG_FILTER_TRANSPORT_MOVES, "Taxi %s event %u of node %u of %s \"%s\") path", departure ? "departure" : "arrival", eventid, node.first, GetGuidStr().c_str(), GetName());

        if (!sScriptMgr.OnProcessEvent(eventid, this, this, departure))
            GetMap()->ScriptsStart(sEventScripts, eventid, this, this);
    }
}

bool Transport::AddNPCPassenger(uint32 entry, float x, float y, float z, float o, uint32 anim)
{
    CreatureInfo const *cinfo = ObjectMgr::GetCreatureTemplate(entry);
    if (!cinfo)
        return false;

    Map* map = GetMap();
    Creature* pCreature = new Creature;

    CreatureCreatePos pos(map, x, y, z, o, GetPhaseMask());

    if (!pCreature->Create(map->GenerateLocalLowGuid(HIGHGUID_UNIT), pos, cinfo))
    {
        delete pCreature;
        return false;
    }

    pCreature->SetTransport(this);

    pCreature->m_movementInfo.AddMovementFlag(MOVEFLAG_ONTRANSPORT);

    pCreature->m_movementInfo.SetTransportData(ObjectGuid(HIGHGUID_MO_TRANSPORT, GetObjectGuid().GetCounter()), x, y, z, o, 0, -1);

    if (anim)
        pCreature->SetUInt32Value(UNIT_NPC_EMOTESTATE, anim);

    AddPassenger(pCreature);

    pCreature->Relocate(
        GetPositionX() + (x * cos(GetOrientation()) + y * sin(GetOrientation() + float(M_PI))),
        GetPositionY() + (y * cos(GetOrientation()) + x * sin(GetOrientation())),
        z + GetPositionZ() ,
        o + GetOrientation());

    if(!pCreature->IsPositionValid())
    {
        sLog.outError("Creature (guidlow %d, entry %d) not created. Suggested coordinates isn't valid (X: %f Y: %f)", pCreature->GetGUIDLow(), pCreature->GetEntry(), pCreature->GetPositionX(), pCreature->GetPositionY());
        delete pCreature;
        return false;
    }

    map->Add(pCreature);

    return true;
}

void Transport::UpdateNPCPositions()
{
    for (UnitSet::iterator itr = _passengers.begin(); itr != _passengers.end(); ++itr)
    {
        if((*itr)->GetTypeId() == TYPEID_UNIT)
        {
            Creature* npc = ((Creature*)(*itr));

            float x, y, z, o;
            o = GetOrientation() + npc->m_movementInfo.GetTransportPos()->o;
            x = GetPositionX() + (npc->m_movementInfo.GetTransportPos()->x * cos(GetOrientation()) + npc->m_movementInfo.GetTransportPos()->y * sin(GetOrientation() + M_PI));
            y = GetPositionY() + (npc->m_movementInfo.GetTransportPos()->y * cos(GetOrientation()) + npc->m_movementInfo.GetTransportPos()->x * sin(GetOrientation()));
            z = GetPositionZ() + npc->m_movementInfo.GetTransportPos()->z;
            GetMap()->CreatureRelocation(npc, x, y, z, o);
        }
    }
}
