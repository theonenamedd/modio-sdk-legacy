#include "c/schemas/ModioGame.h"
#include "Utility.h"                      // for hasKey
#include "c/ModioC.h"                     // for ModioDownload ...
#include "c/schemas/ModioUser.h"          // for modioInitUser ...
#include "c/schemas/ModioIcon.h"          // for modioInitIcon ...
#include "c/schemas/ModioLogo.h"          // for modioInitLogo ...
#include "c/schemas/ModioHeader.h"        // for modioInitHeader ...
#include "c/schemas/ModioGameTagOption.h" // for modioInitGameTagOption ...

extern "C"
{
  void modioInitGame(ModioGame* game, nlohmann::json game_json)
  {
    game->id = 0;
    if(modio::hasKey(game_json, "id"))
      game->id = game_json["id"];

    game->status = 0;
    if(modio::hasKey(game_json, "status"))
      game->status = game_json["status"];

    game->maturity_options = 0;
    if(modio::hasKey(game_json, "maturity_options"))
      game->maturity_options = game_json["maturity_options"];

    game->date_added = 0;
    if(modio::hasKey(game_json, "date_added"))
      game->date_added = game_json["date_added"];

    game->date_updated = 0;
    if(modio::hasKey(game_json, "date_updated"))
      game->date_updated = game_json["date_updated"];

    game->presentation_option = 0;
    if(modio::hasKey(game_json, "presentation_option"))
      game->presentation_option = game_json["presentation_option"];

    game->date_live = 0;
    if(modio::hasKey(game_json, "date_live"))
      game->date_live = game_json["date_live"];

    game->community_options = 0;
    if(modio::hasKey(game_json, "community_options"))
      game->community_options = game_json["community_options"];

    game->submission_option = 0;
    if(modio::hasKey(game_json, "submission_option"))
      game->submission_option = game_json["submission_option"];

    game->curation_option = 0;
    if(modio::hasKey(game_json, "curation_option"))
      game->curation_option = game_json["curation_option"];

    game->revenue_options = 0;
    if(modio::hasKey(game_json, "revenue_options"))
      game->revenue_options = game_json["revenue_options"];

    game->api_access_options = 0;
    if(modio::hasKey(game_json, "api_access_options"))
      game->api_access_options = game_json["api_access_options"];

    game->ugc_name = NULL;
    if(modio::hasKey(game_json, "ugc_name"))
    {
      std::string ugc_name_str = game_json["ugc_name"];
      game->ugc_name = new char[ugc_name_str.size() + 1];
      strcpy(game->ugc_name, ugc_name_str.c_str());
    }

    game->instructions_url = NULL;
    if(modio::hasKey(game_json, "instructions_url"))
    {
      std::string instructions_url_str = game_json["instructions_url"];
      game->instructions_url = new char[instructions_url_str.size() + 1];
      strcpy(game->instructions_url, instructions_url_str.c_str());
    }

    game->name = NULL;
    if(modio::hasKey(game_json, "name"))
    {
      std::string name_str = game_json["name"];
      game->name = new char[name_str.size() + 1];
      strcpy(game->name, name_str.c_str());
    }

    game->name_id = NULL;
    if(modio::hasKey(game_json, "name_id"))
    {
      std::string name_id_str = game_json["name_id"];
      game->name_id = new char[name_id_str.size() + 1];
      strcpy(game->name_id, name_id_str.c_str());
    }

    game->summary = NULL;
    if(modio::hasKey(game_json, "summary"))
    {
      std::string summary_str = game_json["summary"];
      game->summary = new char[summary_str.size() + 1];
      strcpy(game->summary, summary_str.c_str());
    }

    game->instructions = NULL;
    if(modio::hasKey(game_json, "instructions"))
    {
      std::string instructions_str = game_json["instructions"];
      game->instructions = new char[instructions_str.size() + 1];
      strcpy(game->instructions, instructions_str.c_str());
    }

    game->profile_url = NULL;
    if(modio::hasKey(game_json, "profile_url"))
    {
      std::string profile_url_str = game_json["profile_url"];
      game->profile_url = new char[profile_url_str.size() + 1];
      strcpy(game->profile_url, profile_url_str.c_str());
    }

    nlohmann::json submitted_by_json;
    if(modio::hasKey(game_json, "submitted_by"))
      submitted_by_json = game_json["submitted_by"];
    modioInitUser(&(game->submitted_by), submitted_by_json);

    nlohmann::json icon_json;
    if(modio::hasKey(game_json, "icon"))
      icon_json = game_json["icon"];
    modioInitIcon(&(game->icon), icon_json);

    nlohmann::json logo_json;
    if(modio::hasKey(game_json, "logo"))
      logo_json = game_json["logo"];
    modioInitLogo(&(game->logo), logo_json);

    nlohmann::json header_json;
    if(modio::hasKey(game_json, "header"))
      header_json = game_json["header"];
    modioInitHeader(&(game->header), header_json);

    game->game_tag_option_array = NULL;
    game->game_tag_option_array_size = 0;
    if(modio::hasKey(game_json, "tag_options"))
    {
      game->game_tag_option_array_size = (u32)game_json["tag_options"].size();
      game->game_tag_option_array = new ModioGameTagOption[game->game_tag_option_array_size];

      for(u32 i=0; i<game->game_tag_option_array_size; i++)
      {
        modioInitGameTagOption(&(game->game_tag_option_array[i]), game_json["tag_options"][i]);
      }
    }
  }

  void modioInitGameCpp(ModioGame* modio_game, modio::Game* game)
  {
    modio_game->id = game->id;
    modio_game->status = game->status;
    modio_game->maturity_options = game->maturity_options;
    modio_game->date_added = game->date_added;
    modio_game->date_updated = game->date_updated;
    modio_game->presentation_option = game->presentation_option;
    modio_game->date_live = game->date_live;
    modio_game->community_options = game->community_options;
    modio_game->submission_option = game->submission_option;
    modio_game->curation_option = game->curation_option;
    modio_game->revenue_options = game->revenue_options;
    modio_game->api_access_options = game->api_access_options;

    modio_game->ugc_name = new char[game->ugc_name.size() + 1];
    strcpy(modio_game->ugc_name, game->ugc_name.c_str());

    modio_game->instructions_url = new char[game->instructions_url.size() + 1];
    strcpy(modio_game->instructions_url, game->instructions_url.c_str());

    modio_game->name = new char[game->name.size() + 1];
    strcpy(modio_game->name, game->name.c_str());

    modio_game->name_id = new char[game->name_id.size() + 1];
    strcpy(modio_game->name_id, game->name_id.c_str());

    modio_game->summary = new char[game->summary.size() + 1];
    strcpy(modio_game->summary, game->summary.c_str());

    modio_game->instructions = new char[game->instructions.size() + 1];
    strcpy(modio_game->instructions, game->instructions.c_str());
    
    modio_game->profile_url = new char[game->profile_url.size() + 1];
    strcpy(modio_game->profile_url, game->profile_url.c_str());

    modioInitUserCpp(&(modio_game->submitted_by), &(game->submitted_by));
    modioInitIconCpp(&(modio_game->icon), &(game->icon));
    modioInitLogoCpp(&(modio_game->logo), &(game->logo));
    modioInitHeaderCpp(&(modio_game->header), &(game->header));
  }

  void modioFreeGame(ModioGame* game)
  {
    if(game)
    {
      if(game->ugc_name)
        delete[] game->ugc_name;

      if(game->instructions_url)
        delete[] game->instructions_url;

      if(game->name)
        delete[] game->name;

      if(game->name_id)
        delete[] game->name_id;

      if(game->summary)
        delete[] game->summary;

      if(game->instructions)
        delete[] game->instructions;

      if(game->profile_url)
        delete[] game->profile_url;

      modioFreeUser(&(game->submitted_by));
      modioFreeIcon(&(game->icon));
      modioFreeLogo(&(game->logo));
      modioFreeHeader(&(game->header));

      for(u32 i=0; i<game->game_tag_option_array_size; i++)
      {
        modioFreeGameTagOption(&(game->game_tag_option_array[i]));
      }

      if(game->game_tag_option_array)
      {
        delete[] game->game_tag_option_array;
      }
    }
  }
}
