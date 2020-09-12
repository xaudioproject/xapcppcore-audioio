//
//  Copyright 2019 - 2020 The XOrange Studio. All rights reserved.
//  Use of this source code is governed by a BSD-style license that can be
//  found in the LICENSE.md file.
//

#ifndef XAP_AUDIOIO_RECORDER_H__
#define XAP_AUDIOIO_RECORDER_H__

//
//  Imports.
//
#include <xap/audioio/error.h>
#include <xap/audioio/device.h>
#include <xap/core/buffer/buffer.h>

namespace xap {
namespace audioio {

//
//  Structure.
//
typedef struct RecorderOptions_ {
    xap::audioio::InputDevice device;
    uint8_t                   channel_count;
    uint16_t                  sample_rate;
    double                    suggested_latency;
    size_t                    frame_pre_buffer;
} RecorderOptions;

//
//  Classes.
//

/**
 *  Interface of all recorder classes.
 */
class IRecorder {
public:
    //
    //  Destructor.
    //
    
    /**
     *  Destruct the object.
     */
    virtual ~IRecorder() noexcept {};

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
    virtual void start() = 0;

    /**
     *  Set audio callback.
     * 
     *  @param callback
     *      The callback.
     */
    virtual void set_audio_callback(
        std::function<void(const xap::core::buffer::Buffer &)> &callback
    ) = 0;

    /**
     *  Set error callback.
     * 
     *  @param callback
     *      The callback.
     */
    virtual void set_error_callback(
        std::function<void(const xap::audioio::Exception &)> &callback
    ) = 0;

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
     *      True if forcibly
     */
    virtual void stop(bool forcibly = false) = 0;
};

/**
 *  Recorder factory.
 */
class RecorderFactory {
public:
    //
    //  Constructor & destructor.
    //

    /**
     *  Construct the object.
     */
    RecorderFactory() noexcept;

    /**
     *  Destruct the object.
     */
    ~RecorderFactory() noexcept;

    //
    //  Public methods.
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
    std::unique_ptr<xap::audioio::IRecorder> load_unique_pointer(
        const xap::audioio::RecorderOptions &options
    );
    
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
    std::shared_ptr<xap::audioio::IRecorder> load_shared_pointer(
        const xap::audioio::RecorderOptions &options
    );
    
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
    xap::audioio::IRecorder *new_instance(
        const xap::audioio::RecorderOptions &options
    );

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
    void free_instance(xap::audioio::IRecorder **instance);
};

}  //  namespace audioio
}  //  namespace xap


#endif  //  #ifndef XAP_AUDIOIO_RECORDER_H__
