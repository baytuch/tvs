
#include <string.h>
#include "font.h"
#include "draw.h"


void drawClear(const uint16_t &area_width, const uint16_t &area_height,
               uint8_t *frame_area) {
  memset(frame_area, 0x00, area_width * area_height * 3);
}

void drawText(const uint16_t &area_width, const uint16_t &area_height,
              const uint16_t &x, const uint16_t &y,
              uint8_t *frame_area,
              const char *text) {
  const uint8_t font_data_len = getDataSize();
  const size_t text_len = strlen(text);
  if (text_len > 0) {
    for (int p = 0; p < text_len; p++) {
      uint8_t font_data[font_data_len];
      getCharData(text[p], font_data);
      for (int c = 0; c < 3; c++) {
        for (int i = 0; i < 13; i++) {
          for (int n = 0; n < 8; n++) {
            uint8_t pix = 0x00;
            if (font_data[12 - i] & 1 << (7 - n)) pix = 0xFF;
            memset(frame_area + (i * area_width + n + area_width * area_height * c + 8 * p + x + y * area_width), pix, 1);
          }
        }
      }
    }
  }
}

void drawGrid(const uint16_t &area_width, const uint16_t &area_height,
              uint8_t *frame_area) {
  for (int c = 0; c < 3; c++) {
    for (int x = 0; x < area_width; x++) {
      for (int y = 0; y < area_height; y++) {
        if (x % 30 == 0 || y % 30 == 0) {
          memset(frame_area + (x + y * area_width + area_width * area_height * c), 0xFF, 1);
        }
      }
    }
  }
}
