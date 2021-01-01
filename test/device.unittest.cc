//
//  Copyright 2019 - 2021 The XOrange Studio. All rights reserved.
//  Use of this source code is governed by a BSD-style license that can be
//  found in the LICENSE.md file.
//

//
//  Imports.
//
#include "common.h"

#include <stdio.h>
#include <thread>
#include <unistd.h>
#include <xap/audioio/all.h>

void load_input_devices() {
    std::shared_ptr<xap::audioio::DeviceManager> mgr = 
        xap::audioio::DeviceManager::load_shared_instance();
    
    const std::vector<const xap::audioio::InputDevice> input_devices =
        mgr->load_all_input_devices();

    xap::test::assert_ok(
        input_devices.size() != 0,
        "input_devices.size() == 0,"
    );
    
    bool has_default = false;
    for (size_t i = 0; i < input_devices.size(); ++i) {
        printf("=========== Input Device %lu ===========\n", i);
        printf("Device ID: %lld\n", input_devices[i].device_id);
        printf("Device Name: %s\n", input_devices[i].name.c_str());
        if (input_devices[i].is_default) {
            printf("Device is default.\n");
            has_default = true;
        }
    }

    xap::test::assert_ok(
        has_default,
        "There is no default input device.\n"
    );
}

void load_output_devices() {
    std::shared_ptr<xap::audioio::DeviceManager> mgr = 
        xap::audioio::DeviceManager::load_shared_instance();
    const std::vector<const xap::audioio::OutputDevice> output_devices = 
        mgr->load_all_output_devices();

    xap::test::assert_ok(
        output_devices.size() != 0,
        "input_devices.size() == 0"
    );
    
    bool has_default = false;
    for (size_t i = 0; i < output_devices.size(); ++i) {
        printf("=========== Output Device %lu ===========\n", i);
        printf("Device ID: %lld\n", output_devices[i].device_id);
        printf("Device Name: %s\n", output_devices[i].name.c_str());
        if (output_devices[i].is_default) {
            printf("Device is default.\n");
            has_default = true;
        }
    }

    xap::test::assert_ok(
        has_default,
        "There is no default output device.\n"
    );
}

//
//  Main.
//
int main() {
    //
    //  Case 1.
    //
    {
        load_input_devices();
    }

    //
    //  Case 2.
    //
    {
        load_output_devices();
    }

    //
    //  Case 3.
    //
    {
        std::thread t1([] {
            for (size_t i = 0; i < 50U; ++i) {
                load_input_devices();
            }
        });
        std::thread t2([] {
            for (size_t i = 0; i < 50U; ++i) {
                load_input_devices();
            }
        });
        std::thread t3([] {
            for (size_t i = 0; i < 50U; ++i) {
                load_output_devices();
            }
        });
        std::thread t4([] {
            for (size_t i = 0; i < 50U; ++i) {
                load_output_devices();
            }
        });

        t1.join();
        t2.join();
        t3.join();
        t4.join();
    }

    return 0;
}
