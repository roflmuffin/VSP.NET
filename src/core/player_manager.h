/**
 * =============================================================================
 * SourceMod
 * Copyright (C) 2004-2016 AlliedModders LLC.  All rights reserved.
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
 * As a special exception, AlliedModders LLC gives you permission to link the
 * code of this program (as well as its derivative works) to "Half-Life 2," the
 * "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
 * by the Valve Corporation.  You must obey the GNU General Public License in
 * all respects for all other code used.  Additionally, AlliedModders LLC grants
 * this exception to all derivative works.  AlliedModders LLC defines further
 * exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
 * or <http://www.sourcemod.net/license.php>.
 *
 * This file has been modified from its original form, under the GNU General
 * Public License, version 3.0.
 */

#pragma once

#include <string>

#include "core/globals.h"
#include "core/global_listener.h"

class CBaseEntity;
class INetChannelInfo;
class IPlayerInfo;
struct edict_t;

namespace vspdotnet {
class ScriptCallback;
class CBaseEntityWrapper;

class CPlayer {
  friend class PlayerManager;
 public:
  CPlayer();

 public:
  void Initialize(const char* name, const char* ip, edict_t* pEntity);
  void Connect();
  void Disconnect();
  edict_t* GetEdict() const;
  IPlayerInfo* GetPlayerInfo() const;
  bool WasCountedAsInGame() const;
  int GetUserId();
  CBaseEntity* GetBaseEntity() const;
  bool IsAuthStringValidated() const;
  void Authorize();

 public:
  const char* GetName() const;
  const char* GetAuthString();
  bool IsConnected() const;
  bool IsFakeClient() const;
  bool IsAuthorized() const;
  void PrintToConsole(const char* message) const;
  void PrintToChat(const char* message);
  void PrintToHint(const char* message);
  void PrintToCenter(const char* message);
  QAngle GetAbsAngles() const;
  Vector GetAbsOrigin() const;
  bool IsAlive() const;
  bool IsInGame() const;
  void Kick(const char* kickReason);
  const char* GetWeaponName() const;
  void ChangeTeam(int team) const;
  int GetTeam() const;
  CBaseEntityWrapper* GetEntity() const;
  int GetArmor() const;
  int GetFrags() const;
  int GetDeaths() const;
  const char* GetKeyValue(const char* key) const;
  Vector GetMaxSize() const;
  Vector GetMinSize() const;
  int GetMaxHealth() const;
  const char* GetIpAddress() const;
  const char* GetModelName() const;
  int GetUserId() const;
  float GetTimeConnected() const;
  float GetLatency() const;

 private:
  std::string m_name;
  edict_t* m_p_edict = nullptr;
  IPlayerInfo* m_info = nullptr;
  std::string m_auth_id;
  bool m_is_connected = false;
  bool m_is_fake_client = false;
  bool m_is_in_game = false;
  bool m_is_authorized = false;
  int m_user_id = 1;
  int m_i_index;
  std::string m_ip_address;
  void SetName(const char* name);
  INetChannelInfo* GetNetInfo() const;
};


class PlayerManager : public GlobalClass {
  friend class CPlayer;

 public:
  PlayerManager();
  void OnStartup() override;
  void OnAllInitialized() override;
  bool OnClientConnect(edict_t* pEntity, const char* pszName,
                       const char* pszAddress, char* reject, int maxrejectlen);
  bool OnClientConnect_Post(edict_t* pEntity, const char* pszName,
                            const char* pszAddress, char* reject,
                            int maxrejectlen);
  void OnClientPutInServer(edict_t* pEntity, char const* playername);
  void OnClientDisconnect(edict_t* pEntity);
  void OnClientDisconnect_Post(edict_t* pEntity) const;
  void OnServerActivate(edict_t* pEdictList, int edictCount,
                        int clientMax) const;
  void OnGameFrame(bool simulating) const;
  void OnThink(bool last_tick) const;
  void OnShutdown() override;
  void OnLevelEnd() override;
  void OnClientCommand(edict_t* pEntity, const CCommand& args) const;
  int ListenClient() const;

  public:
   int NumPlayers() const;
   int MaxClients() const;
   CPlayer* GetPlayerByIndex(int client) const;
   CPlayer* GetClientOfUserId(int user_id) const;

  private:
   void InvalidatePlayer(CPlayer* pPlayer) const;

   CPlayer* m_players;
   int m_max_clients = 0;
   int m_player_count = 0;
   int* m_user_id_lookup;
   int m_listen_client;
   bool m_is_listen_server;

  ScriptCallback* m_on_client_connect_callback;
  ScriptCallback* m_on_client_put_in_server_callback;
  ScriptCallback* m_on_client_connected_callback;
  ScriptCallback* m_on_client_disconnect_callback;
  ScriptCallback* m_on_client_disconnect_post_callback;
  ScriptCallback* m_on_activate_callback;
};

}  // namespace vspdotnet