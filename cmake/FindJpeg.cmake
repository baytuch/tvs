
cmake_minimum_required(VERSION 2.8.12)

if (UNIX)
  set(_JPEG_LIB_NAME_STATIC "libjpeg.a")
  set(_JPEG_LIB_NAME_SHARED "libjpeg.so")
else()
  set(_JPEG_LIB_NAME "libjpeg.lib")
endif()

if (UNIX AND STATIC)
  set(_JPEG_LIB_NAME ${_JPEG_LIB_NAME_STATIC})
elseif (UNIX AND NOT STATIC)
  set(_JPEG_LIB_NAME ${_JPEG_LIB_NAME_SHARED})
endif()

if (JPEG_INCLUDE_DIRS AND JPEG_LIBRARY_DIRS)
  find_library(JPEG_LIBRARIES ${_JPEG_LIB_NAME} ${JPEG_LIBRARY_DIRS} NO_DEFAULT_PATH)
  find_path(JPEG_INCLUDE_DIR "jpeglib.h" ${JPEG_INCLUDE_DIRS} NO_DEFAULT_PATH)
else()
  find_library(JPEG_LIBRARIES ${_JPEG_LIB_NAME})
  find_path(JPEG_INCLUDE_DIR "jpeglib.h")
endif()

if (JPEG_INCLUDE_DIR AND JPEG_LIBRARIES)
  set(JPEG_FOUND TRUE)
  message(STATUS "Find libjpeg installed: ${JPEG_LIBRARIES}")
else()
  set(JPEG_FOUND FALSE)
  message(STATUS "Can`t find libjpeg installed")
endif()

