#
#  Copyright 2019 - 2020 The XOrange Studio. All rights reserved.
#  Use of this source code is governed by a BSD-style license that can be
#  found in the LICENSE.md file.
#

#  Project name.
project(xapsdk-audioio VERSION 1.0.0)

#  Add library.
add_library(
    ${PROJECT_NAME}
    device.cc
    error.cc
    player.cc
    recorder.cc
    ${CMAKE_BINARY_DIR}/third_party/xapcppcore-bufferutilities/src/buffer.cc
    ${CMAKE_BINARY_DIR}/third_party/xapcppcore-bufferutilities/src/error.cc
)

target_include_directories(
    ${PROJECT_NAME} 
    PUBLIC 
    ${CMAKE_BINARY_DIR}/include
)
target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    ${CMAKE_BINARY_DIR}/third_party/xapcppcore-bufferutilities/include
)

#
#  portaudio
#
find_package(portaudio REQUIRED)
#set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -lportaudiocpp")
#set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} -lportaudiocpp")

include(FindPkgConfig)
pkg_check_modules(PORTAUDIO2 portaudio-2.0)

if (PORTAUDIO_FOUND)

    # Add the include directories for compiling.
    target_include_directories(
        ${PROJECT_NAME} 
        PRIVATE 
        ${PORTAUDIO_INCLUDE_DIRS}
    )

    # Add the lib for linking.
    target_link_libraries(
        ${PROJECT_NAME}
        ${PORTAUDIO_LIBRARIES}
    )

else()
    message(FATAL_ERROR "portaudio Not Found")
endif()

#get_cmake_property(_variableNames VARIABLES)
#list (SORT _variableNames)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()