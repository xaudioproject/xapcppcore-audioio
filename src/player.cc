//
//  Copyright 2019 - 2020 The XOrange Studio. All rights reserved.
//  Use of this source code is governed by a BSD-style license that can be
//  found in the LICENSE.md file.
//

//
//  Imports.
//
#include "error_p.h"
#include "player_p.h"

#include <xap/audioio/player.h>

namespace xap {
namespace audioio {

//
//  Player constructor & destructor.
//

/**
 *  Construct the object.
 * 
 *  @throw xap::audioio::Exception
 *      Raised in the following situations:
 * 
 *          - xap::audioio::ERROR_UNSUPPORTED:
 *              Player cannot supported this format.
 * 
 *          - xap::audioio::ERROR_PORTAUDIOCALL:
 *              PortAudio calling occurred error.
 * 
 *  @param options
 *      The player options.
 */
Player::Player(const xap::audioio::PlayerOptions &options) :
    m_audio_callback(),
    m_audio_callback_lock(),
    m_error_callback(),
    m_error_callback_lock(),
    m_options(options),
    m_stream(nullptr),
    m_is_running(false) 
{
    //
    //  Initialzie PortAudio.
    //
    xap::audioio::pacall_assert(Pa_Initialize());

    //
    //  Build PortAudio parameters.
    //
    PaStreamParameters stream_parameters;
    stream_parameters.device = static_cast<int>(options.device.device_id);
    stream_parameters.channelCount = static_cast<int>(options.channel_count);
    stream_parameters.suggestedLatency = options.suggested_latency;
    stream_parameters.sampleFormat = paInt16;
    stream_parameters.hostApiSpecificStreamInfo = nullptr;

    PaError error = Pa_IsFormatSupported(
        nullptr,
        &stream_parameters,
        static_cast<double>(options.sample_rate)
    );
    if (error != paNoError) {
        throw xap::audioio::Exception(
            Pa_GetErrorText(error),
            ERROR_UNSUPPORTED
        );
    }

    //
    //  Open PortAudio stream.
    //
    xap::audioio::pacall_assert(Pa_OpenStream(
        &(this->m_stream),
        nullptr,
        &stream_parameters,
        static_cast<double>(options.sample_rate),
        static_cast<unsigned long>(options.frame_pre_buffer),
        paNoFlag,
        xap_pa_play_callback,
        static_cast<void *>(this)
    ));
}

/**
 *  Destruct the object.
 */
Player::~Player() noexcept {
    if (this->m_is_running) {
        try {
            this->stop(true);
        } catch (xap::audioio::Exception &) {
            //  Do nothing.
        }
    }

    Pa_CloseStream(this->m_stream);
    Pa_Terminate();
}

//
//  Player public methods.
//

/**
 *  Start player.
 * 
 *  @throw xap::audioio::Exception
 *      Raised in the following situations:
 * 
 *          - xap::audioio::ERROR_INVALIDOPERATION:
 *              The player was already running.
 * 
 *          - xap::audioio::ERROR_PORTAUDIOCALL:
 *              PortAudio calling occurred error.
 */
void Player::start() {
    if (this->m_is_running) {
        throw xap::audioio::Exception(
            "The player was already running.",
            xap::audioio::ERROR_INVALIDOPERATION
        );
    }

    xap::audioio::pacall_assert(Pa_StartStream(this->m_stream));

    this->m_is_running = true; 
}

/**
 *  Set audio callback.
 * 
 *  @throw xap::audioio::Exception
 *      Raised if system (lock) calling occurred error. 
 *      (xap::audioio::ERROR_SYSTEMCALL)
 *  @param callback
 *      The callback.
 */
void Player::set_audio_callback(
    std::function <void(xap::core::buffer::Buffer &)> &callback
) {
    try {
        //
        //  Lock.
        //
        std::lock_guard<std::mutex> lock(this->m_audio_callback_lock);

        this->m_audio_callback = callback;
    } catch (std::system_error &error) {
        throw xap::audioio::Exception(
            error.what(),
            xap::audioio::ERROR_SYSTEMCALL
        );
    }
}

/**
 *  Set error callback.
 * 
 *  @throw xap::audioio::Exception
 *      Raised if system (lock) calling occurred error.
 *  @param callback
 *      The callback.
 */
void Player::set_error_callback(
    std::function <void(const xap::audioio::Exception &)> &callback
) {
    try {
        //
        //  Lock.
        //
        std::lock_guard<std::mutex> lock(this->m_error_callback_lock);

        this->m_error_callback = callback;
    } catch (std::system_error &error) {
        throw xap::audioio::Exception(
            error.what(),
            xap::audioio::ERROR_SYSTEMCALL
        );
    }
}

/**
 *  Stop player.
 * 
 *  @throw xap::audioio::Exception
 *      Raised in the following situations:
 * 
 *          - xap::audioio::ERROR_INVALIDOPERATION:
 *              The player is not running.
 * 
 *          - xap::audioio::ERROR_PORTAUDIOCALL:
 *              Port audio calling occurred error.
 * 
 *  @param forcibly
 *      True if forcibly.
 */
void Player::stop(bool forcibly) {
    if (!this->m_is_running) {
        throw xap::audioio::Exception(
            "The player is not running.",
            xap::audioio::ERROR_INVALIDOPERATION
        );
    }

    if (forcibly) {
        xap::audioio::pacall_assert(Pa_AbortStream(this->m_stream));
    } else {
        xap::audioio::pacall_assert(Pa_StopStream(this->m_stream));
    }

    this->m_is_running = false;
}

//
//  Player private methods.
//

/**
 *  Emit audio callback event.
 * 
 *  @throw xap::audioio::Exception
 *      Raised in the following situations:
 * 
 *          - xap::audioio::ERROR_SYSTEMCALL:
 *              System (lock) calling occurred error.
 * 
 *          - xap::audioio::ERROR_CALLBACK:
 *              Callback occurred error.
 * 
 *  @param data
 *      The audio data (parameter 'data').
 */
void Player::emit_audio_callback(xap::core::buffer::Buffer &data) {
    try {
        //
        //  Lock.
        //
        std::lock_guard<std::mutex> lock(this->m_audio_callback_lock);

        this->m_audio_callback(data);
    } catch (std::system_error &error) {
        throw xap::audioio::Exception(
            error.what(), 
            xap::audioio::ERROR_SYSTEMCALL
        );
    } catch (std::exception &error) {
        throw xap::audioio::Exception(
            error.what(),
            xap::audioio::ERROR_CALLBACK
        );
    }
}

/**
 *  Emit error callback event.
 * 
 *  @throw xap::audioio::Exception
 *      Raised in the following situations:
 * 
 *          - xap::audioio::ERROR_SYSTEMCALL:
 *              System (lock) calling occurred error.
 * 
 *          - xap::audioio::ERROR_CALLBACK:
 *              Callback occurred error.
 * 
 *  @param error
 *      The parameter 'error'.
 */
void Player::emit_error_callback(const xap::audioio::Exception &e) {
    try {
        //
        //  Lock.
        //
        std::lock_guard<std::mutex> lock(this->m_error_callback_lock);

        this->m_error_callback(e);
    } catch (std::system_error &error) {
        throw xap::audioio::Exception(
            error.what(),
            xap::audioio::ERROR_SYSTEMCALL
        );
    } catch (std::exception &error) {
        throw xap::audioio::Exception(
            error.what(),
            xap::audioio::ERROR_CALLBACK
        );
    }
}

//
//  PlayerFactory constructor & destructor.
//

/**
 *  Construct the object.
 */
PlayerFactory::PlayerFactory() noexcept {
    //  Do nothing.
}

/**
 *  Destruct the object.
 */
PlayerFactory::~PlayerFactory() noexcept {
    //  Do nothing.
}

//
//  PlayerFactory public methods.
//

/**
 *  Load unique pointer.
 *  
 *  @throw xap::audioio::Exception
 *      Raised in the following situations:
 * 
 *          - xap::audioio::ERROR_UNSUPPORTED:
 *              Player cannot supported this format.
 * 
 *          - xap::audioio::ERROR_PORTAUDIOCALL:
 *              PortAudio calling occurred error.
 * 
 *          - xap::audioio::ERROR_ALLOC:
 *              Memory allocation was failed.
 * 
 *  @return
 *      The unique pointer.
 */
std::unique_ptr<xap::audioio::IPlayer> PlayerFactory::load_unique_pointer(
    const xap::audioio::PlayerOptions &options
) {
    try {
        xap::audioio::IPlayer *ptr = new xap::audioio::Player(options);
        return std::unique_ptr<xap::audioio::IPlayer>(ptr);
    } catch (std::bad_alloc &error) {
        throw xap::audioio::Exception(
            error.what(),
            xap::audioio::ERROR_ALLOC
        );
    }
}

/**
 *  Load shared pointer.
 *  
 *  @throw xap::audioio::Exception
 *      Raised in the following situations:
 * 
 *          - xap::audioio::ERROR_UNSUPPORTED:
 *              Player cannot supported this format.
 * 
 *          - xap::audioio::ERROR_PORTAUDIOCALL:
 *              PortAudio calling occurred error.
 * 
 *          - xap::audioio::ERROR_ALLOC:
 *              Memory allocation was failed.
 * 
 *  @return
 *      The shared pointer.
 */
std::shared_ptr<xap::audioio::IPlayer> PlayerFactory::load_shared_pointer(
    const xap::audioio::PlayerOptions &options
) {
    try {
        xap::audioio::IPlayer *ptr = new xap::audioio::Player(options);
        return std::shared_ptr<xap::audioio::IPlayer>(ptr);
    } catch (std::bad_alloc &error) {
        throw xap::audioio::Exception(
            error.what(),
            xap::audioio::ERROR_ALLOC
        );
    }
}

/**
 *  Load new instance.
 *  
 *  @throw xap::audioio::Exception
 *      Raised in the following situations:
 * 
 *          - xap::audioio::ERROR_UNSUPPORTED:
 *              Player cannot supported this format.
 * 
 *          - xap::audioio::ERROR_PORTAUDIOCALL:
 *              PortAudio calling occurred error.
 * 
 *          - xap::audioio::ERROR_ALLOC:
 *              Memory allocation was failed.
 * 
 *  @return
 *      The instance.
 */
xap::audioio::IPlayer* PlayerFactory::new_instance(
    const xap::audioio::PlayerOptions &options
) {
    try {
        return new xap::audioio::Player(options);
    } catch (std::bad_alloc &error) {
        throw xap::audioio::Exception(
            error.what(),
            xap::audioio::ERROR_ALLOC
        );
    }
}

/**
 *  Release instance.
 * 
 *  @throw xap::audioio::Exception
 *      Raised in the following situations:
 * 
 *          - xap::audioio::ERROR_PARAMETER:
 *              instance == nullptr
 * 
 *          - xap::audioio::ERROR_INVALIDOPERATION:
 *              The instance has already been released.
 * 
 *  @param instance
 *      The instance.
 */
void PlayerFactory::free_instance(xap::audioio::IPlayer **instance) {
    if (instance == nullptr) {
        throw xap::audioio::Exception(
            "instance == nullptr",
            xap::audioio::ERROR_PARAMETER
        );
    }
    if (*instance == nullptr) {
        throw xap::audioio::Exception(
            "The instance has already been released.",
            xap::audioio::ERROR_INVALIDOPERATION
        );
    }

    free(*instance);
    *instance = nullptr;
}

//
//  Private functions.
//

/**
 *  XAP PortAudio callback.
 * 
 *  @param input_buffer
 *      The input buffer.
 *  @param output_buffer
 *      The output buffer.
 *  @param frames_pre_buffer
 *      The number of sample frames to be processed by the stream callback.
 *  @param time_info
 *      Timestamps indicating the ADC capture time of the first sample in the 
 *      input buffer, the DAC output time of the first sample in the output 
 *      buffer and the time the callback was invoked. 
 *  @param status_flags
 *      Flags indicating whether input and/or output buffers have been inserted 
 *      or will be dropped to overcome underflow or overflow conditions.
 *  @param user_data
 *      The value of a user supplied pointer passed to Pa_OpenStream() intended 
 *      for storing synthesis data etc.
 */
static int xap_pa_play_callback(
    const void                      *input_buffer, 
    void                            *output_buffer,
    unsigned long                    frames_per_buffer,
    const PaStreamCallbackTimeInfo*  time_info,
    PaStreamCallbackFlags            status_flags,
    void                            *user_data
) {
    xap::audioio::Player *player = 
        reinterpret_cast<xap::audioio::Player *>(user_data);
    try {
        size_t datalen = static_cast<size_t>(frames_per_buffer * 2U);
        xap::core::buffer::Buffer data(datalen, false);
        player->emit_audio_callback(data);

        memcpy(output_buffer, data.get_pointer(), datalen);
    } catch (xap::core::buffer::BufferException &error) {
        try {
            player->emit_error_callback(xap::audioio::Exception(
                error.what(),
                xap::audioio::ERROR_ALLOC
            ));
        } catch (xap::audioio::Exception &) {
            //  Do nothing.
        }
    } catch (xap::audioio::Exception &error) {
        try {
            player->emit_error_callback(error);
        } catch (xap::audioio::Exception &) {
            //  Do nothing.
        }
    }

    return paContinue;
}

}  //  namespace audioio
}  //  namespace xap