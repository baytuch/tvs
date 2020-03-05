
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>
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

bool timeToString(const int32_t &time_num, std::string &time_str) {
  const int32_t hour_reriod = 60 * 60;
  const int32_t min_reriod = 60;
  const int32_t time_num_max = hour_reriod * 99 + min_reriod * 59 + 59;
  const char *time_str_err = "time incorrect";
  time_str.clear();
  bool res = false;
  int32_t time_num_abs = 0;
  int32_t hour_num = 0;
  int32_t min_num = 0;
  int32_t sec_num = 0;
  char hour_str[4] = {0x00, 0x00, 0x00, 0x00};
  char min_str[4] = {0x00, 0x00, 0x00, 0x00};
  char sec_str[3] = {0x00, 0x00, 0x00};
  if (time_num < 0) {
    time_num_abs = -time_num;
    time_str += '-';
  } else {
    time_num_abs = time_num;
  }
  if (time_num_abs <= time_num_max) {
    hour_num = static_cast<int32_t>(time_num_abs / hour_reriod);
    min_num = static_cast<int32_t>(time_num_abs % hour_reriod / min_reriod);
    sec_num = time_num_abs - hour_num * hour_reriod - min_num * min_reriod;
    sprintf(hour_str, "%d:", hour_num);
    sprintf(min_str, "%.2d:", min_num);
    sprintf(sec_str, "%.2d", sec_num);
    if (hour_num > 0) hour_num < 10 ? time_str.append(hour_str, 2) : time_str.append(hour_str, 3);
    time_str.append(min_str, 3);
    time_str.append(sec_str, 3);
    res = true;
  } else {
    time_str.clear();
    time_str = time_str_err;
  }
  return res;
}

bool deltaTime(int64_t &last_sec, int64_t &last_nsec, int64_t &delta) {
  delta = 0;
  bool res = false;
  struct timespec mt = {0, 0};
  int64_t now_sec = 0;
  int64_t now_nsec = 0;
  if (clock_gettime (CLOCK_REALTIME, &mt) == 0) {
    now_sec = static_cast<int64_t>(mt.tv_sec);
    now_nsec = static_cast<int64_t>(mt.tv_nsec);
    if (last_sec != 0) {
      delta = (now_sec - last_sec) * 1000000000 + now_nsec - last_nsec;
    }
    last_sec = now_sec;
    last_nsec = now_nsec;
    res = true;
  }
  return res;
}

bool getMediaList(const char *path, const char *ext, std::vector<std::string> &media_list) {
  const char sub = '.';
  bool res = false;
  media_list.clear();
  DIR *dir;
  struct dirent *de;
  dir = opendir(path);
  if (dir != NULL) {
    while(true) {
      de = readdir(dir);
      if (de != NULL) {
        if (de->d_type == DT_REG) {
          std::string file_name(de->d_name);
          const size_t pos = file_name.find_last_of(sub);
          if (pos != std::string::npos && pos == file_name.size() - 4) {
            if (file_name.substr(pos + 1, 3) == ext) {
              media_list.push_back(std::string(path) + '/' + file_name);
            }
          }
        }
      } else {
        break;
      }
    }
    closedir(dir);
  }
  if (media_list.size() > 0) res = true;
  return res;
}

