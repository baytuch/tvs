
#include <iostream>
#include <string.h>
#include "utils.h"
#include "sleep.h"
#include "frame_controller.h"


FrameController::FrameController(const uint16_t &width,
                                 const uint16_t &height,
                                 const uint8_t &rate) : m_width(width),
                                                        m_height(height),
                                                        m_area_size(width * height * 3),
                                                        m_rate(rate),
                                                        m_frame_area(0) {
  m_frame_area = mem_alloc(m_area_size);
  pthread_create(&this->loop_tid, NULL, FrameController::init_loop, this);
  for (int c = 0; c < 3; c++) {
    for (int x = 0; x < m_width; x++) {
      for (int y = 0; y < m_height; y++) {
        if (x % 30 == 0 || y % 30 == 0) {
          memset((void *) m_frame_area + (int) (x + y * m_width + m_width * m_height * c), 0xFF, 1);
        }
      }
    }
  }
}

FrameController::~FrameController() {
  mem_free(m_frame_area);
}

void FrameController::loop() {
  while(true) {
    pushFrameToPipe();
    Sleep(30);
  }
}

void *FrameController::init_loop(void *vptr_args){
  ((FrameController *)vptr_args)->loop();
  return NULL;
}

void FrameController::pushFrameToPipe() {
  char* s = reinterpret_cast<char*>(m_frame_area + (m_width * m_height));   // Get start of G plane
  std::cout.write(s, m_width * m_height);                                   // Output it
  s = reinterpret_cast<char*>(m_frame_area + 2 * (m_width * m_height));     // Get start of B plane
  std::cout.write(s, m_width * m_height);                                   // Output it
  s = reinterpret_cast<char*>(m_frame_area);                                // Get start of R plane
  std::cout.write(s, m_width * m_height);
}
