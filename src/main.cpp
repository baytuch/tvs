
#include <stdint.h>
#include <iostream>
#include <pthread.h>
#include <functional>
#include "sleep.h"
#include "utils.h"
#include "player_audio.h"
#include "queue_audio.h"
#include "server_audio.h"
#include "frame_processor.h"


const char *media_file = "/home/sasha/tmp/Bamboo.mp3";
const uint32_t sample_rate = 44100;
const uint16_t frame_width = 640;
const uint16_t frame_height = 360;
const uint8_t frame_rate = 24;

int main(int argc, char **argv) {

  AudioMeta meta;
  PlayerAudio pla;
  QueueAudio qa;
  ServerAudio sa;
  FrameProcessor fp(frame_width, frame_height, frame_rate);

  const uint32_t compute_buffer_size = qa.getSize();
  int16_t *compute_buffer = mem_alloc_audio(compute_buffer_size * 2);

  if (pla.play(media_file)) std::cout << "init ok" << std::endl;

  if (pla.getMeta(meta)) {
    std::cout << "title: " << meta.title << std::endl;
    std::cout << "artist: " << meta.artist << std::endl;
    std::cout << "album: " << meta.album << std::endl;
  }

  uint32_t const sample_period = 1000000000 / sample_rate + 2000;
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
  }

  return 0;
}

