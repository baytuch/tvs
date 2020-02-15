
#include <iostream>
#include <string.h>
#include "utils.h"
#include "sleep.h"
#include "draw.h"
#include "frame_processor.h"


FrameProcessor::FrameProcessor(const uint16_t &width,
                                 const uint16_t &height,
                                 const uint8_t &rate) : m_width(width),
                                                        m_height(height),
                                                        m_area_size(width * height * 3),
                                                        m_rate(rate),
                                                        m_frame_area(0) {
  m_frame_area = mem_alloc(m_area_size);
  pthread_create(&this->loop_tid, NULL, FrameProcessor::init_loop, this);
}

FrameProcessor::~FrameProcessor() {
  mem_free(m_frame_area);
}

void FrameProcessor::loop() {
  while(true) {
    renderFrame();
    pushFrameToPipe();
    Sleep(30);
  }
}

void *FrameProcessor::init_loop(void *vptr_args){
  ((FrameProcessor *)vptr_args)->loop();
  return NULL;
}

void FrameProcessor::renderFrame() {
  drawClear(m_width, m_height, m_frame_area);
  //drawGrid(m_width, m_height, m_frame_area);
  drawText(m_width, m_height, 10, 10, m_frame_area, "hello world!");
}

void FrameProcessor::pushFrameToPipe() {
  char* s = reinterpret_cast<char*>(m_frame_area + (m_width * m_height));   // Get start of G plane
  std::cout.write(s, m_width * m_height);                                   // Output it
  s = reinterpret_cast<char*>(m_frame_area + 2 * (m_width * m_height));     // Get start of B plane
  std::cout.write(s, m_width * m_height);                                   // Output it
  s = reinterpret_cast<char*>(m_frame_area);                                // Get start of R plane
  std::cout.write(s, m_width * m_height);
}

