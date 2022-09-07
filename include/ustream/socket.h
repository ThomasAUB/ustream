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

#include "ustream/ustream.h"

namespace ustream {

template<typename return_t, typename... args_t>
struct Socket;

template<typename return_t, typename... args_t>
struct Socket<return_t(args_t...)> {

    inline return_t operator()(args_t ... args) const;

    template<auto id>
    void attach();

    void detach();

    template<auto id>
    bool exists();

private:

    static constexpr return_t(*stub)(args_t...) = [](args_t...) { return return_t(); };

    using function_t = return_t(**)(args_t...);
    function_t mFunc = const_cast<function_t>(&stub);

};

template<typename return_t, typename... args_t>
return_t Socket<return_t(args_t...)>::operator()(args_t ... args) const {
    return (*mFunc)(args...);
}

template<typename return_t, typename... args_t>
template<auto id>
void Socket<return_t(args_t...)>::attach() {
    mFunc = Channel<id>::template get<return_t, args_t...>();
}

template<typename return_t, typename... args_t>
void Socket<return_t(args_t...)>::detach() {
    mFunc = const_cast<function_t>(&stub);
}

template<typename return_t, typename... args_t>
template<auto id>
bool Socket<return_t(args_t...)>::exists() {
    return
        (
            (mFunc != const_cast<function_t>(&stub)) &&
            Channel<id>::template exists<return_t, args_t...>()
        );
}




}
