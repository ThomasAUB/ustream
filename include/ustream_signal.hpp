#pragma once

#include "ustream_slot.hpp"
#include "ulink.hpp"

namespace ustream {

    template<typename ... args_t>
    struct Signal {

        void connect(Slot<args_t...>& inSlot);

        void emit(args_t&& ... args);

    private:
        ulink::List<Slot<args_t...>> mSlots;
    };


    template<typename ... args_t>
    void Signal<args_t...>::connect(Slot<args_t...>& inSlot) {
        mSlots.push_front(inSlot);
    }

    template<typename ... args_t>
    void Signal<args_t...>::emit(args_t&& ... args) {
        for (auto& s : mSlots) {
            s(std::forward<args_t>(args)...);
        }
    }

}