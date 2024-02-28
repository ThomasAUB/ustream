#pragma once

#include <utility>
#include "function_ref.hpp"
#include "ull.h"

namespace ustream {

    // forward declaration
    template<auto address, typename ... args_t>
    struct Server;

    template<typename ... args_t>
    class Slot : public ull::Node<Slot<args_t...>> {
        using f_t = tl::function_ref<void(args_t...)>;
    public:

        Slot(f_t&& f) :
            mFunction(f) {}

        void operator()(args_t&&... args) {
            mFunction(std::forward<args_t>(args)...);
        }

        template<auto address>
        bool open() {
            if (this->isLinked()) {
                return false;
            }
            Server<address, args_t...>::add(*this);
            return true;
        }

        void close() {
            this->remove();
        }

    private:
        f_t mFunction;
    };


    template<auto address, typename ... args_t>
    struct Server {

        void add(Slot<args_t...>& n) {
            sList.push_front(n);
        }

        void broadcast(args_t&... args) {
            for (auto& n : sList) {
                n(std::forward<args_t>(args)...);
            }
        }

    private:
        inline static ull::List<Slot<args_t...>> sList;
    };

}