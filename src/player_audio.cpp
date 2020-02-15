
#include <iostream>
#include "utils.h"
#include "sleep.h"
#include "player_audio.h"


void saveID3v2(const mpg123_string *data, std::string &data_str) {
  data_str.clear();
  if (data != NULL) {
    if (data->fill > 0) {
      data_str = std::string(data->p, data->fill / sizeof(char));
    }
  }
}

const uint32_t PlayerAudio::loop_buffer_size = 44100;
const uint32_t PlayerAudio::sample_rate = 44100;
const uint32_t PlayerAudio::decode_interval = 100;

PlayerAudio::PlayerAudio() : m_run(false),
                             m_is_run(false),
                             m_buffer_overflow(false),  
                             m_buffer_push_lock(false),
                             m_decode_is_run(false),
                             m_decode_start_t(false),
                             m_channels(0),
                             m_encoding(0),
                             m_rate(0),
                             m_time_total(0.0),
                             m_time_current(0.0),                             
                             m_pcm_bytes(0),
                             m_pcm_pos(0),
                             m_buffer_push_n(0),
                             m_buffer_pull_n(0),
                             m_last_decode(0),
                             m_frame_index(0),
                             m_frame_length(0),
                             m_media_file(""),
                             m_meta({"", "", ""}) {
  m_loop_buffer = mem_alloc_audio(PlayerAudio::loop_buffer_size * 2);
  pthread_create(&this->m_loop_tid, NULL, PlayerAudio::init_loop, this);
}

PlayerAudio::~PlayerAudio() {
  mem_free_audio(m_loop_buffer);
  mem_free(m_pcm_buffer);
}

bool PlayerAudio::play(const char *path) {
  m_media_file = path;
  m_decode_start_t = true;
  waiter(m_decode_start_t);
  return m_decode_is_run;
}

void PlayerAudio::loop() {
  m_is_run = true;
  m_run = true;
  decode_init();
  while(m_run) {
    decode_controller();
    Sleep(PlayerAudio::decode_interval);
  }
  m_is_run = false;
}

void *PlayerAudio::init_loop(void *vptr_args) {
  ((PlayerAudio *) vptr_args)->loop();
  return NULL;
}

void PlayerAudio::decode_init() {
  mpg123_init();
  m_mpg123 = mpg123_new(NULL, NULL);
  m_pcm_buffer = mem_alloc(mpg123_outblock(m_mpg123));
}

void PlayerAudio::decode_start() {
  m_frame_length = 0;
  mpg123_id3v1 *id3v1_data;
  mpg123_id3v2 *id3v2_data;
  bool decode_is_run = false;
  m_meta = {"", "", ""};
  m_time_total = 0.0;
  m_time_current = 0.0;
  if (mpg123_open(m_mpg123, m_media_file.c_str()) == MPG123_OK) {
    if (mpg123_getformat(m_mpg123, &m_rate, &m_channels, &m_encoding) == MPG123_OK) {
      decode_is_run = true;
    }
    off_t framelength = mpg123_framelength(m_mpg123);
    if (framelength > 0 && framelength != MPG123_ERR) m_frame_length = framelength;
    mpg123_scan(m_mpg123);
    int meta_status = mpg123_meta_check(m_mpg123);
    if (meta_status & MPG123_ID3) {
      if (mpg123_id3(m_mpg123, &id3v1_data, &id3v2_data) == MPG123_OK) {
        if (id3v1_data != NULL) {
          m_meta.title = std::string(id3v1_data->title,
                                     sizeof(id3v1_data->title) / sizeof(char));
          m_meta.artist = std::string(id3v1_data->artist,
                                     sizeof(id3v1_data->artist) / sizeof(char));
          m_meta.album = std::string(id3v1_data->album,
                                     sizeof(id3v1_data->album) / sizeof(char));
        }
        if (id3v2_data != NULL) {
          saveID3v2(id3v2_data->title, m_meta.title);
          saveID3v2(id3v2_data->artist, m_meta.artist);
          saveID3v2(id3v2_data->album, m_meta.album);
        }
      }
    }
    double tpf = mpg123_tpf(m_mpg123);
    if (tpf > 0 && m_frame_length > 0)
      m_time_total = tpf * static_cast<double>(m_frame_length);
  }
  if (m_decode_is_run != decode_is_run) m_decode_is_run = decode_is_run;
  m_decode_start_t = false;
}

