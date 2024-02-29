#pragma once

#include "islot.hpp"

namespace ustream {

    template<typename ... args_t>
    struct Signal {

        void connect(ISlot<args_t...>& inSlot);

        void emit(args_t&& ... args);

    private:
        ulink::List<ISlot<args_t...>> mSlots;
    };


    template<typename ... args_t>
    void Signal<args_t...>::connect(ISlot<args_t...>& inSlot) {
        mSlots.push_front(inSlot);
    }

    template<typename ... args_t>
    void Signal<args_t...>::emit(args_t&& ... args) {
        for (auto& s : mSlots) {
            s.slotInput(std::forward<args_t>(args)...);
        }
    }

}