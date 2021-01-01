//
//  Copyright 2019 - 2021 The XOrange Studio. All rights reserved.
//  Use of this source code is governed by a BSD-style license that can be
//  found in the LICENSE.md file.
//

//
//  Imports.
//
#include "error_p.h"

#include <string>
#include <xap/audioio/error.h>

namespace xap {
namespace audioio {

/**
 *  Construct the object.
 * 
 *  @param message
 *      The error message.
 *  @param code
 *      The error code.
 */
Exception::Exception(
    const char     *message,
    const uint16_t  code
) :
    m_message(message),
    m_code(code)
{}

/**
 *  Construct (Copy) the object.
 * 
 *  @param src
 *      The source object.
 */
Exception::Exception(
    const xap::audioio::Exception &src
) :
    m_message(src.m_message),
    m_code(src.m_code)
{}

/**
 *  Destruct the object.
 */
Exception::~Exception() noexcept {
    //  Do nothing.
}

//
//  Public methods.
//

/**
 *  Get the error message.
 * 
 *  @return
 *      The error message.
 */
const char* Exception::what() const noexcept {
    return this->m_message.c_str();
}

/**
 *  Get the error code.
 * 
 *  @return
 *      The error code.
 */
uint16_t Exception::get_code() const noexcept {
    return this->m_code;
}

/**
 *  Assert PortAudio API calling was succeed.
 * 
 *  @throw xap::audioio::Exception
 *      Raised if PortAudio calling occurred error 
 *      (xap::audioio::ERROR_PORTAUDIOCALL).
 *  @param error
 *      The PortAudio error.
 */
void pacall_assert(const PaError &error) {
    if (error != paNoError) {
        throw xap::audioio::Exception(
            Pa_GetErrorText(error),
            xap::audioio::ERROR_PORTAUDIOCALL
        );
    }
}

}  //  namespace audioio
}  //  namespace xap
