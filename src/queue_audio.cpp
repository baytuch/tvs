
#include <iostream>
#include "utils.h"
#include "queue_audio.h"


const uint32_t QueueAudio::queue_size = 3000;

QueueAudio::QueueAudio() : m_pull_lock(false),
                           m_queue_n(0) {
  m_queue_buffer = mem_alloc_audio(QueueAudio::queue_size * 2);
}

QueueAudio::~QueueAudio() {
  mem_free_audio(m_queue_buffer);
}

void QueueAudio::pp(const int16_t &in_pcm_r, const int16_t &in_pcm_l,
                    int16_t &out_pcm_r, int16_t &out_pcm_l) {
  m_pull_lock = true;
  out_pcm_r = m_queue_buffer[m_queue_n * 2];
  out_pcm_l = m_queue_buffer[m_queue_n * 2 + 1];
  m_queue_buffer[m_queue_n * 2] = in_pcm_r;
  m_queue_buffer[m_queue_n * 2 + 1] = in_pcm_l;
  if (m_queue_n >= QueueAudio::queue_size - 1){
    m_queue_n = 0;
  } else {
    m_queue_n++;
  }
  m_pull_lock = false;
}

void QueueAudio::pull(int16_t *data) {
  waiter(m_pull_lock);
  uint32_t pull_n = m_queue_n;
  for (uint32_t n = 0; n < QueueAudio::queue_size * 2; n++) {
    data[n] = m_queue_buffer[pull_n];
    if (pull_n >= QueueAudio::queue_size - 1) {
      pull_n = 0;
    } else {
      pull_n++;
    }
  }
}

void QueueAudio::compute(const int16_t &in_pcm_r, const int16_t &in_pcm_l,
                         int16_t &out_pcm_r, int16_t &out_pcm_l) {
  pp(in_pcm_r, in_pcm_l, out_pcm_r, out_pcm_l);
}

uint32_t QueueAudio::getSize() const {
  return QueueAudio::queue_size;
}

void QueueAudio::getData(int16_t *data) {
  pull(data);
}

