
#ifndef FRAME_CONTROLLER_H_INCLUDED
#define FRAME_CONTROLLER_H_INCLUDED

#include <stdint.h>
#include <string>
#include <pthread.h>

class FrameProcessor {

  public:
    FrameProcessor(const uint16_t &width,
                    const uint16_t &height,
                    const uint8_t &rate);
    ~FrameProcessor();
    void setTitle(const std::string &title);
    void setDuration(const int32_t &duration);

  private:
    struct RenderData {
      std::string title;
      int32_t duration;
    };
    struct RenderBuffer {
      RenderData push;
      RenderData pull;
    };
    static const uint32_t frame_rate;
    static const uint32_t loop_buffer_size;
    static const char *server_host;
    static const uint16_t server_port;
    const uint16_t m_width;
    const uint16_t m_height;
    const uint32_t m_area_size;
    const uint8_t m_rate;
    bool m_buffer_overflow;
    bool m_buffer_pull_lock;
    bool m_render_buffer_pull_lock;
    uint32_t m_buffer_push_n;
    uint32_t m_buffer_pull_n;
    int64_t m_last_ml_sec;
    int64_t m_last_ml_nsec;
    int64_t m_last_sl_sec;
    int64_t m_last_sl_nsec;
    float m_fill;
    uint8_t *m_frame_area;
    uint8_t *m_loop_buffer;
    uint8_t *m_pull_buffer;
    char *m_server_buffer;
    RenderBuffer m_render_buffer;
    pthread_t m_loop_tid;
    pthread_t m_server_tid;
    void loop();
    static void *init_loop(void *vptr_args);
    void server();
    static void *init_server(void *vptr_args);
    void renderFrame();
    void buffer_push(const uint8_t *data);
    void buffer_pull(uint8_t *data);

};

#endif // FRAME_CONTROLLER_H_INCLUDED
