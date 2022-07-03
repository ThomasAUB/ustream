#pragma once

#include <type_traits>

namespace ustream {

template<auto id>
struct Channel {

    template<typename return_t, typename ... args_t>
    static bool setImmutable(return_t (*inFunc)(args_t...)) {

        using c_t = Caller<return_t, args_t...>;

        if (c_t::sIsMutable || c_t::sFunction == c_t::stub) {
            c_t::sIsMutable = false;
            c_t::sFunction = inFunc;
            return true;
        } else {
            // write attempt on an already set immutable
            return false;
        }

    }

    template<typename return_t, typename ... args_t>
    static void setMutable(return_t (*inFunc)(args_t...)) {

        using c_t = Caller<return_t, args_t...>;

        if (c_t::sIsMutable) {
            c_t::sFunction = inFunc;
        }
    }

    template<typename return_t, typename ... args_t>
    static auto get() {
        return &Caller<return_t, args_t...>::sFunction;
    }

    template<typename return_t, typename ... args_t>
    static bool erase() {

        using c_t = Caller<return_t, args_t...>;

        if(c_t::sIsMutable) {
            c_t::sFunction = c_t::stub;
        }
        return c_t::sIsMutable;
    }

    template<typename return_t, typename ... args_t>
    static bool exists() {
        using c_t = Caller<return_t, args_t...>;
        return (c_t::sFunction != c_t::stub);
    }

private:

    template<typename return_t, typename ... args_t>
    struct Caller {
        static constexpr auto stub = [](args_t...) { return return_t(); };
        static inline return_t(*sFunction)(args_t...)  = stub;
        static inline bool sIsMutable = true;
    };

};


template<typename return_t, typename... args_t>
struct Socket {

    inline return_t operator()(args_t ... args) const {
        return (*mFunc)(args...);
    }

    template<auto id>
    void attach() {
        mFunc = Channel<id>::template get<return_t, args_t...>();
    }

    template<auto id>
    void detach() {
        mFunc = const_cast<function_t>(&stub);
    }

    template<auto id>
    bool exists() {
        return
            (
                (mFunc != const_cast<function_t>(&stub)) &&
                Channel<id>::template exists<return_t, args_t...>()
            );
    }

private:

    static constexpr return_t(*stub)(args_t...) = [](args_t...) { return return_t(); };

    using function_t = return_t(**)(args_t...);
    function_t mFunc = const_cast<function_t>(&stub);

};

}
