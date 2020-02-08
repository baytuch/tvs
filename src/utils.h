
#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <stdint.h>

uint8_t *mem_alloc(const uint32_t size_area);
void mem_clear(uint8_t *area, const uint32_t size_area);
void mem_free(uint8_t *area);

#endif // UTILS_H_INCLUDED
