
cmake_minimum_required(VERSION 2.8.12)

list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_SOURCE_DIR}/cmake")

project(TVS)

set(STATIC ON)

file(GLOB SOURCES src/*.cpp)
file(GLOB HEADERS src/*.h)

if (UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -D_GNU_SOURCE")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -D_GNU_SOURCE")
endif()

find_package(Mpg123)

if (MPG123_FOUND)
  include_directories(${MPG123_INCLUDE_DIR})
endif()

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} ${MPG123_LIBRARIES})

if (UNIX)
  target_link_libraries(${PROJECT_NAME} -lpthread)
endif()

