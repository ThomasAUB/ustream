![build status](https://github.com/ThomasAUB/uStream/actions/workflows/cmake-multi-platform.yml/badge.svg) [![License](https://img.shields.io/github/license/ThomasAUB/uStream.svg)](LICENSE)

# uStream

Lightweight signal-slot library

This library can operate in two different modes, connected and broadcast.

## Connected mode

```cpp
#include "ustream/signal.hpp"
#include "ustream/islot.hpp"

// slot definition
struct Slot : ustream::ISlot<int> {

    Slot(int inID) : mID(inID) {}

    void slotInput(int i) override {
        std::cout << i << " received in slot " << mID << std::endl;
    }

private:
    int mID;
};

int main() {

    // declare a signal
    ustream::Signal<int> signal;

    // declare slots
    Slot slot1(1);
    Slot slot2(2);
    Slot slot3(3);

    // connect the slots to the signal
    signal.connect(slot1);
    signal.connect(slot2);
    signal.connect(slot3);

    // emit a value
    signal.emit(25);

    return 0;
}
```

Result :

```
25 received in slot 3
25 received in slot 2
25 received in slot 1
```

## Broadcast mode

```cpp
#include "ustream/islot.hpp"
#include "ustream/broadcast.hpp"

// slot definition
struct Slot : ustream::ISlot<int> {

    Slot(int inID) : mID(inID) {}

    void slotInput(int i) override {
        std::cout << i << " received in slot " << mID << std::endl;
    }

private:
    int mID;
};

int main() {

    // declare slots
    Slot slot1(1);
    Slot slot2(2);
    Slot slot3(3);

    enum class ePorts {
        A,
        B
    };

    // open slot1 on port A
    ustream::open<ePorts::A>(slot1);

    // open slot2 and slot3 on port B
    ustream::open<ePorts::B>(slot2);
    ustream::open<ePorts::B>(slot3);

    ustream::broadcast<A>(74); // emit to slot1

    ustream::broadcast<B>(12); // emit to slot2 and slot3

    return 0;
}
```

Result :

```
74 received in slot 1
12 received in slot 3
12 received in slot 2
```

As the values are passed by reference, it's possible to modify the value on the fly.

```cpp
#include "ustream/islot.hpp"
#include "ustream/broadcast.hpp"

// slot definition
struct Slot : ustream::ISlot<int&> {
    void slotInput(int& i) override {
        std::cout << i++ << std::endl;
    }
};

int main() {

    // declare slots
    Slot slot1;
    Slot slot2;
    Slot slot3;

    ustream::open<0>(slot1);
    ustream::open<0>(slot2);
    ustream::open<0>(slot3);

    int i = 1;
    ustream::broadcast<0, int&>(i); // prints 1, 2, 3

    slot1.disconnect();

    ustream::broadcast<0, int&>(i); // prints 4, 5

    return 0;
}
```

If a slot that has been connected is deleted, it will automatically remove itself
from its signal.

```cpp
void foo() {
    Slot s;
    someSignal.connect(s);
} // the slot disconnects itself here
```

### Limitations

A slot can be connected to only one source whether it be a signal or a broadcast address. Here are the design choices that were made:

- If a slot is connected to a signal or a broadcast address, connecting it to another **signal** will disconnect it from the previous source.

- If a slot is connected to a signal or a broadcast address, connecting it to another **broadcast address** will fail and return false.
