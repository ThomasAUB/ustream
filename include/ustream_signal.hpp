#pragma once

#include "ustream_slot.hpp"
#include "ull.h"

namespace ustream {

    template<typename ... args_t>
    struct Signal {

        void connect(Slot<args_t...>& inSlot) {
            mList.push_front(inSlot);
        }

        void emit(args_t&& ... args) {
            for (auto& s : mList) {
                s(std::forward<args_t>(args)...);
            }
        }

    private:
        ull::List<Slot<args_t...>> mList;
    };

}