#pragma once

#include "islot.hpp"
#include <utility>

namespace ustream {

    template<auto address, typename ... args_t>
    bool openSlot(ISlot<args_t...>& s);

    template<typename ... args_t>
    void closeSlot(ISlot<args_t...>& s);

    template<auto address, typename ... args_t>
    void broadcast(args_t&&... args);



    namespace detail {

        template<auto address, typename ... args_t>
        struct Server {

            static void connect(ISlot<args_t...>& s) {
                sSlots.push_front(s);
            }

            static void emit(args_t&&... args) {
                for (auto& s : sSlots) {
                    s.slotInput(std::forward<args_t>(args)...);
                }
            }

        private:
            thread_local inline static ulink::List<ISlot<args_t...>> sSlots;
        };

    }

    template<auto address, typename ... args_t>
    void broadcast(args_t&&... args) {
        detail::Server<address, args_t...>::emit(std::forward<args_t>(args)...);
    }

    template<auto address, typename ... args_t>
    bool open(ISlot<args_t...>& s) {
        if (s.isLinked()) {
            return false;
        }
        detail::Server<address, args_t...>::connect(s);
        return true;
    }

    template<typename ... args_t>
    void close(ISlot<args_t...>& s) {
        s.close();
    }

}