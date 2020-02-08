
cmake_minimum_required(VERSION 2.8.12)

project(TVS)

file(GLOB SOURCES src/*.cpp)
file(GLOB HEADERS src/*.h)

if (UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -D_GNU_SOURCE")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -D_GNU_SOURCE")
endif()

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

if (UNIX)
  target_link_libraries(${PROJECT_NAME} -lpthread)
endif()

