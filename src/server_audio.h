
#ifndef SERVER_AUDIO_H_INCLUDED
#define SERVER_AUDIO_H_INCLUDED

#include <stdint.h>
#include <pthread.h>

class ServerAudio {

  public:
    ServerAudio();
    ~ServerAudio();
    void rawPush(int16_t &pcm_r, int16_t &pcm_l);

  private:
    static const uint32_t loop_buffer_size;
    static const uint32_t sample_rate;
    static const uint32_t transmit_interval;
    static const char *server_host;
    static const uint16_t server_port;
    bool m_run;
    bool m_is_run;
    bool m_buffer_overflow;
    bool m_buffer_push_lock;
    float m_fill;
    uint32_t m_buffer_push_n;
    uint32_t m_buffer_pull_n;
    pthread_t m_loop_tid;
    pthread_t m_server_tid;
    int16_t *m_loop_buffer;
    void loop();
    static void *init_loop(void *vptr_args);
    void server();
    static void *init_server(void *vptr_args);
    void buffer_push(int16_t &pcm_r, int16_t &pcm_l);
    void buffer_pull(int16_t &pcm_r, int16_t &pcm_l);
};

#endif // SERVER_AUDIO_H_INCLUDED