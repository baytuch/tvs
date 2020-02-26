
cmake_minimum_required(VERSION 2.8.12)

if (UNIX)
  set(_JANSSON_LIB_NAME_STATIC "libjansson.a")
  set(_JANSSON_LIB_NAME_SHARED "libjansson.so")
else()
  set(_JANSSON_LIB_NAME "libjansson.lib")
endif()

if (UNIX AND STATIC)
  set(_JANSSON_LIB_NAME ${_JANSSON_LIB_NAME_STATIC})
elseif (UNIX AND NOT STATIC)
  set(_JANSSON_LIB_NAME ${_JANSSON_LIB_NAME_SHARED})
endif()

if (JANSSON_INCLUDE_DIRS AND JANSSON_LIBRARY_DIRS)
  find_library(JANSSON_LIBRARIES ${_JANSSON_LIB_NAME} ${JANSSON_LIBRARY_DIRS} NO_DEFAULT_PATH)
  find_path(JANSSON_INCLUDE_DIR "jpeglib.h" ${JANSSON_INCLUDE_DIRS} NO_DEFAULT_PATH)
else()
  find_library(JANSSON_LIBRARIES ${_JANSSON_LIB_NAME})
  find_path(JANSSON_INCLUDE_DIR "jpeglib.h")
endif()

if (JANSSON_INCLUDE_DIR AND JANSSON_LIBRARIES)
  set(JANSSON_FOUND TRUE)
  message(STATUS "Find libjansson installed: ${JANSSON_LIBRARIES}")
else()
  set(JANSSON_FOUND FALSE)
  message(STATUS "Can`t find libjansson installed")
endif()

