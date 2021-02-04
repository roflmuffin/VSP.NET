#include <igameevents.h>
#include <core/utilities/conversions.h>


#include "core/autonative.h"
#include "core/event_manager.h"
#include "core/globals.h"
#include "core/log.h"
#include "core/entity.h"

namespace vspdotnet {

static void HookEvent(ScriptContext& script_context)
{
  const char* name = script_context.GetArgument<const char*>(0);
  auto callback = script_context.GetArgument<CallbackT>(1);
  auto post = script_context.GetArgument<bool>(2);

  globals::event_manager.HookEvent(name, callback, post);
}

static void UnhookEvent(ScriptContext& script_context) {
  const char* name = script_context.GetArgument<const char*>(0);
  auto callback = script_context.GetArgument<CallbackT>(1);
  auto post = script_context.GetArgument<bool>(2);

  globals::event_manager.UnhookEvent(name, callback, post);
}

static IGameEvent* CreateEvent(ScriptContext& script_context)
{
  auto name = script_context.GetArgument<const char*>(0);
  bool force = script_context.GetArgument<bool>(1);


  return globals::gameeventmanager->CreateEvent(name, force);
}

static void FireEvent(ScriptContext& script_context)
{
  auto game_event = script_context.GetArgument<IGameEvent*>(0);
  bool dont_broadcast = script_context.GetArgument<bool>(1);
  if (!game_event)
  {
    script_context.ThrowNativeError("Invalid game event");    
  }

  globals::gameeventmanager->FireEvent(game_event, dont_broadcast);
}

static void FireEventToClient(ScriptContext& script_context) {
  auto game_event = script_context.GetArgument<IGameEvent*>(0);
  int client = script_context.GetArgument<int>(1);
  if (!game_event) {
    script_context.ThrowNativeError("Invalid game event");
  }

  auto base_entity = reinterpret_cast<CBaseEntityWrapper*>(ExcBaseEntityFromIndex(client));
  if (!base_entity)
  {
    script_context.ThrowNativeError("Invalid client index");
  }

  auto iclient = base_entity->GetIClient();
  IGameEventListener2* game_client = (IGameEventListener2*)((intptr_t)iclient - sizeof(void*));

  game_client->FireGameEvent(game_event);
}

static const char* GetEventName(ScriptContext& script_context) {
  IGameEvent* game_event = script_context.GetArgument<IGameEvent*>(0);

  if (!game_event) {
    script_context.ThrowNativeError("Invalid game event");
    return nullptr;
  }

  return game_event->GetName();
}

static bool GetEventBool(ScriptContext& script_context) {
  IGameEvent* game_event = script_context.GetArgument<IGameEvent*>(0);
  const char* key_name = script_context.GetArgument<const char*>(1);

  if (!game_event) {
    script_context.ThrowNativeError("Invalid game event");
    return nullptr;
  }

  return game_event->GetBool(key_name);
}

static int GetEventInt(ScriptContext& script_context) {
  IGameEvent* game_event = script_context.GetArgument<IGameEvent*>(0);
  const char* key_name = script_context.GetArgument<const char*>(1);

  if (!game_event) {
    script_context.ThrowNativeError("Invalid game event");
    return -1;
  }

  return game_event->GetInt(key_name);
}

static float GetEventFloat(ScriptContext& script_context) {
  IGameEvent* game_event = script_context.GetArgument<IGameEvent*>(0);
  const char* key_name = script_context.GetArgument<const char*>(1);

  if (!game_event) {
    script_context.ThrowNativeError("Invalid game event");
    return -1;
  }

  return game_event->GetFloat(key_name);
}

static const char* GetEventString(ScriptContext& script_context)
{
  IGameEvent* game_event = script_context.GetArgument<IGameEvent*>(0);
  const char* key_name = script_context.GetArgument<const char*>(1);

  if (!game_event) {
    script_context.ThrowNativeError("Invalid game event");
    return nullptr;
  }

  return game_event->GetString(key_name);
}


static void SetEventBool(ScriptContext& script_context) {
  IGameEvent* game_event = script_context.GetArgument<IGameEvent*>(0);
  const char* key_name = script_context.GetArgument<const char*>(1);
  const bool value = script_context.GetArgument<bool>(2);

  if (game_event) {
    game_event->SetBool(key_name, value);
  }
}

static void SetEventInt(ScriptContext& script_context) {
  IGameEvent* game_event = script_context.GetArgument<IGameEvent*>(0);
  const char* key_name = script_context.GetArgument<const char*>(1);
  const int value = script_context.GetArgument<int>(2);

  if (game_event) {
    game_event->SetInt(key_name, value);
  }
}

static void SetEventFloat(ScriptContext& script_context) {
  IGameEvent* game_event = script_context.GetArgument<IGameEvent*>(0);
  const char* key_name = script_context.GetArgument<const char*>(1);
  const float value = script_context.GetArgument<float>(2);

  if (game_event) {
    game_event->SetFloat(key_name, value);
  }
}

static void SetEventString(ScriptContext& script_context) {
  IGameEvent* game_event = script_context.GetArgument<IGameEvent*>(0);
  const char* key_name = script_context.GetArgument<const char*>(1);
  const char* value = script_context.GetArgument<const char*>(2);

  if (game_event) {
    game_event->SetString(key_name, value);
  }
}

static int LoadEventsFromFile(ScriptContext& script_context)
{
  auto path = script_context.GetArgument<const char*>(0);

  return globals::gameeventmanager->LoadEventsFromFile(path);
}

REGISTER_NATIVES(events, {
  ScriptEngine::RegisterNativeHandler("HOOK_EVENT", HookEvent);
  ScriptEngine::RegisterNativeHandler("UNHOOK_EVENT", UnhookEvent);
  ScriptEngine::RegisterNativeHandler("CREATE_EVENT", CreateEvent);
  ScriptEngine::RegisterNativeHandler("FIRE_EVENT", FireEvent);
  ScriptEngine::RegisterNativeHandler("FIRE_EVENT_TO_CLIENT", FireEventToClient);

  ScriptEngine::RegisterNativeHandler("GET_EVENT_NAME", GetEventName);
  ScriptEngine::RegisterNativeHandler("GET_EVENT_BOOL", GetEventBool);
  ScriptEngine::RegisterNativeHandler("GET_EVENT_FLOAT", GetEventFloat);
  ScriptEngine::RegisterNativeHandler("GET_EVENT_STRING", GetEventString);
  ScriptEngine::RegisterNativeHandler("GET_EVENT_INT", GetEventInt);

  ScriptEngine::RegisterNativeHandler("SET_EVENT_BOOL", SetEventBool);
  ScriptEngine::RegisterNativeHandler("SET_EVENT_FLOAT", SetEventFloat);
  ScriptEngine::RegisterNativeHandler("SET_EVENT_STRING", SetEventString);
  ScriptEngine::RegisterNativeHandler("SET_EVENT_INT", SetEventInt);

  ScriptEngine::RegisterNativeHandler("LOAD_EVENTS_FROM_FILE", LoadEventsFromFile);
})

}