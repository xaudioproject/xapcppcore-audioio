//
//  Copyright 2019 - 2020 The XOrange Studio. All rights reserved.
//  Use of this source code is governed by a BSD-style license that can be
//  found in the LICENSE.md file.
//

#ifndef XAP_AUDIOIO_ERROR_P_H__
#define XAP_AUDIOIO_ERROR_P_H__

//
//  Imports.
//
#include <portaudio.h>

namespace xap {
namespace audioio {

//
//  Public functions.
//

/**
 *  Assert PortAudio API calling was succeed.
 * 
 *  @throw xap::audioio::Exception
 *      Raised if PortAudio calling occurred error 
 *      (xap::audioio::ERROR_PORTAUDIOCALL).
 *  @param error
 *      The PortAudio error.
 */
void pacall_assert(const PaError &error);

}  //  namespace audioio
}  //  namespace xap

#endif //  XAP_AUDIOIO_ERROR_P_H__
