
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <jpeglib.h>
#include "utils.h"
#include "draw_utils.h"


bool jpegToRaw(const char *img_path, uint8_t **img_area, uint32_t &img_width, uint32_t &img_height) {
  bool res = false;
  img_width = 0;
  img_height = 0;
  uint32_t img_area_size = 0;
  uint32_t img_row_area_size = 0;
  JSAMPROW img_rows_area[1];
  jpeg_decompress_struct cinfo;
  jpeg_error_mgr jerr;
  FILE *img_file = fopen(img_path, "rb");
  if (img_file != NULL) {
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, img_file);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);
    img_area_size = cinfo.output_width * cinfo.output_height * cinfo.num_components;
    img_row_area_size = cinfo.output_width * cinfo.num_components;
    img_width = cinfo.output_width;
    img_height = cinfo.output_height;
    if (cinfo.num_components == 3) {
      *img_area = mem_alloc(img_area_size);
      img_rows_area[0] = mem_alloc(img_row_area_size);
      while (cinfo.output_scanline < cinfo.image_height) {
        jpeg_read_scanlines(&cinfo, img_rows_area, 1);
        for (uint32_t pix_n = 0; pix_n < cinfo.image_width; pix_n++) {
          memset(*img_area + pix_n + cinfo.image_width * (cinfo.output_scanline - 1),
                 img_rows_area[0][pix_n * 3 + 1], 1);
          memset(*img_area + pix_n + cinfo.image_width * (cinfo.output_scanline - 1) + cinfo.image_width * cinfo.image_height,
                 img_rows_area[0][pix_n * 3 + 2], 1);
          memset(*img_area + pix_n + cinfo.image_width * (cinfo.output_scanline - 1) + cinfo.image_width * cinfo.image_height * 2,
                 img_rows_area[0][pix_n * 3], 1);
        }
      }
      mem_free(img_rows_area[0]);
      res = true;
    }
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(img_file);
  }
  return res;
}

void interBi(const uint8_t *img_area_base, const uint32_t &img_width_base, const uint32_t &img_height_base,
             uint8_t *img_area_com, const uint32_t &img_width_com, const uint32_t &img_height_com) {
  for (uint32_t com_y = 0; com_y < img_height_com; com_y++) {
    float tmp = 0.0;
    tmp = static_cast<float>(com_y) / static_cast<float>(img_height_com - 1) * static_cast<float>(img_height_base - 1);
    uint32_t base_c_y = static_cast<uint32_t>(floor(tmp));
    if (base_c_y >= img_height_base - 1) {
      base_c_y = img_height_base - 2;
    }
    const float u = tmp - static_cast<float>(base_c_y);
    for (uint32_t com_x = 0; com_x < img_width_com; com_x++) {
      tmp = static_cast<float>(com_x) / static_cast<float>(img_width_com - 1) * static_cast<float>(img_width_base - 1);
      uint32_t base_c_x = static_cast<uint32_t>(floor(tmp));
      if (base_c_x >= img_width_base - 1) {
        base_c_x = img_width_base - 2;
      }
      const float t = tmp - static_cast<float>(base_c_x);
      const float k_a = (1.0 - t) * (1.0 - u);
      const float k_b = t * (1.0 - u);
      const float k_c = t * u;
      const float k_d = (1.0 - t) * u;
      const uint8_t pix_a_r = img_area_base[base_c_x + base_c_y * img_width_base + img_width_base * img_height_base * 2];
      const uint8_t pix_a_g = img_area_base[base_c_x + base_c_y * img_width_base];
      const uint8_t pix_a_b = img_area_base[base_c_x + base_c_y * img_width_base + img_width_base * img_height_base];
      const uint8_t pix_b_r = img_area_base[base_c_x + 1 + base_c_y * img_width_base + img_width_base * img_height_base * 2];
      const uint8_t pix_b_g = img_area_base[base_c_x + 1 + base_c_y * img_width_base];
      const uint8_t pix_b_b = img_area_base[base_c_x + 1 + base_c_y * img_width_base + img_width_base * img_height_base];
      const uint8_t pix_c_r = img_area_base[base_c_x + 1 + (base_c_y + 1) * img_width_base + img_width_base * img_height_base * 2];
      const uint8_t pix_c_g = img_area_base[base_c_x + 1 + (base_c_y + 1) * img_width_base];
      const uint8_t pix_c_b = img_area_base[base_c_x + 1 + (base_c_y + 1) * img_width_base + img_width_base * img_height_base];
      const uint8_t pix_d_r = img_area_base[base_c_x + (base_c_y + 1) * img_width_base + img_width_base * img_height_base * 2];
      const uint8_t pix_d_g = img_area_base[base_c_x + (base_c_y + 1) * img_width_base];
      const uint8_t pix_d_b = img_area_base[base_c_x + (base_c_y + 1) * img_width_base + img_width_base * img_height_base];
      const uint8_t com_pix_r = static_cast<uint8_t>(static_cast<float>(pix_a_r) * k_a) +
                                static_cast<uint8_t>(static_cast<float>(pix_b_r) * k_b) +
                                static_cast<uint8_t>(static_cast<float>(pix_c_r) * k_c) +
                                static_cast<uint8_t>(static_cast<float>(pix_d_r) * k_d);
      const uint8_t com_pix_g = static_cast<uint8_t>(static_cast<float>(pix_a_g) * k_a) +
                                static_cast<uint8_t>(static_cast<float>(pix_b_g) * k_b) +
                                static_cast<uint8_t>(static_cast<float>(pix_c_g) * k_c) +
                                static_cast<uint8_t>(static_cast<float>(pix_d_g) * k_d);
      const uint8_t com_pix_b = static_cast<uint8_t>(static_cast<float>(pix_a_b) * k_a) +
                                static_cast<uint8_t>(static_cast<float>(pix_b_b) * k_b) +
                                static_cast<uint8_t>(static_cast<float>(pix_c_b) * k_c) +
                                static_cast<uint8_t>(static_cast<float>(pix_d_b) * k_d);
      img_area_com[com_x + com_y * img_width_com + img_width_com * img_height_com * 2] = com_pix_r;
      img_area_com[com_x + com_y * img_width_com] = com_pix_g;
      img_area_com[com_x + com_y * img_width_com + img_width_com * img_height_com] = com_pix_b;
    }
  }
}

