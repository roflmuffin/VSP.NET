/*
 *  This file is part of VSP.NET.
 *  VSP.NET is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  VSP.NET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with VSP.NET.  If not, see <https://www.gnu.org/licenses/>. *
 */

#include "core/dotnet_host.h"

#include <netcore/coreclr_delegates.h>
#include <netcore/hostfxr.h>

#include <codecvt>
#include <locale>

#ifdef WIN32
#include <direct.h>
#include <Windows.h>

#define STR(s) L##s
#define CH(c) L##c
#define DIR_SEPARATOR L'\\'

#endif

#include <iostream>

#include "core/log.h"
#include "core/utils.h"

std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

namespace {
hostfxr_initialize_for_runtime_config_fn init_fptr;
hostfxr_get_runtime_delegate_fn get_delegate_fptr;
hostfxr_close_fn close_fptr;
hostfxr_handle cxt;

bool load_hostfxr();
load_assembly_and_get_function_pointer_fn get_dotnet_load_assembly(
    const char_t *assembly);
}  // namespace

namespace {
// Forward declarations
void *load_library(const char_t *);
void *get_export(void *, const char *);

#ifdef _WINDOWS
void *load_library(const char_t *path) {
  HMODULE h = ::LoadLibraryW(path);
  assert(h != nullptr);
  return (void *)h;
}

void *get_export(void *h, const char *name) {
  void *f = ::GetProcAddress((HMODULE)h, name);
  assert(f != nullptr);
  return f;
}
#else
void *load_library(const char_t *path) {
  void *h = dlopen(path, RTLD_LAZY | RTLD_LOCAL);
  assert(h != nullptr);
  return h;
}
void *get_export(void *h, const char *name) {
  void *f = dlsym(h, name);
  assert(f != nullptr);
  return f;
}
#endif

// <SnippetLoadHostFxr>
// Using the nethost library, discover the location of hostfxr and get exports
bool load_hostfxr() {
  std::string baseDir = vspdotnet::utils::CSGODotNetDirectory();

  std::wstring buffer =
      converter.from_bytes(baseDir + "/dotnet/host/fxr/5.0.1/hostfxr.dll");

  // Load hostfxr and get desired exports
  void* lib = load_library(buffer.c_str());
  init_fptr = (hostfxr_initialize_for_runtime_config_fn)get_export(
      lib, "hostfxr_initialize_for_runtime_config");
  get_delegate_fptr = (hostfxr_get_runtime_delegate_fn)get_export(
      lib, "hostfxr_get_runtime_delegate");
  close_fptr = (hostfxr_close_fn)get_export(lib, "hostfxr_close");

  return (init_fptr && get_delegate_fptr && close_fptr);
}
// </SnippetLoadHostFxr>

// <SnippetInitialize>
// Load and initialize .NET Core and get desired function pointer for scenario
load_assembly_and_get_function_pointer_fn get_dotnet_load_assembly(
    const char_t *config_path) {
  // Load .NET Core
  void *load_assembly_and_get_function_pointer = nullptr;
  int rc = init_fptr(config_path, nullptr, &cxt);
  if (rc != 0 || cxt == nullptr) {
    std::cerr << "Init failed: " << std::hex << std::showbase << rc
              << std::endl;
    close_fptr(cxt);
    return nullptr;
  }

  // Get the load assembly function pointer
  rc = get_delegate_fptr(cxt, hdt_load_assembly_and_get_function_pointer,
                         &load_assembly_and_get_function_pointer);
  if (rc != 0 || load_assembly_and_get_function_pointer == nullptr)
    std::cerr << "Get delegate failed: " << std::hex << std::showbase << rc
              << std::endl;

  //close_fptr(cxt);
  return (load_assembly_and_get_function_pointer_fn)
      load_assembly_and_get_function_pointer;
}

}  // namespace

CDotNetManager::CDotNetManager()
{
}

CDotNetManager::~CDotNetManager()
{
}

bool CDotNetManager::Initialize()
{
  std::string baseDir = vspdotnet::utils::CSGODotNetDirectory();

  if (!load_hostfxr())
  {
    VSPDN_CORE_ERROR("Failed to initialize .NET");
    return false;
  }

  std::wstring wideStr = converter.from_bytes((baseDir + "/api/CSGONET.API.runtimeconfig.json").c_str());


  load_assembly_and_get_function_pointer_fn
      load_assembly_and_get_function_pointer = nullptr;
  load_assembly_and_get_function_pointer = get_dotnet_load_assembly(wideStr.c_str());
  assert(load_assembly_and_get_function_pointer != nullptr &&
         "Failure: get_dotnet_load_assembly()");

  const std::wstring dotnetlib_path = converter.from_bytes((baseDir + "/api/CSGONET.API.dll").c_str());
  const char_t *dotnet_type = STR("CSGONET.API.Core.Helpers, CSGONET.API");
  // Namespace, assembly name

  typedef int(CORECLR_DELEGATE_CALLTYPE *
               custom_entry_point_fn)();
  custom_entry_point_fn entry_point = nullptr;
  int rc = load_assembly_and_get_function_pointer(
      dotnetlib_path.c_str(), dotnet_type,
      STR("LoadAllPlugins") /*method_name*/,
      UNMANAGEDCALLERSONLY_METHOD, nullptr, (void **)&entry_point);
  assert(rc == 0 && entry_point != nullptr &&
         "Failure: load_assembly_and_get_function_pointer()");

  const bool success = entry_point();
  if (!success)
  {
    VSPDN_CORE_ERROR("Failed to initialize .NET");
    return false;
  }

  VSPDN_CORE_INFO(".NET Initialized.");
  return true;
}

void CDotNetManager::UnloadPlugin(PluginContext* context)
{
}

void CDotNetManager::Shutdown()
{
  // CoreCLR does not currently supporting unloading... :(
}


PluginContext* CDotNetManager::FindContext(std::string path)
{ return nullptr; }

