
#ifndef FRAME_CONTROLLER_H_INCLUDED
#define FRAME_CONTROLLER_H_INCLUDED

#include <stdint.h>
#include <pthread.h>

class FrameController {

  public:
    FrameController(const uint16_t &width,
                    const uint16_t &height,
                    const uint8_t &rate);
    ~FrameController();

  private:
    const uint16_t m_width;
    const uint16_t m_height;
    const uint32_t m_area_size;
    const uint8_t m_rate;
    uint8_t *m_frame_area;
    pthread_t loop_tid;
    void loop();
    static void *init_loop(void *vptr_args);
    void pushFrameToPipe();

};

#endif // FRAME_CONTROLLER_H_INCLUDED
