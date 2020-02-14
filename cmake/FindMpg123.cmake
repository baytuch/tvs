
cmake_minimum_required(VERSION 2.8.12)

if (UNIX)
  set(_MPG123_LIB_NAME_STATIC "libmpg123.a")
  set(_MPG123_LIB_NAME_SHARED "libmpg123.so")
else()
  set(_MPG123E_LIB_NAME "libmpg123.lib")
endif()

if (UNIX AND STATIC)
  set(_MPG123_LIB_NAME ${_MPG123_LIB_NAME_STATIC})
elseif (UNIX AND NOT STATIC)
  set(_MPG123_LIB_NAME ${_MPG123_LIB_NAME_SHARED})
endif()

if (MPG123_INCLUDE_DIRS AND MPG123_LIBRARY_DIRS)
  find_library(MPG123_LIBRARIES ${_MPG123_LIB_NAME} ${MPG123_LIBRARY_DIRS} NO_DEFAULT_PATH)
  find_path(MPG123_INCLUDE_DIR "mpg123.h" ${MPG123_INCLUDE_DIRS} NO_DEFAULT_PATH)
else()
  find_library(MPG123_LIBRARIES ${_MPG123_LIB_NAME})
  find_path(MPG123_INCLUDE_DIR "mpg123.h")
endif()

if (MPG123_INCLUDE_DIR AND MPG123_LIBRARIES)
  set(MPG123_FOUND TRUE)
  message(STATUS "Find libmpg123 installed: ${MPG123_LIBRARIES}")
else()
  set(MPG123_FOUND FALSE)
  message(STATUS "Can`t find libmpg123 installed")
endif()

