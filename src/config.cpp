
#include "config.h"


const uint16_t resolution_width_default = 640;
const uint16_t resolution_height_default = 360;
const char *media_dir = "media_dir";

bool loadConfigFile() {
  bool res = false;
  return res;
}

void loadConfig(Config &config) {
  config.resolution.width = 0;
  config.resolution.height = 0;
  config.media_dir = "";

  config.resolution.width = resolution_width_default;
  config.resolution.height = resolution_height_default;
  config.media_dir = media_dir;
}
