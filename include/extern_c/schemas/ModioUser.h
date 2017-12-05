#ifndef MODIO_MODIOUSER_H
#define MODIO_MODIOUSER_H

#include "dependencies/json/json.hpp"
#include "Utility.h"
#include "extern_c/schemas/ModioImage.h"

using json = nlohmann::json;

extern "C"
{
  struct ModioUser
  {
    u32 id;
    long date_online;
    char* username;
    char* name_id;
    char* timezone;
    char* language;
    char* profile_url;
    ModioImage avatar;
  };

  void modioInitUser(ModioUser* user, json user_json);
  void modioFreeUser(ModioUser* user);
}

#endif