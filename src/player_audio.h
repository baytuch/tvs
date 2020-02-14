
#ifndef PLAYER_AUDIO_H_INCLUDED
#define PLAYER_AUDIO_H_INCLUDED

#include <stdint.h>
#include <pthread.h>
#include <string>
#include <mpg123.h>

struct AudioMeta {
  std::string title;
  std::string artist;
  std::string album;
};

class PlayerAudio {

  public:
    PlayerAudio();
    ~PlayerAudio();
    bool play(const char *path);
    void getPCM(int16_t &pcm_r, int16_t &pcm_l, float &fill);
    bool getMeta(AudioMeta &meta);
    bool getTime(double &current, double &total);

  private:
    static const uint32_t loop_buffer_size;
    static const uint32_t sample_rate;
    static const uint32_t decode_interval;
    bool m_run;
    bool m_is_run;
    bool m_buffer_overflow;
    bool m_buffer_push_lock;
    bool m_decode_is_run;
    bool m_decode_start_t;
    int m_channels;
    int m_encoding;
    long m_rate;
    double m_time_total;
    double m_time_current;
    size_t m_pcm_bytes;
    uint32_t m_pcm_pos;
    uint32_t m_buffer_push_n;
    uint32_t m_buffer_pull_n;
    uint64_t m_last_decode;
    off_t m_frame_index;
    off_t m_frame_length;
    std::string m_media_file;
    AudioMeta m_meta;
    int16_t *m_loop_buffer;
    uint8_t *m_pcm_buffer;
    pthread_t m_loop_tid;
    mpg123_handle *m_mpg123;
    void loop();
    static void *init_loop(void *vptr_args);
    void decode_init();
    void decode_start();
    void decode_worker();
    void decode_controller();
    void buffer_push(int16_t &pcm_r, int16_t &pcm_l);
    void buffer_pull(int16_t &pcm_r, int16_t &pcm_l);

};

#endif // PLAYER_AUDIO_H_INCLUDED
