#ifndef MODIO_MOD_METHODS_H
#define MODIO_MOD_METHODS_H

#include "wrappers/CurlWrapper.h"
#include "wrappers/MinizipWrapper.h"
#include "data_containers/Mod.h"
#include "Globals.h"
#include "Filter.h"
#include "handlers/ModHandler.h"

#ifdef WINDOWS
#  ifdef BUILDING_MODIO_DLL
#    define MODIO_DLL __declspec(dllexport)
#  else
#    define MODIO_DLL __declspec(dllimport)
#  endif
#else
#  define MODIO_DLL
#endif

extern "C"
{
  void MODIO_DLL modioGetMods(ModioFilter* filter, void (*callback)(ModioResponse* response, ModioMod* mods, int mods_size));
  void MODIO_DLL modioAddMod(ModioModHandler* mod_handler, void (*callback)(ModioResponse* response, ModioMod* mod));
  void MODIO_DLL modioEditMod(ModioMod* mod, ModioModHandler* mod_handler, void (*callback)(ModioResponse* response, ModioMod* mod));
  void MODIO_DLL modioDeleteMod(ModioMod* mod, void (*callback)(ModioResponse* response, ModioMod* mod));
  void MODIO_DLL modioInstallMod(ModioMod *mod, char* destination_path, void (*callback)(ModioResponse* response, char* path));
}

#endif
