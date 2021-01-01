//
//  Copyright 2019 - 2021 The XOrange Studio. All rights reserved.
//  Use of this source code is governed by a BSD-style license that can be
//  found in the LICENSE.md file.
//

#ifndef XAP_AUDIOIO_RECORDER_P_H__
#define XAP_AUDIOIO_RECORDER_P_H__

//
//  Imports.
//
#include <mutex>
#include <portaudio.h>
#include <xap/audioio/recorder.h>

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
static int xap_pa_record_callback(
    const void                      *inputBuffer, 
    void                            *outputBuffer,
    unsigned long                    framesPerBuffer,
    const PaStreamCallbackTimeInfo*  timeInfo,
    PaStreamCallbackFlags            statusFlags,
    void                            *userData
);

/**
 *  Recorder.
 * 
 *  @extends IRecorder
 */
class Recorder: public xap::audioio::IRecorder {
public:
    //
    //  Constructor & destructor.
    //

    /**
     *  Construct the object.
     * 
     *  @throw xap::audioio::Exception
     *      Raised in the following situations:
     * 
     *          - xap::audioio::ERROR_UNSUPPORTED:
     *              Recorder cannot supported this format.
     * 
     *          - xap::audioio::ERROR_PORTAUDIOCALL:
     *              PortAudio calling occurred error.
     * 
     *  @param options
     *      The recorder options.
     */
    Recorder(const xap::audioio::RecorderOptions &options);

    /**
     *  Destruct the object.
     */
    virtual ~Recorder() noexcept;

    //
    //  Public methods.
    //

    /**
     *  Start recorder.
     * 
     *  @throw ap::audioio::Exception
     *      Raised in the following situations:
     * 
     *          - xap::audioio::ERROR_INVALIDOPERATION:
     *              The recorder was already running.
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
        std::function<void(const xap::core::buffer::Buffer &)> &callback
    ) override;

    /**
     *  Set error callback.
     * 
     *  @throw xap::audioio::Exception
     *      Raised if system (lock) calling occurred error. 
     *      (xap::audioio::ERROR_SYSTEMCALL)
     *  @param callback
     *      The callback.
     */
    virtual void set_error_callback(
        std::function<void(const xap::audioio::Exception &)> &callback
    ) override;

    /**
     *  Stop recorder.
     * 
     *  @throw xap::audioio::Exception
     *      Raised in the following situations:
     * 
     *          - xap::audioio::ERROR_INVALIDOPERATION:
     *              The recorder is not running.
     * 
     *          - xap::audioio::ERROR_PORTAUDIOCALL:
     *              PortAudio calling occurred error.
     * 
     *  @param forcibly
     *      True if forcibly
     */
    virtual void stop(bool forcibly = false) override;

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
    void emit_audio_callback(const xap::core::buffer::Buffer &data);

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
    std::function <void(const xap::core::buffer::Buffer &)> 
        m_audio_callback;
    std::mutex                                        m_audio_callback_lock;
    std::function <void(const xap::audioio::Exception &)>   
        m_error_callback;
    std::mutex                                        m_error_callback_lock;
    const xap::audioio::RecorderOptions               m_options;
    PaStreamParameters                                m_pa_parameters;
    PaStream                                         *m_stream;
    bool                                              m_is_running;

    //
    //  Friend functions.
    //
    friend int xap_pa_record_callback(
        const void                      *input_buffer, 
        void                            *output_buffer,
        unsigned long                    frames_per_buffer,
        const PaStreamCallbackTimeInfo*  time_info,
        PaStreamCallbackFlags            status_flags,
        void                            *user_data
    );
};

}  //  namespace audioio
}  //  namespace xap

#endif // XAP_AUDIOIO_RECORDER_P_H__