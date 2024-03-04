/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MIT License                                                                     *
 *                                                                                 *
 * Copyright (c) 2024 Thomas AUBERT                                                *
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

#include "islot.hpp"

namespace ustream {

    template<typename ... args_t>
    struct Signal {

        /**
         * @brief Connects a slot to this signal.
         *
         * @param inSlot Slot to connect.
         * @return true if the connection succeeded
         * @return false otherwise.
         */
        bool connect(ISlot<args_t...>& inSlot);

        /**
         * @brief Emits data to the connected slots.
         *
         * @param args data to emit.
         * @return true if at least one slot is connected
         * @return false otherwise.
         */
        bool emit(args_t&& ... args);

    private:
        ulink::List<ISlot<args_t...>> mSlots;
    };


    template<typename ... args_t>
    bool Signal<args_t...>::connect(ISlot<args_t...>& inSlot) {
        if (inSlot.isLinked()) {
            return false;
        }
        mSlots.push_front(inSlot);
        return true;
    }

    template<typename ... args_t>
    bool Signal<args_t...>::emit(args_t&& ... args) {
        if (mSlots.empty()) {
            return false;
        }
        for (auto& s : mSlots) {
            s.processSignal(std::forward<args_t>(args)...);
        }
        return true;
    }

}