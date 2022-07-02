#pragma once

#include <type_traits>


template<auto id>
struct Stream {

    template<typename return_t, typename ... args_t>
    static bool setImmutable(return_t (*inFunc)(args_t...)) {
        bool ioSuccess = false;
        rw<false>(ioSuccess, true, inFunc);
        return ioSuccess;
    }

    template<typename return_t, typename ... args_t>
    static void setMutable(return_t (*inFunc)(args_t...)) {
        bool _;
        rw<true>(_, true, inFunc);
    }

    template<bool is_mutable, typename return_t, typename ... args_t>
    static auto get() {
        bool _;
        return rw<is_mutable, return_t, args_t...>(_);
    }

    template<typename return_t, typename ... args_t>
    static void erase() {
        bool _;
        rw<true, return_t, args_t...>(_, true);
    }

    template<bool is_mutable, typename return_t, typename ... args_t>
    static bool exists() {
        bool ioSuccess = false;
        rw<is_mutable, return_t, args_t...>(ioSuccess);
        return ioSuccess;
    }

private:

    template<bool is_mutable, typename return_t, typename ... args_t>
    static auto rw(bool &ioSuccess, bool inWrite = false, return_t (*inFunc)(args_t...) = nullptr) {

        auto stub = [](args_t...) { return return_t(); };

        using f_t = return_t(*)(args_t...);

        static f_t sFunction = stub;

        ioSuccess = true;

        if (inWrite) {
            if (is_mutable || sFunction == stub) {
                if (inFunc) {
                    sFunction = inFunc;
                } else {
                    sFunction = stub;
                }
            } else {
                // write attempt on an already set immutable
                ioSuccess = false;
            }
        } else { // read
            // true if sFunction has been set
            ioSuccess = (sFunction != stub);
        }

        return &sFunction;
    }

};


template<bool is_mutable, typename return_t, typename... args_t>
struct Socket {

    inline return_t operator()(args_t ... args) const {
        return (*mFunc)(args...);
    }

    bool detach() {
        if constexpr (is_mutable) {
            mFunc = &stub;
        }
        return is_mutable;
    }

    template<auto id>
    void attach() {
        mFunc = Stream<id>::template get<is_mutable, return_t, args_t...>();
    }

    template<auto id>
    bool exists() {
        return  (
                    (mFunc != &stub) &&
                    Stream<id>::template exists<is_mutable, return_t, args_t...>()
                );
    }

private:

    using function_t = return_t(**)(args_t...);

    static constexpr return_t(*_)(args_t...) = [](args_t...) { return return_t(); };
    static inline return_t(*stub)(args_t...) = _;

    function_t mFunc = &stub;

};

template<typename return_t, typename ... args_t>
using ImmutableSocket = Socket<false, return_t, args_t...>;

template<typename return_t, typename ... args_t>
using MutableSocket = Socket<true, return_t, args_t...>;

