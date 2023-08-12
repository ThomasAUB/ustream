[![Build Status](https://app.travis-ci.com/ThomasAUB/uStream.svg)](https://travis-ci.com/ThomasAUB/uStream)
[![License](https://img.shields.io/github/license/ThomasAUB/uStream.svg)](LICENSE)

# uStream

uStream is a lightweight dependency injection tool meant for microcontrollers without memory allocation.
It uses ID to differentiate streams, a stream can support multiple sources and multiple types.
A destination can be connected and disconnected from a stream at runtime.

## How to

First, you'll have to define at least one stream ID

Example :

```cpp
enum class eMyStreams {
    eStream
};
```

Then you can define free or static functions to receive the streams

```cpp
struct MyArgType1 {/**/};
struct MyArgType2 {/**/};

struct RX1 {

    static bool input(MyArgType1) {
        /**/
    }

};

struct RX2 {

    template<typename T>
    static bool process(const MyArgType1, T&) {
        /**/
    }

};
```

Now it's time to associate this class to the stream somewhere in your code. A mutable stream can be changed afterwards and an immutable can't. You can set several functions on the same stream as long as the prototypes are not the same

```cpp
// set RX1::input as the mutable function called on MyStreams::eStream
ustream::Channel<eMyStreams::eStream>::setMutable(&RX1::input);

// set RX1::input as the immutable function called on MyStreams::eStream
ustream::Channel<eMyStreams::eStream>::setImmutable(&RX2::process<MyArgType2>);
```

Now, you can start to use the stream

```cpp
#include "ustream.h"

int main() {

    // call the stream that takes 'MyArgType1' in argument and returns bool : calls RX1::input function
    bool result1 = ustream::Channel<eMyStreams::eStream>::call<bool, MyArgType1>(MyArgType1());


    MyArgType2 arg;

    // call the stream that takes 'MyArgType1' and 'MyArgType2&' in argument and returns bool : calls RX2::process<MyArgType2> function
    bool result2 = ustream::Channel<eMyStreams::eStream>::call<bool, MyArgType1, MyArgType2&>(MyArgType1(), arg);


    return 0;
}
```

Note that if there is no matching function associated to a stream, the call will fall into an empty function that will return the default value of the return type.

It's also possible to send callbacks on a stream.

```cpp
static void asyncProcess(int i, void(*inCallback)(int)) {
    /*...*/
    inCallback(i * 10); 
}

int main() {

    enum class eMyStreams {
        eStream
    };

    // attach the the function to the stream
    ustream::Channel<eMyStreams::eStream>::setMutable(asyncProcess);

    // call the stream with a lambda expression
    ustream::Channel<eMyStreams::eStream>::call<void, int, void(*)(int)>(
        5, 
        [](int inResult) {
            
            if(inResult == 50) {
                std::cout << "received 50 !" << std::endl;
            }

        }
    );

    return 0;
}
```

uStream also provides a **Socket** object to be used when the stream ID is not known in the scope of usage. 

```cpp
#include "socket.h"

static bool input(MyArgType1) {
    std::cout << "Arg received" << std::endl;
    return true;
}


auto getSocket() {
    
    enum class eMyStreams {
        eStream
    };

    // attach the stream to the input function
    ustream::Channel<eMyStreams::eStream>::setMutable(input);

    // instantiate a socket returning bool and taking MyArgType1 in argument
    ustream::Socket<bool(MyArgType1)> socket;
    
    // attach the socket to eMyStreams::eStream
    socket.attach<eMyStreams::eStream>();

    return socket;
}

int main() {

    auto socket = getSocket();

    bool result = socket(MyArgType1());

    std::cout << result << std::endl;
    
    return 0;
}
```


It's possible to change the channel function at runtime if declared as mutable
```cpp
#include "ustream.h"

static void receive2(int i) {
    /*...*/
}

static void receive1(int i) {
    // redirect the stream to the 'receive2' function
    ustream::Channel<eMyStreams::eStream>::setMutable(receive2);
}

void init() {
    // attach a function tot the stream
    ustream::Channel<eMyStreams::eStream>::setMutable(receive1);
}

int main() {

    init();

    // calls the "receive1" function
    ustream::Channel<eMyStreams::eStream>::call(456);
    
    // calls the "receive2" function
    ustream::Channel<eMyStreams::eStream>::call(456);
}
```
