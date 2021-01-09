#pragma once

#include "core/globals.h"
//#include "script_engine.h"

namespace vspdotnet {}


#define REGISTER_NATIVE(name, func)                     \
  vspdotnet::ScriptEngine::RegisterNativeHandler(#name, func);

#define REGISTER_NATIVES(name, method)                  \
  class Natives##name : public vspdotnet::GlobalClass { \
   public:                                              \
    void OnAllInitialized() override \
    method \
    }; \
        \
  Natives##name g_natives_##name;

#define CREATE_GETTER_FUNCTION(object_name, parameter_type, parameter_name, \
                               from_type, getter)                           \
  static parameter_type object_name##Get##parameter_name(                          \
      ScriptContext& script_context) {                                      \
    auto obj = script_context.GetArgument<from_type>(0);                    \
    return getter;                                                          \
  }

#define CREATE_SETTER_FUNCTION(type_name, get_type, name, from_type, setter) \
  static void type_name##Set##name(ScriptContext& script_context) {                 \
    auto obj = script_context.GetArgument<from_type>(0);                     \
    auto value = script_context.GetArgument<get_type>(1);                    \
    setter;                                                           \
  }