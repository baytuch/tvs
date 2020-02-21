
#ifndef DRAW_H_INCLUDED
#define DRAW_H_INCLUDED

#include <stdint.h>

void loadImg(const char *img_path, const uint16_t &area_width,
             const uint16_t &area_height, uint8_t *img_area);

void drawClear(const uint16_t &area_width, const uint16_t &area_height,
               uint8_t *frame_area);

void drawText(const uint16_t &area_width, const uint16_t &area_height,
              const uint16_t &x, const uint16_t &y,
              uint8_t *frame_area,
              const char *text);

void drawGrid(const uint16_t &area_width, const uint16_t &area_height,
              uint8_t *frame_area);

void drawBackground(const uint8_t *img_bg_area, uint8_t *frame_area,
                    const uint16_t &area_width, const uint16_t &area_height);



#endif // DRAW_H_INCLUDED
