#
#  Copyright 2019 - 2021 The XOrange Studio. All rights reserved.
#  Use of this source code is governed by a BSD-style license that can be
#  found in the LICENSE.md file.
#

#
#  Private functions.
#

function(add_executable_dependencies PROJ_NAME)

    #
    #  xapcppcore-bufferutilities
    #
    target_include_directories(
        ${PROJ_NAME}
        PRIVATE 
        ${CMAKE_BINARY_DIR}/third_party/xapcppcore-bufferutilities/include
    )

    #
    #  xapclientsdk-audioio
    #
    target_include_directories(
        ${PROJ_NAME}
        PRIVATE
        ${CMAKE_BINARY_DIR}/include
    )
    target_link_libraries(
        ${PROJ_NAME}
        ${CMAKE_BINARY_DIR}/lib/libxapsdk-audioio.a
    )

    #
    #  portaudio
    #

    # Add the include directories for compiling.
    target_include_directories(
        ${PROJ_NAME} 
        PRIVATE 
        ${PORTAUDIO_INCLUDE_DIRS}
    )

    # Add the lib for linking.
    target_link_libraries(
        ${PROJ_NAME}
        ${PORTAUDIO_LIBRARIES}
    )

endfunction()

#  Test case.
add_executable(device-unittest device.unittest.cc)
add_executable(
    recorder-player-unittest
    ${CMAKE_BINARY_DIR}/third_party/xapcppcore-bufferutilities/src/buffer.cc
    ${CMAKE_BINARY_DIR}/third_party/xapcppcore-bufferutilities/src/error.cc
    ${CMAKE_BINARY_DIR}/third_party/xapcppcore-bufferutilities/src/fetcher.cc
    ${CMAKE_BINARY_DIR}/third_party/xapcppcore-bufferutilities/src/queue.cc
    recorder_player.unittest.cc
)
target_include_directories(
    recorder-player-unittest
    PRIVATE 
    ${CMAKE_BINARY_DIR}/third_party/xapcppcore-bufferutilities/include
)

#  Find package.
find_package(portaudio REQUIRED)

add_executable_dependencies(device-unittest)
add_executable_dependencies(recorder-player-unittest)

add_test(
    NAME                xaptest-device
    COMMAND             ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/device-unittest
    WORKING_DIRECTORY   ${CMAKE_BINARY_DIR}
)
add_test(
    NAME                xaptest-recorder-player
    COMMAND             ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/recorder-player-unittest
    WORKING_DIRECTORY   ${CMAKE_BINARY_DIR}
)

#  Timeout.
set_tests_properties(xaptest-device PROPERTIES TIMEOUT 1)
set_tests_properties(xaptest-recorder-player PROPERTIES TIMEOUT 300)
