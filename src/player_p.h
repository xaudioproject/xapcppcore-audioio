//
//  Copyright 2019 - 2020 The XOrange Studio. All rights reserved.
//  Use of this source code is governed by a BSD-style license that can be
//  found in the LICENSE.md file.
//

#ifndef XAP_AUDIOIO_PLAYER_P_H__
#define XAP_AUDIOIO_PLAYER_P_H__

//
//  Imports.
//
#include <mutex>
#include <portaudio.h>
#include <xap/audioio/player.h>

namespace xap {
namespace audioio {

//
//  Declare.
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
    unsigned long                    frames_pre_buffer,
    const PaStreamCallbackTimeInfo*  time_info,
    PaStreamCallbackFlags            status_flags,
    void                            *user_data
);

class Player: public xap::audioio::IPlayer {
public:

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
    Player(const xap::audioio::PlayerOptions &options);

    /**
     *  Destruct the object.
     */
    virtual ~Player() noexcept;

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
    virtual void start() override;

    /**
     *  Set audio callback.
     * 
     *  @throw xap::audioio::Exception
     *      Raised if system (lock) calling occurred error. 
     *      (xap::audioio::ERROR_SYSTEMCALL)
     *  @param callback
     *      The callback.
     */
    virtual void set_audio_callback(
        std::function <void(xap::core::buffer::Buffer &)> &callback
    ) override;

    /**
     *  Set error callback.
     * 
     *  @throw xap::audioio::Exception
     *      Raised if system (lock) calling occurred error.
     *  @param callback
     *      The callback.
     */
    virtual void set_error_callback(
        std::function <void(const xap::audioio::Exception &)> &callback
    ) override;

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
    virtual void stop(bool forcibly) override;

private:
    //
    //  Private methods.
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
    void emit_audio_callback(xap::core::buffer::Buffer &data);
    
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
    void emit_error_callback(const xap::audioio::Exception &error);

    //
    //  Members.
    //
    std::function <void(xap::core::buffer::Buffer &)>     m_audio_callback;
    std::mutex                                            m_audio_callback_lock;
    std::function <void(const xap::audioio::Exception &)> m_error_callback;
    std::mutex                                            m_error_callback_lock;
    const xap::audioio::PlayerOptions                     m_options;
    PaStream                                             *m_stream;
    bool                                                  m_is_running;

    //
    //  Friend functions.
    //
    friend int xap_pa_play_callback(
        const void                      *inputBuffer, 
        void                            *outputBuffer,
        unsigned long                    framesPerBuffer,
        const PaStreamCallbackTimeInfo*  timeInfo,
        PaStreamCallbackFlags            statusFlags,
        void                            *userData
    );
};

}  //  namespace audioio
}  //  namespace xap

#endif  //  #ifndef XAP_AUDIOIO_PLAYER_P_H__