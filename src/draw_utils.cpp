
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
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

