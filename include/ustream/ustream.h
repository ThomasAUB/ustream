#pragma once


namespace ustream {

template<auto id>
struct Channel {

    template<typename return_t, typename ... args_t>
    static bool setImmutable(return_t (*inFunc)(args_t...));

    template<typename return_t, typename ... args_t>
    static bool setMutable(return_t (*inFunc)(args_t...));

    template<typename return_t, typename ... args_t>
    static auto get();

    template<typename return_t, typename ... args_t>
    static bool erase();

    template<typename return_t, typename ... args_t>
    static bool exists();

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

    inline return_t operator()(args_t ... args) const;

    template<auto id>
    void attach();

    template<auto id>
    void detach();

    template<auto id>
    bool exists();

private:

    static constexpr return_t(*stub)(args_t...) = [](args_t...) { return return_t(); };

    using function_t = return_t(**)(args_t...);
    function_t mFunc = const_cast<function_t>(&stub);

};





template<auto id>
template<typename return_t, typename ... args_t>
bool Channel<id>::setImmutable(return_t (*inFunc)(args_t...)) {

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

template<auto id>
template<typename return_t, typename ... args_t>
bool Channel<id>::setMutable(return_t (*inFunc)(args_t...)) {

    using c_t = Caller<return_t, args_t...>;

    if (c_t::sIsMutable) {
        c_t::sFunction = inFunc;
    }
    return c_t::sIsMutable;
}

template<auto id>
template<typename return_t, typename ... args_t>
auto Channel<id>::get() {
    return &Caller<return_t, args_t...>::sFunction;
}

template<auto id>
template<typename return_t, typename ... args_t>
bool Channel<id>::erase() {

    using c_t = Caller<return_t, args_t...>;

    if(c_t::sIsMutable) {
        c_t::sFunction = c_t::stub;
    }
    return c_t::sIsMutable;
}

template<auto id>
template<typename return_t, typename ... args_t>
bool Channel<id>::exists() {
    using c_t = Caller<return_t, args_t...>;
    return (c_t::sFunction != c_t::stub);
}



template<typename return_t, typename... args_t>
return_t Socket<return_t, args_t...>::operator()(args_t ... args) const {
    return (*mFunc)(args...);
}

template<typename return_t, typename... args_t>
template<auto id>
void Socket<return_t, args_t...>::attach() {
    mFunc = Channel<id>::template get<return_t, args_t...>();
}

template<typename return_t, typename... args_t>
template<auto id>
void Socket<return_t, args_t...>::detach() {
    mFunc = const_cast<function_t>(&stub);
}

template<typename return_t, typename... args_t>
template<auto id>
bool Socket<return_t, args_t...>::exists() {
    return
        (
            (mFunc != const_cast<function_t>(&stub)) &&
            Channel<id>::template exists<return_t, args_t...>()
        );
}




}
