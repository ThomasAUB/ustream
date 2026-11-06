/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MIT License                                                                     *
 *                                                                                 *
 * Copyright (c) 2025 Thomas AUBERT                                                *
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

#include <utility>
#include <functional>
#include <type_traits>
#include "islot.hpp"

namespace ustream {

    /**
     *@brief Slot implemenation with lambda or free function.
     *
     * @tparam args_t Argument types of the signal.
     */
    template<typename ... args_t>
    struct Slot : ISlot<args_t...> {

        Slot() = delete;

        /**
         *@brief Construct a new slot with a free function.
         *
         * @param inF Function pointer.
         */
         //Slot(void(*inF)(args_t...));

         /**
          *@brief Construct a new slot with a lambda.
          *
          * @tparam F Lambda type.
          * @param inF Lambda.
          */
        template <typename F>
        Slot(F&& inF);

    private:
        // No override of processSignal: ISlot now holds a callable. The Slot
        // constructor simply installs the provided callable into the base.
    };
    template<typename ... args_t>
    template <typename F>
    Slot<args_t...>::Slot(F&& inF) {
        // Install the provided callable into the base ISlot handler storage.
        this->setHandler(std::forward<F>(inF));
    }

}