#ifndef MODIO_MODIORATINGS_H
#define MODIO_MODIORATINGS_H

#include "dependencies/json/json.hpp"
#include "Utility.h"
;
using json = nlohmann::json;

extern "C"
{
  struct ModioRatings
  {
    int total;
    int positive;
    int negative;
    double weighted;
    int percentage;
    char* text;
  };

  void modioInitRatings(ModioRatings* ratings, json ratings_json);
  void modioFreeRatings(ModioRatings* ratings);
}

#endif