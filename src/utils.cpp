
#include <stdlib.h>
#include <string.h>
#include "utils.h"


uint8_t *mem_alloc(const uint32_t size_area) {
  uint8_t *area;
  area = (uint8_t *) malloc((size_t) size_area);
  memset((void *) area, 0x00, (size_t) size_area);
  return area;
}

void mem_clear(uint8_t *area, const uint32_t size_area) {
  memset((void *) area, 0x00, (size_t) size_area);
}

void mem_free(uint8_t *area) {
  free ((void *) area);
}
