#pragma once

#include <utility>
#include "function_ref.hpp"
#include "ulink.hpp"

namespace ustream {

    template<typename ... args_t>
    class Slot : public ulink::Node<Slot<args_t...>> {
        using f_t = tl::function_ref<void(args_t...)>;
    public:

        Slot(f_t&& f);

        void operator()(args_t&&... args);

        template<auto address>
        bool open();

        void close();

    private:
        f_t mFunction;
    };

    namespace detail {

        template<auto address, typename ... args_t>
        struct Server {

            static void connect(Slot<args_t...>& n);

            static void emit(args_t&&... args);

        private:
            inline static ulink::List<Slot<args_t...>> sSlots;
        };

    }

    template<typename ... args_t>
    Slot<args_t...>::Slot(f_t&& f) :
        mFunction(f) {}

    template<typename ... args_t>
    void Slot<args_t...>::operator()(args_t&&... args) {
        mFunction(std::forward<args_t>(args)...);
    }

    template<typename ... args_t>
    template<auto address>
    bool Slot<args_t...>::open() {
        if (this->isLinked()) {
            return false;
        }
        detail::Server<address, args_t...>::connect(*this);
        return true;
    }

    template<typename ... args_t>
    void Slot<args_t...>::close() {
        this->remove();
    }

    namespace detail {

        template<auto address, typename ... args_t>
        void Server<address, args_t...>::connect(Slot<args_t...>& s) {
            sSlots.push_front(s);
        }

        template<auto address, typename ... args_t>
        void Server<address, args_t...>::emit(args_t&&... args) {
            for (auto& s : sSlots) {
                s(std::forward<args_t>(args)...);
            }
        }

    }

}