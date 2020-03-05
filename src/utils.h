
#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <stdint.h>
#include <string>
#include <vector>

uint8_t *mem_alloc(const uint32_t size_area);
void mem_clear(uint8_t *area, const uint32_t size_area);
void mem_free(uint8_t *area);
int16_t *mem_alloc_audio(const uint32_t size_area);
void mem_clear_audio(int16_t *area, const uint32_t size_area);
void mem_free_audio(int16_t *area);
void waiter(bool &flag);
void get_time(uint64_t &realtime);
bool timeToString(const int32_t &time_num, std::string &time_str);
bool deltaTime(int64_t &last_sec, int64_t &last_nsec, int64_t &delta);
bool getMediaList(const char *path, const char *ext, std::vector<std::string> &media_list);

#endif // UTILS_H_INCLUDED
