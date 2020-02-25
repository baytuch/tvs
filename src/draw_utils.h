
#ifndef DRAW_UTILS_H_INCLUDED
#define DRAW_UTILS_H_INCLUDED

#include <stdint.h>

bool jpegToRaw(const char *img_path, uint8_t **img_area, uint32_t &img_width, uint32_t &img_height);
void interBi(const uint8_t *img_area_base, const uint32_t &img_width_base, const uint32_t &img_height_base,
             uint8_t *img_area_com, const uint32_t &img_width_com, const uint32_t &img_height_com);

#endif // DRAW_UTILS_H_INCLUDED
