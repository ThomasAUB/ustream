#pragma once

#include <utility>
#include "ulink.hpp"

namespace ustream {

    template<typename ... args_t>
    struct ISlot : ulink::Node<ISlot<args_t...>> {

        template<auto address>
        bool open();

        void close();

        virtual void slotInput(args_t... args) = 0;

    };

    namespace detail {

        template<auto address, typename ... args_t>
        struct Server {

            static void connect(ISlot<args_t...>& n);

            static void emit(args_t&&... args);

        private:
            inline static ulink::List<ISlot<args_t...>> sSlots;
        };

    }

    template<typename ... args_t>
    template<auto address>
    bool ISlot<args_t...>::open() {
        if (this->isLinked()) {
            return false;
        }
        detail::Server<address, args_t...>::connect(*this);
        return true;
    }

    template<typename ... args_t>
    void ISlot<args_t...>::close() {
        this->remove();
    }

    namespace detail {

        template<auto address, typename ... args_t>
        void Server<address, args_t...>::connect(ISlot<args_t...>& s) {
            sSlots.push_front(s);
        }

        template<auto address, typename ... args_t>
        void Server<address, args_t...>::emit(args_t&&... args) {
            for (auto& s : sSlots) {
                s.slotInput(std::forward<args_t>(args)...);
            }
        }

    }

}