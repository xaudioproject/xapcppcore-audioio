//
//  Copyright 2019 - 2020 The XOrange Studio. All rights reserved.
//  Use of this source code is governed by a BSD-style license that can be
//  found in the LICENSE.md file.
//

//
//  Imports.
//
#include "common.h"

#include <memory>
#include <mutex>
#include <stdio.h>
#include <unistd.h>
#include <xap/audioio/all.h>
#include <xap/core/buffer/buffer.h>
#include <xap/core/buffer/queue.h>

//
//  Entry.
//
int main() {
    xap::core::buffer::BufferQueue audio_queue;
    std::mutex                     audio_queue_lock;

    std::shared_ptr<xap::audioio::DeviceManager> device_mgr = 
        xap::audioio::DeviceManager::load_shared_instance();

    const xap::audioio::InputDevice input_device = 
        device_mgr->load_default_input_device();
    const xap::audioio::OutputDevice output_device = 
        device_mgr->load_default_output_device();

    //
    //  Recorder.
    //
    printf("Recording...\n");
    std::unique_ptr<xap::audioio::RecorderFactory> recorder_factory = 
        std::make_unique<xap::audioio::RecorderFactory>();
    xap::audioio::RecorderOptions recorder_options;
    recorder_options.device = input_device;
    recorder_options.channel_count = 1U;
    recorder_options.frame_pre_buffer = 1024U;
    recorder_options.sample_rate = 16000U;
    recorder_options.suggested_latency = input_device.default_low_latency;
    std::unique_ptr<xap::audioio::IRecorder> recorder = 
        recorder_factory->load_unique_pointer(recorder_options);
    std::function<void(const xap::core::buffer::Buffer &)> audio_callback = 
        [&] (const xap::core::buffer::Buffer &data) {
            audio_queue.push(data);
        };
    recorder->set_audio_callback(audio_callback);

    std::function<void(const xap::audioio::Exception &)> error_callback = 
        [] (const xap::audioio::Exception &error) {
            printf("Recorder exception : %s\n", error.what());
            xap::test::assert_ok(false, "Recorder raised unexpected error.");
        };
    recorder->set_error_callback(error_callback);

    recorder->start();
    usleep(3000U * 1000U);
    recorder->stop();

    printf("Finished recording...\n");

    //
    //  Player.
    //
    printf("Start to play...\n");
    std::unique_ptr<xap::audioio::PlayerFactory> player_factory = 
        std::make_unique<xap::audioio::PlayerFactory>();

    xap::audioio::PlayerOptions player_options;
    player_options.channel_count = 1U;
    player_options.device = output_device;
    player_options.frame_pre_buffer = 1024U;
    player_options.sample_rate = 16000U;
    player_options.suggested_latency = output_device.default_low_latency;
    std::unique_ptr<xap::audioio::IPlayer> player = 
        player_factory->load_unique_pointer(player_options);
    std::function<void(xap::core::buffer::Buffer &)> player_audio_cbk = 
        [&](xap::core::buffer::Buffer &data) {
            //
            //  Lock.
            //
            std::lock_guard<std::mutex> lock(audio_queue_lock);

            if (audio_queue.get_remaining_size() == 0) {
                return;
            }

            if (data.get_length() <= audio_queue.get_remaining_size()) {
                audio_queue.pop(data.get_length()).copy(data);
            } else {
                audio_queue.pop_all().copy(data);
            }
        };
    std::function<void(const xap::audioio::Exception &)> player_error_cbk = 
        [&](const xap::audioio::Exception &error) {
            printf("Player exception: %s\n", error.what());
            xap::test::assert_ok(false, "Player occurred unexpected error.");
        };

    player->set_audio_callback(player_audio_cbk);
    player->set_error_callback(player_error_cbk);

    player->start();

    while (true) {
        {
            //
            //  Lock.
            //
            std::lock_guard<std::mutex> lock(audio_queue_lock);

            if (audio_queue.get_remaining_size() == 0) {
                break;
            }
        }

        usleep(256U * 1000U);
    }
    player->stop(false);
    printf("Finsihed playing...\n");

    return 0;
}