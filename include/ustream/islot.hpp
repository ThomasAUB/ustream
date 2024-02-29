#pragma once

#include "ulink.hpp"

namespace ustream {

    template<typename ... args_t>
    struct ISlot : ulink::Node<ISlot<args_t...>> {

        void close() { this->remove(); }

        virtual void slotInput(args_t... args) = 0;

    };

}