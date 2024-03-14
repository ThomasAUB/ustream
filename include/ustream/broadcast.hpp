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
 * github : https://github.com/ThomasAUB/ustream                                   *
 *                                                                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#pragma once

#include "signal.hpp"

namespace ustream {

    /**
     * @brief Open a slot at the given address.
     *
     * @tparam address Signal address.
     * @tparam args_t Argument types.
     * @param s Slot to connect.
     * @return true if the connection succeeded
     * @return false otherwise.
     */
    template<auto address, typename ... args_t>
    bool open(ISlot<args_t...>& s);

    /**
     * @brief Close a slot.
     *
     * @tparam args_t Argument types.
     * @param s Slot to close.
     */
    template<typename ... args_t>
    void close(ISlot<args_t...>& s);

    /**
     * @brief Broadcast data at a given address.
     *
     * @tparam address Broadcast address.
     * @tparam args_t Argument types.
     * @param args Data to broadcast.
     * @return true if at least one slot is connected.
     * @return false otherwise.
     */
    template<auto address, typename ... args_t>
    void broadcast(args_t... args);

    namespace detail {

        template<auto address, typename ... args_t>
        Signal<args_t...>& getSignal() {
            thread_local static Signal<args_t...> sSignal;
            return sSignal;
        }

    }

    template<auto address, typename ... args_t>
    void broadcast(args_t... args) {
        detail::getSignal<address, args_t...>().emit(args...);
    }

    template<auto address, typename ... args_t>
    bool open(ISlot<args_t...>& s) {
        return detail::getSignal<address, args_t...>().connect(s);
    }

    template<typename ... args_t>
    void close(ISlot<args_t...>& s) {
        s.disconnect();
    }

}