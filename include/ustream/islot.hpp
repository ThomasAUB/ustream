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

#include "ulink.hpp"
#include "../../ucall/include/ucall.hpp"

namespace ustream {

    /**
     * @brief Slot.
     *
     * @tparam args_t Argument types of the signal.
     */
    template<typename ... args_t>
    struct ISlot : ulink::Node<ISlot<args_t...>> {

        /**
         * @brief Disconnects this slot.
         */
        void disconnect() {
            ulink::Node<ISlot<args_t...>>::remove();
            this->disconnected();
        }

        /**
         * @brief Tells if this slot is connected to a signal or broadcast port.
         *
         * @return true if this slot is connected
         * @return false otherwise.
         */
        bool isConnected() const { return this->isLinked(); }

        /**
         * @brief Called when the slot is connected.
         */
        virtual void connected() {}

        /**
         * @brief Called when the slot is diconnected.
         */
        virtual void disconnected() {}

        /**
         * @brief Handler invocation when a connected signal emits data.
         *
         * The library no longer uses a virtual function for signal delivery.
         * Instead a small, in-place callable is stored (from the ucall submodule)
         * and invoked here. This avoids virtual dispatch and reduces code size
         * on embedded targets.
         */
        void processSignal(args_t... args) {
            if (mHandler) {
                mHandler(std::forward<args_t>(args)...);
            }
        }

        /**
         * @brief Set the handler callable for this slot.
         *
         * Accepts any callable compatible with void(args_t...). The callable is
         * stored in-place using `ucall::Callable` to avoid dynamic allocations.
         */
        template<typename F>
        void setHandler(F&& f) {
            mHandler = ucall::Callable<void(args_t...)>(std::forward<F>(f));
        }

        /**
         * @brief Clear the current handler.
         */
        void clearHandler() { mHandler = ucall::Callable<void(args_t...)>(); }

        /**
         * @brief Tests if a handler is set.
         */
        bool hasHandler() const { return static_cast<bool>(mHandler); }

    private:

        using ulink::Node<ISlot<args_t...>>::remove;

        template<typename T>
        friend class ulink::List;

        ucall::Callable<void(args_t...)> mHandler;

    };

}