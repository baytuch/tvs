
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "sleep.h"
#include "draw.h"
#include "frame_processor.h"


const uint32_t FrameProcessor::loop_buffer_size = 25;
const char *FrameProcessor::server_host = "127.0.0.1";
const uint16_t FrameProcessor::server_port = 35001;

FrameProcessor::FrameProcessor(const uint16_t &width,
                                 const uint16_t &height,
                                 const uint8_t &rate) : m_width(width),
                                                        m_height(height),
                                                        m_area_size(width * height * 3),
                                                        m_rate(rate),
                                                        m_buffer_overflow(false),
                                                        m_buffer_pull_lock(false),
                                                        m_buffer_push_n(0),
                                                        m_buffer_pull_n(0),
                                                        m_fill(0.0),
                                                        m_frame_area(0),
                                                        m_loop_buffer(0),
                                                        m_pull_buffer(0),
                                                        m_server_buffer(0) {
  m_frame_area = mem_alloc(m_area_size);
  m_loop_buffer = mem_alloc(m_area_size * FrameProcessor::loop_buffer_size);
  m_pull_buffer = mem_alloc(m_area_size);
  m_server_buffer = (char *) malloc((size_t) m_area_size);
  pthread_create(&m_loop_tid, NULL, FrameProcessor::init_loop, this);
  pthread_create(&m_server_tid, NULL, FrameProcessor::init_server, this);
}

FrameProcessor::~FrameProcessor() {
  mem_free(m_frame_area);
  mem_free(m_loop_buffer);
}

void FrameProcessor::loop() {
  while(true) {
    renderFrame();
    buffer_push(m_frame_area);
    Sleep(33);
  }
}

void *FrameProcessor::init_loop(void *vptr_args){
  ((FrameProcessor *)vptr_args)->loop();
  return NULL;
}

void *FrameProcessor::init_server(void *vptr_args) {
  ((FrameProcessor *) vptr_args)->server();
  return NULL;
}

void FrameProcessor::renderFrame() {
  drawClear(m_width, m_height, m_frame_area);
  //drawGrid(m_width, m_height, m_frame_area);
  drawText(m_width, m_height, 10, 10, m_frame_area, "hello world!");
}

void FrameProcessor::buffer_push(const uint8_t *data) {
  m_buffer_pull_lock = true;
  if ((m_buffer_pull_n > 0 && m_buffer_push_n + 1 == m_buffer_pull_n) ||
      (m_buffer_pull_n == 0 && m_buffer_push_n == FrameProcessor::loop_buffer_size - 1)) {
    m_buffer_overflow = true;
    //std::cout << "FrameProcessor: buffer overflow!" << std::endl;
  } else {
    memcpy(m_loop_buffer + m_area_size * m_buffer_push_n, (void *) data, (size_t) m_area_size);
    if (m_buffer_push_n >= FrameProcessor::loop_buffer_size - 1) {
      m_buffer_push_n = 0;
    } else {
      m_buffer_push_n++;
    }
    m_buffer_overflow = false;
  }
  m_buffer_pull_lock = false;
}

void FrameProcessor::buffer_pull(uint8_t *data) {
 waiter(m_buffer_pull_lock);
  if (m_buffer_pull_n != m_buffer_push_n) {
    memcpy(data, m_loop_buffer + m_area_size * m_buffer_pull_n, (size_t) m_area_size);
    if (m_buffer_pull_n >= FrameProcessor::loop_buffer_size - 1) {
      m_buffer_pull_n = 0;
    } else {
      m_buffer_pull_n++;
    }
  }
  float delta = static_cast<float>(m_buffer_push_n > m_buffer_pull_n ?
    m_buffer_push_n - m_buffer_pull_n :
    FrameProcessor::loop_buffer_size - m_buffer_pull_n + m_buffer_push_n - 1);
  m_fill = delta / static_cast<float>(FrameProcessor::loop_buffer_size) * 100.0;
}

void FrameProcessor::server() {
  int socketd = 0;
  int acceptd = 0;
  sockaddr_in serv_addr;
  memset(&serv_addr, 0x00, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr(FrameProcessor::server_host);
  serv_addr.sin_port = htons(FrameProcessor::server_port);
  socketd = socket(AF_INET, SOCK_STREAM, 0);
  bind(socketd, (const sockaddr*) &serv_addr, sizeof(serv_addr));
  listen(socketd, 10);
  while (true) {
    acceptd = accept(socketd, (sockaddr*) NULL, NULL);
    std::cout << "FrameProcessor: server start stream" << std::endl;
    while (true) {
      memset(m_pull_buffer, 0x00, m_area_size);
      memset(m_server_buffer, 0x00, m_area_size);
      buffer_pull(m_pull_buffer);
      const uint32_t frame_size = m_width * m_height;
      memcpy(m_server_buffer, reinterpret_cast<char*>(m_pull_buffer + frame_size), frame_size);
      memcpy(m_server_buffer + frame_size, reinterpret_cast<char*>(m_pull_buffer + frame_size), frame_size);
      memcpy(m_server_buffer + frame_size * 2, reinterpret_cast<char*>(m_pull_buffer + frame_size * 2), frame_size);
      bool end_stream_t = false;
      ssize_t write_size_total = 0;
      while (true) {
        ssize_t write_size = write(acceptd,
                                   m_server_buffer + write_size_total,
                                   m_area_size - write_size_total);
        if (write_size > 0) {
          write_size_total += write_size;
          if (write_size_total == m_area_size) break;
        } else {
          end_stream_t = true;
          break;
        }
      }   
      //if (m_fill < 50) Sleep(29);
      //else Sleep(31);
      Sleep(33);
      //std::cout << m_fill << std::endl; 
      if (end_stream_t) {
        std::cout << "FrameProcessor: server end stream" << std::endl;
        close(acceptd);
        break;
      }
    }
  }
}

