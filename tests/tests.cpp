#include <iostream>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "ustream_slot.hpp"
#include "ustream_signal.hpp"
#include "ustream_broadcast.hpp"

TEST_CASE("basic uStream tests") {

    ustream::Signal<int> sig;

    int receivedData1 = 0;
    ustream::Slot<int> slot1(
        [&] (int i) {
            receivedData1 = i;
        }
    );

    sig.emit(42);
    CHECK(receivedData1 == 0);
    sig.connect(slot1);

    sig.emit(42);
    CHECK(receivedData1 == 42);

    int receivedData2 = 0;
    ustream::Slot<int> slot2(
        [&] (int i) {
            receivedData2 = i;
        }
    );

    sig.connect(slot2);

    sig.emit(75);
    CHECK(receivedData1 == 75);
    CHECK(receivedData2 == 75);

    slot1.close();
    sig.emit(753);
    CHECK(receivedData1 == 75);
    CHECK(receivedData2 == 753);

    int receivedData3 = 0;
    {
        ustream::Slot<int> slot3(
            [&] (int i) {
                receivedData3 = i;
            }
        );

        sig.connect(slot3);

        sig.emit(789);
        CHECK(receivedData1 == 75);
        CHECK(receivedData2 == 789);
        CHECK(receivedData3 == 789);
    }

    sig.emit(951);
    CHECK(receivedData1 == 75);
    CHECK(receivedData2 == 951);
    CHECK(receivedData3 == 789);

    slot1.template open<44>();

    ustream::broadcast<44>(12);

    CHECK(receivedData1 == 12);
    CHECK(receivedData2 == 951);
    CHECK(receivedData3 == 789);

    // the slot shouldn't open as it's already connected to a signal
    CHECK(!slot2.template open<44>());

    ustream::broadcast<44>(452);

    CHECK(receivedData1 == 452);
    CHECK(receivedData2 == 951);
    CHECK(receivedData3 == 789);

    slot2.close();
    CHECK(slot2.template open<44>());

    ustream::broadcast<44>(956);

    CHECK(receivedData1 == 956);
    CHECK(receivedData2 == 956);
    CHECK(receivedData3 == 789);

    // this should have no effect
    sig.emit(123);

    CHECK(receivedData1 == 956);
    CHECK(receivedData2 == 956);
    CHECK(receivedData3 == 789);

}

