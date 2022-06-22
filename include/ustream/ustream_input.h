/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MIT License                                                                     *
 *                                                                                 *
 * Copyright (c) 2022 Thomas AUBERT                                                *
 *                                                                                 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy    *
 * of this software and associated documentation files (the "Software"), to deal   *
 * in the Software without restriction, including without limitation the rights    *
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell       *
 * copies of the Software, and to permit persons to whom the Software is           *
 * furnished to do so, subject to the following conditions:                        *
 *                                                                                 *
 * The above copyright notice and this permission notice shall be included in all  *
 * copies or substantial portions of the Software.                                 *
 *                                                                                 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE     *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE   *
 * SOFTWARE.                                                                       *
 *                                                                                 *
 * github : https://github.com/ThomasAUB/uStream                                   *
 *                                                                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#pragma once

#include <type_traits>
#include <stdint.h>


#define USTREAM_INPUT_COUNT(input_type, instance_count)                             \
template<> struct unet::InputCount<input_type> {                                    \
    static constexpr uint8_t input_count = instance_count;                          \
};        


namespace ustream {


template<typename derived_t>
struct InputCount {
    static constexpr uint8_t input_count = 1;
};


template<typename derived_t>
struct InputBase {

    InputBase() {
        attach();
    }

    bool attach();

    bool detach();

    template<typename... data_t>
    static bool s_uStreamRead(data_t&... inData);

    template<typename... data_t>
    static bool s_uStreamWrite(const data_t&... inData);

private:

    static constexpr auto input_count = InputCount<derived_t>::input_count;

    static inline derived_t *sDerivedList[input_count] = {nullptr};

};

template<typename derived_t>
bool InputBase<derived_t>::attach() {

    static_assert(
        std::is_convertible<derived_t, InputBase<derived_t>>::value,
        "Custom input must inherit from unet::InputBase"
    );

    // check if this is not already attached
    using slot_idx_t = typename std::decay<decltype(input_count)>::type;
    slot_idx_t freeSlotIdx = 0;
    bool isFreeSlot = false;

    for(auto i = 0 ; i < input_count ; i++) {

        if(sDerivedList[i] == static_cast<derived_t*>(this)) {

            return false;

        }else if(sDerivedList[i] == nullptr) {

            isFreeSlot = true;
            freeSlotIdx = i;

        }
    }

    if(!isFreeSlot) {
        return false;
    }

    sDerivedList[freeSlotIdx] = static_cast<derived_t*>(this);

    return true;

}

template<typename derived_t>
bool InputBase<derived_t>::detach() {

    // check if this is already attached
    for(auto i = 0 ; i < input_count ; i++) {

        if(sDerivedList[i] == static_cast<derived_t*>(this)) {

            sDerivedList[i] = nullptr;

            return true;
        }
    }

    return false;
}

template<typename derived_t>
template<typename... data_t>
bool InputBase<derived_t>::s_uStreamRead(data_t&... d) {

    bool res = false;

    for(auto i = 0 ; i < input_count ; i++) {
        if (sDerivedList[i]) {
            sDerivedList[i]->uStreamRead(d...);
            res = true;
        }
    }

    return res;
}

template<typename derived_t>
template<typename... data_t>
bool InputBase<derived_t>::s_uStreamWrite(const data_t&... d) {

    bool res = false;

    for(auto i = 0 ; i < input_count ; i++) {
        if (sDerivedList[i]) {
            sDerivedList[i]->uStreamWrite(d...);
            res = true;
        }
    }

    return res;
}

}
