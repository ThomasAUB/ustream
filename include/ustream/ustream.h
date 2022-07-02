#pragma once

#include <type_traits>

namespace ustream {

template<auto id>
struct Channel {

    template<typename return_t, typename ... args_t>
    static bool setImmutable(return_t (*inFunc)(args_t...)) {
        return FuncContainer<return_t, args_t...>::set(inFunc, false);
    }

    template<typename return_t, typename ... args_t>
    static void setMutable(return_t (*inFunc)(args_t...)) {
        FuncContainer<return_t, args_t...>::set(inFunc, true);
    }

    template<typename return_t, typename ... args_t>
    static auto get() {
        return FuncContainer<return_t, args_t...>::get();
    }

    template<typename return_t, typename ... args_t>
    static bool erase() {
        return FuncContainer<return_t, args_t...>::erase();
    }

    template<typename return_t, typename ... args_t>
    static bool exists() {
        return FuncContainer<return_t, args_t...>::exists();
    }

private:

    template<typename return_t, typename ... args_t>
    struct FuncContainer {

        static constexpr auto stub = [](args_t...) { return return_t(); };

        using f_t = return_t(*)(args_t...);

        static inline f_t sFunction = stub;

        static bool set(f_t inFunc, bool inIsMutable) {

            if (sIsMutable || sFunction == stub) {
                sIsMutable = inIsMutable;
                sFunction = inFunc;
                return true;
            } else {
                // write attempt on an already set immutable
                return false;
            }
        }

        static bool erase() {
            if(sIsMutable) {
                sFunction = stub;
            }
            return sIsMutable;
        }

        static auto get() {
            return &sFunction;
        }

        static bool exists() {
            return (sFunction != stub);
        }

    private:

        static inline bool sIsMutable;

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