void PlayerAudio::decode_worker() {
  m_pcm_pos = 0;
  if (mpg123_decode_frame(m_mpg123, &m_frame_index, &m_pcm_buffer, &m_pcm_bytes) != MPG123_OK) {
    std::cout << "error" << std::endl;
    m_decode_is_run = false;
  }
}

void PlayerAudio::decode_controller() {
  const uint64_t sample_period = 1000000000 / PlayerAudio::sample_rate;
  uint64_t current_time = 0;
  get_time(current_time);
  if (current_time > 0) {
    if (m_last_decode == 0) {
      m_last_decode = current_time;
    } else {
      uint64_t delta_time = current_time > m_last_decode ?
                            current_time - m_last_decode :
                            (current_time + 1000000000) - m_last_decode;
      const uint64_t need_samples = delta_time / sample_period;
      for (uint64_t i = 0; i < need_samples; i++) {
        int16_t pcm_r = 0;
        int16_t pcm_l = 0;
        if (m_decode_is_run) {
          if (m_pcm_pos == m_pcm_bytes || m_pcm_bytes == 0) decode_worker();
          pcm_r = m_pcm_buffer[m_pcm_pos] + m_pcm_buffer[m_pcm_pos + 1] * 256;
          pcm_l = m_pcm_buffer[m_pcm_pos + 2] + m_pcm_buffer[m_pcm_pos + 3] * 256;
          m_pcm_pos += 4;
        }  
        buffer_push(pcm_r, pcm_l);
      }
      m_last_decode = current_time;
      if (m_frame_length > 0) {
        m_time_current = static_cast<double>(m_frame_index) *
                         m_time_total / static_cast<double>(m_frame_length);
      }
    }
  }
  if (m_decode_start_t) decode_start();
  if (!m_decode_is_run) {
    m_meta = {"", "", ""};
    m_time_total = 0.0;
    m_time_current = 0.0;
  }
}

void PlayerAudio::buffer_push(int16_t &pcm_r, int16_t &pcm_l) {
  waiter(m_buffer_push_lock);
  if ((m_buffer_pull_n > 0 && m_buffer_push_n + 1 == m_buffer_pull_n) ||
      (m_buffer_pull_n == 0 && m_buffer_push_n == PlayerAudio::loop_buffer_size - 1)) {
    m_buffer_overflow = true;
    std::cout << "PlayerAudio: buffer overflow!" << std::endl;
  } else {
    m_loop_buffer[m_buffer_push_n * 2] = pcm_r;
    m_loop_buffer[m_buffer_push_n * 2 + 1] = pcm_l;
    if (m_buffer_push_n >= PlayerAudio::loop_buffer_size - 1){
      m_buffer_push_n = 0;
    } else {
      m_buffer_push_n++;
    }
    m_buffer_overflow = false;
  }
}

void PlayerAudio::buffer_pull(int16_t &pcm_r, int16_t &pcm_l) {
  m_buffer_push_lock = true;
  pcm_r = 0;
  pcm_l = 0;
  if (m_buffer_pull_n != m_buffer_push_n) {
    pcm_r = m_loop_buffer[m_buffer_pull_n * 2];
    pcm_l = m_loop_buffer[m_buffer_pull_n * 2 + 1];
    if (m_buffer_pull_n >= PlayerAudio::loop_buffer_size - 1) {
      m_buffer_pull_n = 0;
    } else {
      m_buffer_pull_n++;
    }
  }
  m_buffer_push_lock = false;
}

void PlayerAudio::getPCM(int16_t &pcm_r, int16_t &pcm_l, float &fill) {
  buffer_pull(pcm_r, pcm_l);
  float delta = static_cast<float>(m_buffer_push_n > m_buffer_pull_n ?
    m_buffer_push_n - m_buffer_pull_n :
    PlayerAudio::loop_buffer_size - m_buffer_pull_n + m_buffer_push_n - 1);
  fill = delta / static_cast<float>(PlayerAudio::loop_buffer_size) * 100.0;
}
bool PlayerAudio::getMeta(AudioMeta &meta) {
  bool res = false;
  if (!m_meta.title.empty() ||
      !m_meta.artist.empty() ||
      !m_meta.album.empty()) res = true;
  meta = m_meta;
  return res;
}

bool PlayerAudio::getTime(double &current, double &total) {
  bool res = false;
  current = 0.0;
  total = 0.0;
  if (m_decode_is_run && m_time_total > 0) {
    current = m_time_current;
    total = m_time_total;
  }
  return res;
}
