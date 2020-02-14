
#ifndef QUEUE_AUDIO_H_INCLUDED
#define QUEUE_AUDIO_H_INCLUDED

#include <stdint.h>

class QueueAudio {

  public:
    QueueAudio();
    ~QueueAudio();
    void compute(const int16_t &in_pcm_r, const int16_t &in_pcm_l,
                 int16_t &out_pcm_r, int16_t &out_pcm_l);
    uint32_t getSize() const;
    void getData(int16_t *data);

  private:
    static const uint32_t queue_size;
    bool m_pull_lock;
    uint32_t m_queue_n;
    int16_t *m_queue_buffer;
    void pp(const int16_t &in_pcm_r, const int16_t &in_pcm_l,
            int16_t &out_pcm_r, int16_t &out_pcm_l);
    void pull(int16_t *data);
};

#endif // QUEUE_AUDIO_H_INCLUDED