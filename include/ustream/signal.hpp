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

#include "islot.hpp"

namespace ustream {

    /**
     * @brief Signal.
     *
     * @tparam args_t Argument types of the signal.
     */
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
        void emit(args_t... args);

        /**
         * @brief Tells if this signal is connected to at least one slot.
         *
         * @return true if this signal is connected
         * @return false otherwise.
         */
        bool isConnected() const;

        /**
         * @brief Disconnects all the connected slots.
         */
        ~Signal();

    protected:
        ulink::List<ISlot<args_t...>> mSlots;

    private:

        // Placeholder linked in the slot list during emit, right after the
        // slot being processed. Slots disconnected or destroyed by a
        // processSignal call unlink themselves around it, so the next slot
        // to process is always cursor.next.
        struct Cursor : ISlot<args_t...> {
            Cursor(std::size_t& inCount) : mCount(inCount) { mCount++; }
            ~Cursor() { mCount--; }
            void processSignal(args_t...) override {}
        private:
            std::size_t& mCount;
        };

        // number of cursors currently linked in mSlots (nested emits)
        std::size_t mCursorCount = 0;
    };


    template<typename ... args_t>
    bool Signal<args_t...>::connect(ISlot<args_t...>& inSlot) {
        if (inSlot.isLinked()) {
            return false;
        }
        mSlots.push_front(inSlot);
        inSlot.connected();
        return true;
    }

    // the cursor is a local linked in mSlots, it always unlinks itself when
    // it goes out of scope
#if defined(__GNUC__) && !defined(__clang__) && (__GNUC__ >= 12)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdangling-pointer"
#endif

    template<typename ... args_t>
    void Signal<args_t...>::emit(args_t ... args) {

        using iterator = typename ulink::List<ISlot<args_t...>>::iterator;

        // slots connected during emit are pushed front, before the cursor,
        // so they are not processed by this emit
        Cursor cursor(mCursorCount);
        mSlots.push_front(cursor);

        iterator it(&cursor);

        while (++it != mSlots.end()) {
            auto& s = *it;
            mSlots.insert_after(it, cursor);
            // cursors of nested emits are processed as no-op
            s.processSignal(args...);
            it = iterator(&cursor);
        }
    }

#if defined(__GNUC__) && !defined(__clang__) && (__GNUC__ >= 12)
#pragma GCC diagnostic pop
#endif

    template<typename ... args_t>
    bool Signal<args_t...>::isConnected() const {
        // true if the list holds more nodes than the emit cursors
        std::size_t count = 0;
        for (auto it = mSlots.begin(); it != mSlots.end(); ++it) {
            if (++count > mCursorCount) {
                return true;
            }
        }
        return false;
    }

    template<typename ... args_t>
    Signal<args_t...>::~Signal() {
        while (!mSlots.empty()) {
            mSlots.front().disconnect();
        }
    }

}
