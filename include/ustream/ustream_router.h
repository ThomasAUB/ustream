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

#include <tuple>


#define USTREAM_SET_INPUT(id, ...)                                                  \
template<> struct                                                                   \
ustream::InputList<id> { using list_t = std::tuple<__VA_ARGS__>; };                 \



namespace ustream {


template<auto id>
class InputList {
    struct unknown_stream {

        template<typename... data_t>
        [[deprecated]]
        static bool s_uStreamRead(data_t&... inData) { return false; }

        template<typename... data_t>
        [[deprecated]]
        static bool s_uStreamWrite(const data_t&... inData) { return false; }
        
    };
public:
    using list_t = std::tuple<unknown_stream>;
};





class Router {

    template<auto id>
    using dest_tuple = typename InputList<id>::list_t;

    template<auto id, auto read_write, std::size_t I, typename... data_t>
    static constexpr bool dispatchSingle(data_t&... d) {

        using dest_t = typename std::tuple_element<I, dest_tuple<id>>::type;

        if constexpr (read_write == eType::eRead) {
            return dest_t::s_uStreamRead(d...);
        }else if (read_write == eType::eWrite) {
            return dest_t::s_uStreamWrite(d...);
        }

        return false;
    }

    template<auto id, auto read_write, std::size_t ... Is, typename... data_t>
    static constexpr bool dispatch(const std::index_sequence<Is...>&, data_t&... d) {
        return (dispatchSingle<id, read_write, Is>(d...) | ...);
    }

public:

    enum class eType {
        eRead,
        eWrite
    };
    
    template<auto id, auto read_write, typename... data_t>
    static constexpr bool send(data_t&... d) {
        return
            dispatch<id, read_write>(
                std::make_index_sequence<std::tuple_size<dest_tuple<id>>::value>{},
                d...
            );
    }

};

}
