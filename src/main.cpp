
#include <stdint.h>
#include <iostream>
#include <pthread.h>
#include <functional>
#include <string>
#include "sleep.h"
#include "utils.h"
#include "draw_utils.h"
#include "player_audio.h"
#include "queue_audio.h"
#include "server_audio.h"
#include "frame_processor.h"
#include "config.h"


const char *media_file = "/home/sasha/tmp/Cyberpunk2077.mp3";
const char *img_bg_file = "/home/sasha/tmp/background_small.jpg";
//const char *img_bg_file = "/home/sasha/tmp/background.jpeg";
//const char *img_bg_file = "/home/sasha/tmp/background_extra_small.jpg";
const uint32_t sample_rate = 44100;
const uint8_t frame_rate = 24;

int main(int argc, char **argv) {

  Config config;
  loadConfig(config);

  AudioMeta meta;
  PlayerAudio pla;
  QueueAudio qa;
  ServerAudio sa;
  FrameProcessor fp(config.resolution.width,
                    config.resolution.height,
                    frame_rate,
                    img_bg_file);

  const uint32_t compute_buffer_size = qa.getSize();
  int16_t *compute_buffer = mem_alloc_audio(compute_buffer_size * 2);

  if (pla.play(media_file)) std::cout << "init ok" << std::endl;

  if (pla.getMeta(meta)) {
    std::cout << "title: " << meta.title << std::endl;
    std::cout << "artist: " << meta.artist << std::endl;
    std::cout << "album: " << meta.album << std::endl;
  }

  uint32_t const sample_period = 1000000000 / sample_rate + 2000;
  int main_loop_n = 0;
  while(true) {
    int16_t pcm_r = 0;
    int16_t pcm_l = 0;
    int16_t pcm_r_q = 0;
    int16_t pcm_l_q = 0;
    float fill = 0.0;
    for (int n = 0; n < 1000; n++) {
      pla.getPCM(pcm_r, pcm_l, fill);
      qa.compute(pcm_r, pcm_l, pcm_r_q, pcm_l_q);
      sa.rawPush(pcm_r_q, pcm_l_q);
    }
    if (fill > 50) usleep(sample_period - 5000);
    else usleep(sample_period - 1);
    qa.getData(compute_buffer);
    int16_t pcm_r_peek = 0;
    for (uint32_t i = 0; i < compute_buffer_size; i++) {
      int16_t pcm_r_d = compute_buffer[i * 2] < 0 ?
                        -compute_buffer[i * 2] :
                        compute_buffer[i * 2];
      if (pcm_r_d > pcm_r_peek) pcm_r_peek = pcm_r_d; 
    }
    pcm_r_peek /= 256;
    if (pcm_r_peek > 100) pcm_r_peek = 100;
    //for (int16_t m = 0; m < pcm_r_peek; m++) std::cout << "#";
    //std::cout << std::endl;
    //std::cout << fill << std::endl;
    std::string title;
    if (pla.getMeta(meta)) {
      for (const auto &s : meta.artist) {
        if (s != 0x00) title += s;
      }
      title += " - ";
      for (const auto &s : meta.title) {
        if (s != 0x00) title += s;
      }
      //title += "";
      fp.setTitle(title);
    }
    double current = 0.0;
    double total = 0.0;
    pla.getTime(current, total);
    int time_num = static_cast<int32_t>(total - current);
    std::string time_str;
    timeToString(time_num, time_str); 
    //std::cout << time_str << std::endl;
    title += " [";
    for (const auto &s : time_str) {
      if (s != 0x00) title += s;
    }
    if (main_loop_n > 30) {
      title += "]|";
    } else {
      title += "]";
    }
    main_loop_n++;
    if (main_loop_n > 60) main_loop_n = 0;    
    fp.setTitle(title);
  }

  return 0;
}

