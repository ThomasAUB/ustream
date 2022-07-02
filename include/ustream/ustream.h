#pragma once

#include <type_traits>

namespace ustream {

template<auto id>
struct Channel {

    template<typename return_t, typename ... args_t>
    static bool setImmutable(return_t (*inFunc)(args_t...)) {
        return FuncContainer<false, return_t, args_t...>::write(inFunc);
    }

    template<typename return_t, typename ... args_t>
    static void setMutable(return_t (*inFunc)(args_t...)) {
        FuncContainer<true, return_t, args_t...>::write(inFunc);
    }

    template<bool is_mutable, typename return_t, typename ... args_t>
    static auto get() {
        return FuncContainer<is_mutable, return_t, args_t...>::read();
    }

    template<typename return_t, typename ... args_t>
    static void erase() {
        FuncContainer<true, return_t, args_t...>::write();
    }

    template<bool is_mutable, typename return_t, typename ... args_t>
    static bool exists() {
        return FuncContainer<is_mutable, return_t, args_t...>::exists();
    }

private:

    template<bool is_mutable, typename return_t, typename ... args_t>
    struct FuncContainer {

        static constexpr auto stub = [](args_t...) { return return_t(); };

        using f_t = return_t(*)(args_t...);

        static inline f_t sFunction = stub;

        static bool write(f_t inFunc = nullptr) {

            if (is_mutable || sFunction == stub) {
                if (inFunc) {
                    sFunction = inFunc;
                } else {
                    sFunction = stub;
                }
                return true;
            } else {
                // write attempt on an already set immutable
                return false;
            }

        }

        static auto read() {
            return &sFunction;
        }

        static bool exists() {
            return (sFunction != stub);
        }

    };


};


template<bool is_mutable, typename return_t, typename... args_t>
struct Socket {

    inline return_t operator()(args_t ... args) const {
        return (*mFunc)(args...);
    }

    template<auto id>
    void attach() {
        mFunc = Channel<id>::template get<is_mutable, return_t, args_t...>();
    }

    template<auto id>
    bool detach() {
        if(is_mutable) {
            mFunc = const_cast<function_t>(&stub);
        }
        return is_mutable;
    }

    template<auto id>
    bool exists() {
        return
            (
                (mFunc != const_cast<function_t>(&stub)) &&
                Channel<id>::template exists<is_mutable, return_t, args_t...>()
            );
    }

private:

    static constexpr return_t(*stub)(args_t...) = [](args_t...) { return return_t(); };

    using function_t = return_t(**)(args_t...);
    function_t mFunc = const_cast<function_t>(&stub);

};

template<typename return_t, typename ... args_t>
using ImmutableSocket = Socket<false, return_t, args_t...>;

template<typename return_t, typename ... args_t>
using MutableSocket = Socket<true, return_t, args_t...>;

}
