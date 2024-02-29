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
#include <utility>

namespace ustream {

    template<auto address, typename ... args_t>
    bool open(ISlot<args_t...>& s);

    template<typename ... args_t>
    void close(ISlot<args_t...>& s);

    template<auto address, typename ... args_t>
    void broadcast(args_t&&... args);


    namespace detail {

        template<auto address, typename ... args_t>
        struct Server {

            static void connect(ISlot<args_t...>& s) {
                sSlots.push_front(s);
            }

            static void emit(args_t&&... args) {
                for (auto& s : sSlots) {
                    s.slotInput(std::forward<args_t>(args)...);
                }
            }

        private:
            thread_local inline static ulink::List<ISlot<args_t...>> sSlots;
        };

    }

    template<auto address, typename ... args_t>
    void broadcast(args_t&&... args) {
        detail::Server<address, args_t...>::emit(std::forward<args_t>(args)...);
    }

    template<auto address, typename ... args_t>
    bool open(ISlot<args_t...>& s) {
        if (s.isLinked()) {
            return false;
        }
        detail::Server<address, args_t...>::connect(s);
        return true;
    }

    template<typename ... args_t>
    void close(ISlot<args_t...>& s) {
        s.disconnect();
    }

}