//
//  Copyright 2019 - 2020 The XOrange Studio. All rights reserved.
//  Use of this source code is governed by a BSD-style license that can be
//  found in the LICENSE.md file.
//

//
//  Imports.
//
#include "error_p.h"
#include "recorder_p.h"

#include <mutex>
#include <xap/audioio/recorder.h>

namespace xap {
namespace audioio {

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
Recorder::Recorder(
    const xap::audioio::RecorderOptions &options
) :
    m_audio_callback(),
    m_error_callback(),
    m_options(options),
    m_stream(nullptr),
    m_is_running(false)
{
    //
    //  Initialize PortAudio.
    //
    xap::audioio::pacall_assert(Pa_Initialize());

    //
    //  Build PortAudio parameters.
    //
    this->m_pa_parameters.device 
        = static_cast<int>(options.device.device_id);
    this->m_pa_parameters.channelCount 
        = static_cast<int>(options.channel_count);
    this->m_pa_parameters.suggestedLatency 
        = static_cast<PaTime>(options.suggested_latency);
    this->m_pa_parameters.sampleFormat 
        = paInt16;
    this->m_pa_parameters.hostApiSpecificStreamInfo 
        = nullptr;
    PaError error = Pa_IsFormatSupported(
        &(this->m_pa_parameters), 
        nullptr, 
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
        &(this->m_pa_parameters),
        nullptr,
        static_cast<double>(options.sample_rate),
        static_cast<unsigned long>(options.frame_pre_buffer),
        paNoFlag,
        xap_pa_record_callback,
        static_cast<void *>(this)
    ));
}

/**
 *  Destruct the object.
 */
Recorder::~Recorder() noexcept {
    if (this->m_is_running) {
        try {
            this->stop();
        } catch (xap::audioio::Exception &) {
            //  Do nothing.
        }
    }

    Pa_CloseStream(this->m_stream);
    Pa_Terminate();
}

//
//  Recorder public methods.
//

/**
 *  Start recorder.
 * 
 *  @throw xap::audioio::Exception
 *      Raised in the following situations:
 * 
 *          - xap::audioio::ERROR_INVALIDOPERATION:
 *              The recorder was already running.
 * 
 *          - xap::audioio::ERROR_PORTAUDIOCALL:
 *              PortAudio calling occurred error.
 */
void Recorder::start() {
    if (this->m_is_running) {
        throw xap::audioio::Exception(
            "The recorder was already running.",
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
void Recorder::set_audio_callback(
    std::function<void(const xap::core::buffer::Buffer &)> &callback
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
 *      (xap::audioio::ERROR_SYSTEMCALL)
 *  @param callback
 *      The callback.
 */
void Recorder::set_error_callback(
    std::function<void(const xap::audioio::Exception &)> &callback
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
 *  Stop recorder.
 * 
 *  @throw xap::audioio::Exception
 *      Raised in the following situations:
 * 
 *          - xap::audioio::ERROR_INVALIDOPERATION:
 *              The recorder is not running.
 * 
 *          - xap::audioio::ERROR_PORTAUDIOCALL:
 *              Port audio calling occurred error.
 * 
 *  @param forcibly
 *      True if forcibly.
 */
void Recorder::stop(bool forcibly) {
    if (!this->m_is_running) {
        throw xap::audioio::Exception(
            "The recorder is not running.",
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
//  Recorder private methods.
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
void Recorder::emit_audio_callback(const xap::core::buffer::Buffer &data) {
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
void Recorder::emit_error_callback(const xap::audioio::Exception &error) {
    try {
        //
        //  Lock.
        //
        std::lock_guard<std::mutex> lock(this->m_error_callback_lock);

        this->m_error_callback(error);
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
//  RecorderFactory constructor & destructor.
//

/**
 *  Construct the object.
 */
RecorderFactory::RecorderFactory() noexcept {
    //  Do nothing.
}

/**
 *  Destruct the object.
 */
RecorderFactory::~RecorderFactory() noexcept {
    //  Do nothing.
}

//
//  RecorderFactory public methods.
//

/**
 *  Load unique pointer.
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
 *          - xap::audioio::ERROR_ALLOC:
 *              Memory allocation was failed.
 * 
 *  @return
 *      The unique pointer.
 */
std::unique_ptr<xap::audioio::IRecorder> RecorderFactory::load_unique_pointer(
    const xap::audioio::RecorderOptions &options
) {
    try {
        xap::audioio::IRecorder *ptr = new xap::audioio::Recorder(options);
        return std::unique_ptr<xap::audioio::IRecorder>(ptr);
    } catch (xap::audioio::Exception &error) {
        throw xap::audioio::Exception(error.what(), xap::audioio::ERROR_ALLOC);
    }
}

/**
 *  Load shared pointer.
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
 *          - xap::audioio::ERROR_ALLOC:
 *              Memory allocation was failed.
 * 
 *  @return
 *      The shared pointer.
 */
std::shared_ptr<xap::audioio::IRecorder> RecorderFactory::load_shared_pointer(
    const xap::audioio::RecorderOptions &options
) {
    try {
        xap::audioio::IRecorder *ptr = new xap::audioio::Recorder(options);
        return std::shared_ptr<xap::audioio::IRecorder>(ptr);
    } catch (std::bad_alloc &error) {
        throw xap::audioio::Exception(error.what(), xap::audioio::ERROR_ALLOC);
    }
}

/**
 *  Load new instance.
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
 *          - xap::audioio::ERROR_ALLOC:
 *              Memory allocation was failed.
 * 
 *  @return
 *      The instance.
 */
xap::audioio::IRecorder *RecorderFactory::new_instance(
    const xap::audioio::RecorderOptions &options
) {
    try {
        return new xap::audioio::Recorder(options);
    } catch (std::bad_alloc &error) {
        throw xap::audioio::Exception(error.what(), xap::audioio::ERROR_ALLOC);
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
void RecorderFactory::free_instance(xap::audioio::IRecorder **instance) {
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
static int xap_pa_record_callback(
    const void                      *input_buffer, 
    void                            *output_buffer,
    unsigned long                    frames_per_buffer,
    const PaStreamCallbackTimeInfo*  time_info,
    PaStreamCallbackFlags            status_flags,
    void                            *user_data
) {
    Recorder *recorder = reinterpret_cast<Recorder *>(user_data);
    try {
        xap::core::buffer::Buffer audio_data(
            reinterpret_cast<const uint8_t *>(input_buffer),
            static_cast<size_t>(frames_per_buffer) * 2U  //  16-bit
        );
        
        recorder->emit_audio_callback(audio_data);
    } catch (xap::core::buffer::BufferException &error) {
        recorder->emit_error_callback(
            xap::audioio::Exception(
                error.what(), 
                xap::audioio::ERROR_ALLOC
            )
        );
    } catch (xap::audioio::Exception &error) {
        try {
            recorder->emit_error_callback(error);
        } catch (xap::audioio::Exception &) {
            //  Do nothing.
        }
    } catch (std::exception &error) {
        try {
            recorder->emit_error_callback(xap::audioio::Exception(
                error.what(),
                ERROR_UNEXPECTED
            ));
        } catch (xap::audioio::Exception &) {
            //  Do nothing.
        }
    }
    
    return paContinue;
}

}  //  namespace audioio
}  //  namespace xap