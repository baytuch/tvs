
#ifndef DRAW_UTILS_H_INCLUDED
#define DRAW_UTILS_H_INCLUDED

#include <stdint.h>

bool jpegToRaw(const char *img_path, uint8_t **img_area, uint32_t &img_width, uint32_t &img_height);

#endif // DRAW_UTILS_H_INCLUDED
