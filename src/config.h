
#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#include <stdint.h>
#include <string>

struct Config {
  struct {
    uint16_t width;
    uint16_t height;
  } resolution;
  std::string media_dir;
};

void loadConfig(Config &config);

#endif // CONFIG_H_INCLUDED
