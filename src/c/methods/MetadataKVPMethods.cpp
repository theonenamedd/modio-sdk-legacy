#include <map>                                          // for map
#include <string>                                       // for string, opera...
#include "Utility.h"        // for toString, get...
#include "wrappers/CurlWrapper.h"       // for getCallNumber
#include "c/ModioC.h"                        // for u32, ModioRes...
#include "Globals.h"       // for GAME_ID, MODI...
#include "ModioUtility.h"  // for GenericReques...
#include "c/methods/callbacks/MetadataKVPCallbacks.h"   // for GetAllMetadat...
#include "dependencies/nlohmann/json.hpp"               // for json

extern "C"
{
  void modioGetAllMetadataKVP(void* object, u32 mod_id, void (*callback)(void* object, ModioResponse response, ModioMetadataKVP* metadata_kvp_array, u32 metadata_kvp_array_size))
  {
    std::string url = modio::MODIO_URL + modio::MODIO_VERSION_PATH + "games/" + modio::toString(modio::GAME_ID) + "/mods/" + modio::toString(mod_id) + "/metadatakvp/";

    u32 call_number = modio::curlwrapper::getCallNumber();

    get_all_metadata_kvp_callbacks[call_number] = new GetAllMetadataKVPParams;
    get_all_metadata_kvp_callbacks[call_number]->callback = callback;
    get_all_metadata_kvp_callbacks[call_number]->object = object;

    modio::curlwrapper::get(call_number, url, modio::getHeaders(), &modioOnGetAllMetadataKVP);
  }

  void modioAddMetadataKVP(void* object, u32 mod_id, char const* const* metadata_kvp_array, u32 metadata_kvp_array_size, void (*callback)(void* object, ModioResponse response))
  {
    if(!modioIsLoggedIn())
    {
      modio::processGenericLocalUnauthorizedRequest(object, callback);
      return;
    }

    std::map<std::string, std::string> data;

    u32 call_number = modio::curlwrapper::getCallNumber();

    add_metadata_kvp_callbacks[call_number] = new GenericRequestParams;
    add_metadata_kvp_callbacks[call_number]->callback = callback;
    add_metadata_kvp_callbacks[call_number]->object = object;

    std::string url = modio::MODIO_URL + modio::MODIO_VERSION_PATH + "games/" + modio::toString(modio::GAME_ID) + "/mods/" + modio::toString(mod_id) + "/metadatakvp";

    for(u32 i=0; i<metadata_kvp_array_size; i++)
    {
      if(i==0)
        url += "?";
      else
        url += "&";
      url += std::string("metadata[]=") + metadata_kvp_array[i];
    }

    modio::curlwrapper::post(call_number, url, modio::getUrlEncodedHeaders(), data, &modioOnAddMetadataKVP);
  }

  void modioDeleteMetadataKVP(void* object, u32 mod_id, char const* const* metadata_kvp_array, u32 metadata_kvp_array_size, void (*callback)(void* object, ModioResponse response))
  {
    if(!modioIsLoggedIn())
    {
      modio::processGenericLocalUnauthorizedRequest(object, callback);
      return;
    }
    
    std::map<std::string, std::string> data;

    u32 call_number = modio::curlwrapper::getCallNumber();

    delete_metadata_kvp_callbacks[call_number] = new GenericRequestParams;
    delete_metadata_kvp_callbacks[call_number]->callback = callback;
    delete_metadata_kvp_callbacks[call_number]->object = object;

    std::string url = modio::MODIO_URL + modio::MODIO_VERSION_PATH + "games/" + modio::toString(modio::GAME_ID) + "/mods/" + modio::toString(mod_id) + "/metadatakvp";

    for(u32 i=0; i<metadata_kvp_array_size; i++)
    {
      if(i==0)
        url += "?";
      else
        url += "&";
      url += std::string("metadata[]=") + metadata_kvp_array[i];
    }

    modio::curlwrapper::deleteCall(call_number, url, modio::getUrlEncodedHeaders(), data, &modioOnDeleteMetadataKVP);
  }
}
