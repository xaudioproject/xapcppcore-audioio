//
//  Copyright 2019 - 2021 The XOrange Studio. All rights reserved.
//  Use of this source code is governed by a BSD-style license that can be
//  found in the LICENSE.md file.
//

#ifndef XAP_AUDIOIO_PLAYER_H__
#define XAP_AUDIOIO_PLAYER_H__

//
//  Imports.
//
#include <xap/audioio/device.h>
#include <xap/audioio/error.h>
#include <xap/core/buffer/buffer.h>

namespace xap {
namespace audioio {

//
//  Structure.
//
typedef struct PlayerOptions_ {
    xap::audioio::OutputDevice device;
    uint8_t                    channel_count;
    uint8_t                    __pad1[1];

    uint16_t                   sample_rate;
    uint8_t                    __pad2[4];

    double                     suggested_latency;
    size_t                     frame_pre_buffer;
} PlayerOptions;

/**
 *  Interface of all player classes.
 */
class IPlayer {

public:
    //
    //  Destructor.
    //

    /**
     *  Destruct the object.
     */
    virtual ~IPlayer() noexcept {}

    //
    //  Public methods.
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
    virtual void start() = 0;

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
    ) = 0;

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
    ) = 0;

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
    virtual void stop(bool forcibly) = 0;
};

/**
 *  Player factory.
 */
class PlayerFactory {
public:
    //
    //  Constructor & destructor.
    //

    /**
     *  Construct the object.
     */
    PlayerFactory() noexcept;

    /**
     *  Destruct the object.
     */
    ~PlayerFactory() noexcept;

    //
    //  Public methods.
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
    std::unique_ptr<xap::audioio::IPlayer> load_unique_pointer(
        const xap::audioio::PlayerOptions &options
    );

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
    std::shared_ptr<xap::audioio::IPlayer> load_shared_pointer(
        const xap::audioio::PlayerOptions &options
    );

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
    xap::audioio::IPlayer *new_instance(
        const xap::audioio::PlayerOptions &options
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
    void free_instance(xap::audioio::IPlayer **instance);
};

}  //  namespace audioio
}  //  namespace xap

#endif //  #ifndef XAP_AUDIOIO_PLAYER_H__