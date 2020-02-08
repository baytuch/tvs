
#include <stdint.h>
#include <iostream>
#include "sleep.h"
#include "frame_controller.h"


const uint16_t frame_width = 640;
const uint16_t frame_height = 360;
const uint8_t frame_rate = 24;

int main() {
  FrameController fc(frame_width, frame_height, frame_rate);
  //std::cout << "Hello world!" << std::endl;
  Sleep(180000);
  return 0;
}
