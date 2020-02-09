
cmake_minimum_required(VERSION 2.8.12)

if (UNIX)
  set(_LAME_LIB_NAME_STATIC "libmp3lame.a")
  set(_LAME_LIB_NAME_SHARED "libmp3lame.so")
else()
  set(_LAME_LIB_NAME "libmp3lame.lib")
endif()

if (UNIX AND STATIC)
  set(_LAME_LIB_NAME ${_LAME_LIB_NAME_STATIC})
elseif (UNIX AND NOT STATIC)
  set(_LAME_LIB_NAME ${_LAME_LIB_NAME_SHARED})
endif()

if (LAME_INCLUDE_DIRS AND LAME_LIBRARY_DIRS)
  find_library(LAME_LIBRARIES ${_LAME_LIB_NAME} ${LAME_LIBRARY_DIRS} NO_DEFAULT_PATH)
  find_path(LAME_INCLUDE_DIR "lame/lame.h" ${LAME_INCLUDE_DIRS} NO_DEFAULT_PATH)
else()
  find_library(LAME_LIBRARIES ${_LAME_LIB_NAME})
  find_path(LAME_INCLUDE_DIR "lame/lame.h")
endif()

if (LAME_INCLUDE_DIR AND LAME_LIBRARIES)
  set(SECURE_FOUND TRUE)
  message(STATUS "Find liblame installed: ${LAME_LIBRARIES}")
else()
  set(SECURE_FOUND FALSE)
  message(STATUS "Can`t find liblame installed")
endif()

