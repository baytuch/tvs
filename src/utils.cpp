
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "sleep.h"
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

int16_t *mem_alloc_audio(const uint32_t size_area) {
  int16_t *area;
  const size_t size_area_bytes = size_area * sizeof(int16_t);
  area = (int16_t *) malloc(size_area_bytes);
  memset((void *) area, 0x00, size_area_bytes);
  return area;
}

void mem_clear_audio(int16_t *area, const uint32_t size_area) {
  const size_t size_area_bytes = size_area * sizeof(int16_t);
  memset((void *) area, 0x00, size_area_bytes);
}

void mem_free_audio(int16_t *area) {
  free ((void *) area);
}

void waiter(bool &flag){
  while (flag){
    Sleep(1);
  }
}

void get_time(uint64_t &realtime) {
  realtime = 0;
  struct timespec mt;
  if (clock_gettime (CLOCK_REALTIME, &mt) == 0) {
    realtime = mt.tv_nsec;
  } 
}
