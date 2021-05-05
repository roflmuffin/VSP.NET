/**
* =============================================================================
* Source Python
* Copyright (C) 2012-2015 Source Python Development Team.  All rights reserved.
* =============================================================================
*
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License, version 3.0, as published by the
* Free Software Foundation.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
* details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
* As a special exception, the Source Python Team gives you permission
* to link the code of this program (as well as its derivative works) to
* "Half-Life 2," the "Source Engine," and any Game MODs that run on software
* by the Valve Corporation.  You must obey the GNU General Public License in
* all respects for all other code used.  Additionally, the Source.Python
* Development Team grants this exception to all derivative works.
*/

//-----------------------------------------------------------------------------
// Includes.
//-----------------------------------------------------------------------------
#include "../conversions.h"
#include "core/globals.h"

namespace vspdotnet {

//-----------------------------------------------------------------------------
// Returns an index from the given Edict instance.
//-----------------------------------------------------------------------------
bool IndexFromEdict(edict_t* pEdict, int& output) {
  if (!pEdict || pEdict->IsFree()) return false;

  int iEntityIndex;
#if defined(ENGINE_ORANGEBOX) || defined(ENGINE_BMS) || defined(ENGINE_GMOD)
  iEntityIndex = engine->IndexOfEdict(pEdict);
#else
  iEntityIndex = pEdict - globals::gpGlobals->pEdicts;
#endif

  if (iEntityIndex == INVALID_ENTITY_INDEX) return false;

  output = iEntityIndex;
  return true;
}

//-----------------------------------------------------------------------------
// Returns an index from the given player name.
//-----------------------------------------------------------------------------
bool IndexFromName(const char* szName, int& output) {
  if (!szName || szName[0] == '\0') return false;

  int iEntityIndex = 0;
  IPlayerInfo* pInfo = NULL;
  while (iEntityIndex < globals::gpGlobals->maxClients) {
    iEntityIndex++;
    if (!PlayerInfoFromIndex(iEntityIndex, pInfo)) continue;

    if (V_strcmp(pInfo->GetName(), szName) == 0) {
      output = iEntityIndex;
      return true;
    }
  }

  return false;
}

//-----------------------------------------------------------------------------
// Returns an index from the given BaseEntity instance.
//-----------------------------------------------------------------------------
bool IndexFromBaseEntity(CBaseEntity* pBaseEntity, int& output) {
  if (!pBaseEntity) return false;

  IServerNetworkable* pServerNetworkable =
      ((IServerUnknown*)pBaseEntity)->GetNetworkable();
  if (!pServerNetworkable) return false;

  edict_t* pEdict = pServerNetworkable->GetEdict();
  if (!pEdict || pEdict->IsFree()) return false;

  return IndexFromEdict(pEdict, output);
}

//-----------------------------------------------------------------------------
// Returns an index from the given BaseHandle instance.
//-----------------------------------------------------------------------------
bool IndexFromBaseHandle(CBaseHandle hBaseHandle, int& output) {
  if (!hBaseHandle.IsValid() || !hBaseHandle.ToInt()) return false;

  int iEntityIndex = hBaseHandle.GetEntryIndex();
  if (iEntityIndex == INVALID_ENTITY_INDEX) return false;

  output = iEntityIndex;
  return true;
}

//-----------------------------------------------------------------------------
// Returns an index from the given IntHandle.
//-----------------------------------------------------------------------------
bool IndexFromIntHandle(int iEntityHandle, int& output) {
  if (iEntityHandle == (int)INVALID_EHANDLE_INDEX) return false;

  CBaseHandle hBaseHandle(iEntityHandle);
  int iEntityIndex;
  if (!IndexFromBaseHandle(hBaseHandle, iEntityIndex)) return false;

  edict_t* pEdict;
  if (!EdictFromIndex(iEntityIndex, pEdict)) return false;

  CBaseHandle hTestHandle;
  if (!BaseHandleFromEdict(pEdict, hTestHandle)) return false;

  if (!hTestHandle.IsValid() ||
      hBaseHandle.GetSerialNumber() != hTestHandle.GetSerialNumber())
    return false;

  output = iEntityIndex;
  return true;
}

//-----------------------------------------------------------------------------
// Returns an index from the given UserID.
//-----------------------------------------------------------------------------
bool IndexFromUserid(int iUserID, int& output) {
  edict_t* pEdict;
  if (!EdictFromUserid(iUserID, pEdict)) return false;

  return IndexFromEdict(pEdict, output);
}

//-----------------------------------------------------------------------------
// Returns an index instance from the given PlayerInfo instance.
//-----------------------------------------------------------------------------
bool IndexFromPlayerInfo(IPlayerInfo* pPlayerInfo, int& output) {
  edict_t* pEdict;
  if (!EdictFromPlayerInfo(pPlayerInfo, pEdict)) return false;

  return IndexFromEdict(pEdict, output);
}

//-----------------------------------------------------------------------------
// Returns an index instance from the given SteamID.
//-----------------------------------------------------------------------------
bool IndexFromSteamID(const char* szSteamID, int& output) {
  IPlayerInfo* pInfo = NULL;

  for (int i = 1; i <= (int)globals::gpGlobals->maxClients; ++i) {
    if (!PlayerInfoFromIndex(i, pInfo)) continue;

    if (V_strcmp(pInfo->GetNetworkIDString(), szSteamID) == 0) {
      output = i;
      return true;
    }
  }

  return false;
}

//-----------------------------------------------------------------------------
// Returns an index instance from the given SteamID.
//-----------------------------------------------------------------------------
bool IndexFromUniqueID(const char* szUniqueID, int& output) {
  for (int i = 1; i <= (int)globals::gpGlobals->maxClients; ++i) {
    IPlayerInfo* pInfo = NULL;
    if (!PlayerInfoFromIndex(i, pInfo)) continue;

    char szTempUniqueID[UNIQUE_ID_SIZE] = "";
    char* pTempUniqueID = (char*)szTempUniqueID;
    if (!UniqueIDFromPlayerInfo2(pInfo, pTempUniqueID)) continue;

    if (V_strcmp(szUniqueID, pTempUniqueID) == 0) {
      output = i;
      return true;
    }
  }

  return false;
}
}  // namespace vspdotnet