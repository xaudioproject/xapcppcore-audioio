//
//  Copyright 2019 - 2020 The XOrange Studio. All rights reserved.
//  Use of this source code is governed by a BSD-style license that can be
//  found in the LICENSE.md file.
//

//
//  Imports.
//
#include <exception>
#include <portaudio.h>
#include <xap/audioio/device.h>
#include <xap/audioio/error.h>

namespace xap {
namespace audioio {

//
//  Declare.
//
std::weak_ptr<xap::audioio::DeviceManager> 
    xap::audioio::DeviceManager::m_instance;
std::mutex xap::audioio::DeviceManager::m_instance_lock;

//
//  DeviceManager constructor & destructor.
//

/**
 *  Construct the object.
 * 
 *  @throw XAPAudioIOException
 *      Raised if portaudio initialization was failed 
 *      (xap::audioio::ERROR_PORTAUDIOCALL).
 * 
 */
DeviceManager::DeviceManager() {
    PaError error = Pa_Initialize();
    if (error != paNoError) {
        throw xap::audioio::Exception(
            Pa_GetErrorText(error), 
            xap::audioio::ERROR_PORTAUDIOCALL
        );
    }
}

/**
 *  Destruct the object.
 */
DeviceManager::~DeviceManager() noexcept {
    Pa_Terminate();
}

//
//  XAPAudioDeviceManager public methods.
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
std::shared_ptr<xap::audioio::DeviceManager> 
DeviceManager::load_shared_instance() {
    try {
        //
        //  Lock.
        //
        std::lock_guard<std::mutex> lock(
            xap::audioio::DeviceManager::m_instance_lock
        );
        
        if (auto mgr = xap::audioio::DeviceManager::m_instance.lock()) {
            return mgr;
        } else {
            xap::audioio::DeviceManager *manager = 
                new xap::audioio::DeviceManager();
            std::shared_ptr<xap::audioio::DeviceManager> ptr = 
                std::shared_ptr<xap::audioio::DeviceManager>(manager);
            xap::audioio::DeviceManager::m_instance = ptr;
            return ptr;
        }
    } catch (std::system_error &error) {
        throw xap::audioio::Exception(
            error.what(), 
            ERROR_SYSTEMCALL
        );
    } catch (std::bad_alloc &error) {
        throw xap::audioio::Exception(
            error.what(), 
            xap::audioio::ERROR_ALLOC
        );
    }
}

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
std::vector<xap::audioio::InputDevice> DeviceManager::load_all_input_devices() {
    try {
        //
        //  Lock.
        //
        std::lock_guard<std::mutex> lock(this->m_input_device_lock);

        std::vector<xap::audioio::InputDevice> rst;
        
        int devices_count = Pa_GetDeviceCount();
        if (devices_count < 0) {
            throw xap::audioio::Exception(
                Pa_GetErrorText(devices_count), 
                xap::audioio::ERROR_PORTAUDIOCALL
            );
        }

        int default_device = Pa_GetDefaultInputDevice();
        if (default_device < paNoDevice) {
            throw xap::audioio::Exception(
                Pa_GetErrorText(default_device),
                xap::audioio::ERROR_PORTAUDIOCALL
            );
        }

        for (int i = 0; i < devices_count; ++i) {
            const PaDeviceInfo *info = Pa_GetDeviceInfo(i);

            if (info == nullptr || info->maxInputChannels == 0) {
                continue;
            }

            xap::audioio::InputDevice device;
            device.is_default = (i == default_device);
            device.device_id = static_cast<int64_t>(i);
            device.name = std::string(info->name);
            rst.push_back(device);
        }

        return rst;
    } catch (std::system_error &error) {
        throw xap::audioio::Exception(
            error.what(), 
            xap::audioio::ERROR_SYSTEMCALL
        );
    } catch (std::bad_alloc &error) {
        throw xap::audioio::Exception(
            error.what(),
            xap::audioio::ERROR_ALLOC
        );
    }
}

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
std::vector<xap::audioio::OutputDevice> 
DeviceManager::load_all_output_devices() {
    try {
        //
        //  Lock.
        //
        std::lock_guard<std::mutex> lock(this->m_output_device_lock);

        std::vector<xap::audioio::OutputDevice> rst;
        
        int devices_count = Pa_GetDeviceCount();
        if (devices_count < 0) {
            throw xap::audioio::Exception(
                Pa_GetErrorText(devices_count), 
                xap::audioio::ERROR_PORTAUDIOCALL
            );
        }

        int default_device = Pa_GetDefaultOutputDevice();

        if (default_device < paNoDevice) {
            throw xap::audioio::Exception(
                Pa_GetErrorText(default_device),
                xap::audioio::ERROR_PORTAUDIOCALL
            );
        }

        for (int i = 0; i < devices_count; ++i) {
            const PaDeviceInfo *info = Pa_GetDeviceInfo(i);

            if (info == nullptr || info->maxOutputChannels == 0) {
                continue;
            }

            xap::audioio::OutputDevice device;
            device.is_default = (i == default_device);
            device.device_id = static_cast<int64_t>(i);
            device.name = std::string(info->name);
            rst.push_back(device);
        }

        return rst;
    } catch (std::system_error &error) {
        throw xap::audioio::Exception(
            error.what(), 
            xap::audioio::ERROR_SYSTEMCALL
        );
    } catch (std::bad_alloc &error) {
        throw xap::audioio::Exception(
            error.what(), 
            xap::audioio::ERROR_ALLOC
        );
    }
}

}  //  namespace audioio
}  //  namespace xap
