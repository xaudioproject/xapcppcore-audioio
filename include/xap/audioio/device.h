//
//  Copyright 2019 - 2020 The XOrange Studio. All rights reserved.
//  Use of this source code is governed by a BSD-style license that can be
//  found in the LICENSE.md file.
//

#ifndef XAP_AUDIOIO_DEVICE_H__
#define XAP_AUDIOIO_DEVICE_H__

//
//  Imports.
//
#include <memory>
#include <mutex>
#include <stdint.h>
#include <string>
#include <vector>

namespace xap {
namespace audioio {

/**
 *  Input device.
 */
typedef struct InputDevice_ {
    int64_t device_id;
    std::string name;
    double default_low_latency;
    double defalut_high_latency;
    bool is_default;
} InputDevice;

/**
 *  Output device.
 */
typedef struct OutputDevice_ {
    int64_t device_id;
    std::string name;
    double default_low_latency;
    double defalut_high_latency;
    bool is_default;
} OutputDevice;

/**
 *  Device manager.
 */
class DeviceManager {
public:
    //
    //  Destructor.
    //
    ~DeviceManager() noexcept;

    //
    //  Public methods.
    //

    /**
     *  Load shared (single) instance.
     * 
     *  @throw xap::audioio::Exception
     *      Raised in the following situations:
     * 
     *          - xap::audioio::ERROR_PORTAUDIOCALL:
     *              Raised if PortAudio initialization was failed.
     * 
     *          - xap::audioio::ERROR_ALLOC:
     *              Raised if memory allocation was failed.
     * 
     *          - xap::audioio::ERROR_SYSTEMCALL:
     *              Raised if system (lock) call was failed.
     * 
     *  @return
     *      The device manager.
     */
    static std::shared_ptr<xap::audioio::DeviceManager> load_shared_instance();

    /**
     *  Load all valid input devices.
     * 
     *  @throw
     *      Raised in the following situations:
     *  
     *          - xap::audioio::ERROR_PORTAUDIOCALL:
     *              Raised if PortAudio calling was failed.
     * 
     *          - xap::audioio::ERROR_ALLOC:
     *              Raised if memory allocation was failed.
     * 
     *          - xap::audioio::ERROR_SYSTEMCALL:
     *              Raised if system (lock) calling was failed.
     * 
     *  @return
     *      The input devices vector.
     */
    const std::vector<const xap::audioio::InputDevice> 
    load_all_input_devices();

    /**
     *  Load all valid output devices.
     * 
     *  @throw
     *      Raised in the following situations:
     *  
     *          - xap::audioio::ERROR_PORTAUDIOCALL:
     *              Raised if PortAudio calling was failed.
     * 
     *          - xap::audioio::ERROR_ALLOC:
     *              Raised if memory allocation was failed.
     * 
     *          - xap::audioio::ERROR_SYSTEMCALL:
     *              Raised if system (lock) calling was failed.
     * 
     *  @return
     *      The output devices vector.
     */
    const std::vector<const xap::audioio::OutputDevice> 
    load_all_output_devices();

    /**
     *  Load default input device.
     * 
     *  @throw
     *      Raised in the following situations:
     *  
     *          - xap::audioio::ERROR_NODEVICE:
     *              Raised if cannot get default input device.
     * 
     *          - xap::audioio::ERROR_PORTAUDIOCALL:
     *              Raised if PortAudio calling was failed.
     * 
     *          - xap::audioio::ERROR_ALLOC:
     *              Raised if memory allocation was failed.
     * 
     *          - xap::audioio::ERROR_SYSTEMCALL:
     *              Raised if system (lock) calling was failed.
     * 
     *  @return
     *      The default input device.
     */
    const xap::audioio::InputDevice load_default_input_device();

    /**
     *  Load default output device.
     * 
     *  @throw
     *      Raised in the following situations:
     *  
     *          - xap::audioio::ERROR_NODEVICE:
     *              Raised if cannot get default output device.
     * 
     *          - xap::audioio::ERROR_PORTAUDIOCALL:
     *              Raised if PortAudio calling was failed.
     * 
     *          - xap::audioio::ERROR_ALLOC:
     *              Raised if memory allocation was failed.
     * 
     *          - xap::audioio::ERROR_SYSTEMCALL:
     *              Raised if system (lock) calling was failed.
     * 
     *  @return
     *      The default output device.
     */
    const xap::audioio::OutputDevice load_default_output_device();

private:
    //
    //  Constructor.
    //

    /**
     *  Construct the object.
     * 
     *  @throw XAPAudioIOException
     *      Raised if portaudio initialization was failed 
     *      (xap::audioio::ERROR_PORTAUDIOCALL).
     * 
     */
    DeviceManager();

    //
    //  Members.
    //
    static std::weak_ptr<xap::audioio::DeviceManager>   m_instance;
    static std::mutex                                   m_instance_lock;
    std::mutex                                          m_input_device_lock;
    std::mutex                                          m_output_device_lock;
};

}  //  namespace audioio
}  //  namespace xap

#endif  //  #ifndef XAP_AUDIOIO_DEVICE_H__