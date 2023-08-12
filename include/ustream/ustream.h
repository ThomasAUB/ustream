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


namespace ustream {

    template<auto id>
    struct Channel {

        /**
         * @brief Associate an immutable function.
         * 
         * @tparam return_t Return type of the function.
         * @tparam args_t Arguments types of the function.
         * @param inFunc Function pointer to associate.
         * @return true if the association was successful
         * @return false otherwise.
         */
        template<typename return_t, typename ... args_t>
        static bool setImmutable(return_t(*inFunc)(args_t...));

        /**
         * @brief Associate a mutable function.
         * 
         * @tparam return_t Return type of the function.
         * @tparam args_t Arguments types of the function.
         * @param inFunc Function pointer to associate.
         * @return true if the association was successful
         * @return false otherwise.
         */
        template<typename return_t, typename ... args_t>
        static bool setMutable(return_t(*inFunc)(args_t...));

        /**
         * @brief Calls the associated function with the matching arguments.
         * 
         * @tparam return_t Return type of the function.
         * @tparam args_t Arguments types of the function.
         * @param args Arguments of the function.
         * @return auto Return value of the function.
         */
        template<typename return_t, typename ... args_t>
        static auto call(args_t... args);

        /**
         * @brief Calls the associated function with the matching arguments 
         * and returning 'void'.
         * 
         * @tparam args_t Argument types.
         * @param args Arguments of the function.
         */
        template<typename ... args_t>
        static void call(args_t... args);

        /**
         * @brief Get a pointer to the function pointer.
         * 
         * @tparam return_t Return type of the function.
         * @tparam args_t Arguments types of the function.
         * @return auto Function pointer pointer.
         */
        template<typename return_t, typename ... args_t>
        static auto get();

        /**
         * @brief Deletes the associated function.
         * 
         * @tparam return_t Return type of the function.
         * @tparam args_t Arguments types of the function.
         * @return true if the function was deleted
         * @return false otherwise.
         */
        template<typename return_t, typename ... args_t>
        static bool erase();

        /**
         * @brief Tells if a function is associated to the stream.
         * 
         * @tparam return_t Return type of the function.
         * @tparam args_t Arguments types of the function.
         * @return true if a function is associated.
         * @return false otherwise.
         */
        template<typename return_t, typename ... args_t>
        static bool exists();

    private:

        template<typename return_t, typename ... args_t>
        struct Caller {
            static constexpr auto stub = [](args_t...) { return return_t(); };
            static inline return_t(*sFunction)(args_t...) = stub;
            static inline bool sIsMutable = true;
        };

    };




    template<auto id>
    template<typename return_t, typename ... args_t>
    bool Channel<id>::setImmutable(return_t(*inFunc)(args_t...)) {

        using c_t = Caller<return_t, args_t...>;

        if ((c_t::sIsMutable || c_t::sFunction == c_t::stub) && inFunc) {
            c_t::sIsMutable = false;
            c_t::sFunction = inFunc;
            return true;
        } else {
            // write attempt on an already set immutable or inFunc is null
            return false;
        }

    }

    template<auto id>
    template<typename return_t, typename ... args_t>
    bool Channel<id>::setMutable(return_t(*inFunc)(args_t...)) {

        using c_t = Caller<return_t, args_t...>;

        if (c_t::sIsMutable && inFunc) {
            c_t::sFunction = inFunc;
            return true;
        }
        // caller is immutable or inFunc is null
        return false;
    }

    template<auto id>
    template<typename return_t, typename ... args_t>
    auto Channel<id>::call(args_t... args) {
        return Caller<return_t, args_t...>::sFunction(args...);
    }

    template<auto id>
    template<typename ... args_t>
    void Channel<id>::call(args_t... args) {
        Caller<void, args_t...>::sFunction(args...);
    }

    template<auto id>
    template<typename return_t, typename ... args_t>
    auto Channel<id>::get() {
        return &Caller<return_t, args_t...>::sFunction;
    }

    template<auto id>
    template<typename return_t, typename ... args_t>
    bool Channel<id>::erase() {

        using c_t = Caller<return_t, args_t...>;

        if(c_t::sIsMutable) {
            c_t::sFunction = c_t::stub;
        }
        return c_t::sIsMutable;
    }

    template<auto id>
    template<typename return_t, typename ... args_t>
    bool Channel<id>::exists() {
        using c_t = Caller<return_t, args_t...>;
        return (c_t::sFunction != c_t::stub);
    }


}
