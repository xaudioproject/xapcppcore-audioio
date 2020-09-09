//
//  Copyright 2019 - 2020 The XOrange Studio. All rights reserved.
//  Use of this source code is governed by a BSD-style license that can be
//  found in the LICENSE.md file.
//

#ifndef XAP_TEST_COMMON_H__
#define XAP_TEST_COMMON_H__

//
//  Imports.
//
#include <assert.h>
#include <functional>
#include <stdio.h>
#include <string>

namespace xap {
namespace test {

//
//  Public functions.
//

/**
 *  Excepted 'true' condition.
 * 
 *  @param condition
 *      The condition.
 *  @param message
 *      The message.
 */
static void assert_ok(
    bool condition,
    const char *message = nullptr
) {
    if (message == nullptr) {
        assert(condition);
    } else {
        if (message != nullptr && !condition) {
            printf("Assert Message: %s\n", message);
        }
        assert(((void)message, condition));
    }
}

/**
 *  Expected 'item1' == 'item2'.
 * 
 *  @param item1
 *      The item 1.
 *  @param item2
 *      The item 2
 *  @param message
 *      The message.
 */
template<class T>
static void assert_equal(
    T item1, 
    T item2, 
    const char *message = nullptr
) {
    assert_ok((item1 == item2), message);
}

/**
 *  Expceted callback raised a error.
 * 
 *  @param callback
 *      The callback.
 *  @param message
 *      The message.
 */
template<class Error>
static void assert_throw(
    std::function<void(void)> callback,
    const char *message = nullptr
) {
    try {
        callback();
        assert_ok(false, message);
    } catch (Error &error) {
        //  Do nothing.
    } catch (...) {
        assert_ok(false, "Unexpected error occurred.");
    }
}

/**
 *  Expceted callback doesn't raised a error.
 * 
 *  @param callback
 *      The callback.
 *  @param message
 *      The message.
 */
static void assert_notthrow(
    std::function<void(void)> callback,
    const char *message = nullptr
) {
    try {
        callback();
    } catch (...) {
        assert_ok(false, message);
    }
}

}  //  namespace test
}  //  namespace test

#endif  //  #ifndef XAP_TEST_COMMON_H__