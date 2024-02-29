#pragma once

#include "ustream_slot.hpp"
#include <utility>

namespace ustream {

    template<auto address, typename ... args_t>
    void broadcast(args_t&&... args) {
        detail::Server<address, args_t...>::emit(std::forward<args_t>(args)...);
    }

}