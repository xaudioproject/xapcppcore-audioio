#
#  Copyright 2019 - 2020 The XOrange Studio. All rights reserved.
#  Use of this source code is governed by a BSD-style license that can be
#  found in the LICENSE.md file.
#

#
#  Private functions.
#

function(add_executable_dependencies PROJ_NAME)

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

#  Find package.
find_package(portaudio REQUIRED)

add_executable_dependencies(device-unittest)

add_test(
    NAME                xaptest-device
    COMMAND             ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/device-unittest
    WORKING_DIRECTORY   ${CMAKE_BINARY_DIR}
)

#  Timeout.
set_tests_properties(xaptest-device PROPERTIES TIMEOUT 1)
