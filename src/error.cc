//
//  Copyright 2019 - 2020 The XOrange Studio. All rights reserved.
//  Use of this source code is governed by a BSD-style license that can be
//  found in the LICENSE.md file.
//

//
//  Imports.
//
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

}  //  namespace audioio
}  //  namespace xap
