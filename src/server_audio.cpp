
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include "utils.h"
#include "sleep.h"
#include "server_audio.h"


const uint32_t ServerAudio::loop_buffer_size = 1024 * 1024;
const uint32_t ServerAudio::sample_rate = 44100;
const uint32_t ServerAudio::transmit_interval = 100;
const char *ServerAudio::server_host = "127.0.0.1";
const uint16_t ServerAudio::server_port = 35000;

ServerAudio::ServerAudio(): m_run(false),
                            m_is_run(false),
                            m_buffer_overflow(false),
                            m_buffer_push_lock(false),
                            m_fill(0.0), 
                            m_buffer_push_n(0),
                            m_buffer_pull_n(0) {
  m_loop_buffer = mem_alloc_audio(ServerAudio::loop_buffer_size * 2);
  pthread_create(&m_loop_tid, NULL, ServerAudio::init_loop, this);
  pthread_create(&m_server_tid, NULL, ServerAudio::init_server, this);
}

ServerAudio::~ServerAudio() {
}

void ServerAudio::loop() {
  m_is_run = true;
  m_run = true;
  while(m_run) {
    Sleep(ServerAudio::transmit_interval);
  }
  m_is_run = false;
}

void *ServerAudio::init_loop(void *vptr_args) {
  ((ServerAudio *) vptr_args)->loop();
  return NULL;
}

void ServerAudio::server() {
  const ssize_t write_buffer_size = 256;
  int socketd = 0;
  int acceptd = 0;
  sockaddr_in serv_addr;
  memset(&serv_addr, 0x00, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr(ServerAudio::server_host);
  serv_addr.sin_port = htons(ServerAudio::server_port);
  socketd = socket(AF_INET, SOCK_STREAM, 0);
  bind(socketd, (const sockaddr*) &serv_addr, sizeof(serv_addr));
  listen(socketd, 10);
  while (true) {
    acceptd = accept(socketd, (sockaddr*) NULL, NULL);
    std::cout << "ServerAudio: start stream" << std::endl;
    while (true) {
      char buffer[write_buffer_size];
      memset(buffer, 0x00, sizeof(buffer));
      for (ssize_t n = 0; n < write_buffer_size; n += 4) {
        int16_t pcm_r = 0;
        int16_t pcm_l = 0;
        buffer_pull(pcm_r, pcm_l);
        memcpy(buffer + n, reinterpret_cast<char*>(&pcm_r), 2);
        memcpy(buffer + n + 2, reinterpret_cast<char*>(&pcm_l), 2);  
      }
      bool end_stream_t = false;
      ssize_t write_size_total = 0;
      while (true) {
        ssize_t write_size = write(acceptd,
                                   buffer + write_size_total,
                                   write_buffer_size - write_size_total);
        if (write_size > 0) {
          write_size_total += write_size;
          if (write_size_total == write_buffer_size) break;
        } else {
          end_stream_t = true;
          break;
        }
      }   
      if (m_fill < 50) usleep(1500);
      else usleep(1000); 
      if (end_stream_t) {
        std::cout << "ServerAudio: end stream" << std::endl;
        close(acceptd);
        break;
      }
    }
  }
}

void *ServerAudio::init_server(void *vptr_args) {
  ((ServerAudio *) vptr_args)->server();
  return NULL;
}

void ServerAudio::buffer_push(int16_t &pcm_r, int16_t &pcm_l) {
  waiter(m_buffer_push_lock);
  if ((m_buffer_pull_n > 0 && m_buffer_push_n + 1 == m_buffer_pull_n) ||
      (m_buffer_pull_n == 0 && m_buffer_push_n == ServerAudio::loop_buffer_size - 1)) {
    m_buffer_overflow = true;
    //std::cout << "ServerAudio: buffer overflow!" << std::endl;
  } else {
    m_loop_buffer[m_buffer_push_n * 2] = pcm_r;
    m_loop_buffer[m_buffer_push_n * 2 + 1] = pcm_l;
    if (m_buffer_push_n >= ServerAudio::loop_buffer_size - 1){
      m_buffer_push_n = 0;
    } else {
      m_buffer_push_n++;
    }
    m_buffer_overflow = false;
  }
}

void ServerAudio::buffer_pull(int16_t &pcm_r, int16_t &pcm_l) {
  m_buffer_push_lock = true;
  pcm_r = 0;
  pcm_l = 0;
  if (m_buffer_pull_n != m_buffer_push_n) {
    pcm_r = m_loop_buffer[m_buffer_pull_n * 2];
    pcm_l = m_loop_buffer[m_buffer_pull_n * 2 + 1];
    if (m_buffer_pull_n >= ServerAudio::loop_buffer_size - 1) {
      m_buffer_pull_n = 0;
    } else {
      m_buffer_pull_n++;
    }
  }
  float delta = static_cast<float>(m_buffer_push_n > m_buffer_pull_n ?
    m_buffer_push_n - m_buffer_pull_n :
    ServerAudio::loop_buffer_size - m_buffer_pull_n + m_buffer_push_n - 1);
  m_fill = delta / static_cast<float>(ServerAudio::loop_buffer_size) * 100.0;
  m_buffer_push_lock = false;
}

void ServerAudio::rawPush(int16_t &pcm_r, int16_t &pcm_l) {
  buffer_push(pcm_r, pcm_l);
}
