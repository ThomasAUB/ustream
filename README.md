# uStream

uStream is a lightweight dependency injection tool meant for microcontrollers.
It uses ID to differentiate streams, a stream can support multiple sources and multiple types.
A destination can be connected and disconnected from a stream.

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

Now it's time to associate this class to the stream somewhere in your code. A mutable stream can be changed afterwards and an immutable can't. You can set several function on the same stream as long as the prototype is not the same

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

    // instantiate a socket returning bool and taking MyArgType1 in argument
    ustream::Socket<bool(MyArgType1)> socket1;

    // attach the socket to eMyStreams::eStream
    socket1.attach<eMyStreams::eStream>();

    // send data on the stream
    socket1(MyArgType1());

    // instantiate a socket returning bool and taking const MyArgType1 and 
    // MyArgType2& in argument
    ustream::Socket<bool(const MyArgType1, MyArgType2&)> socket2;

    // attach the socket to eMyStreams::eStream
    socket2.attach<eMyStreams::eStream>();

    MyArgType2 arg;

    // send data on the stream
    socket2(MyArgType1(), arg);

    return 0;
}
```
