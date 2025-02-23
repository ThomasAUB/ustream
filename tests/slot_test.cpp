
#include "test_list.h"
#include "doctest.h"
#include "ustream/slot.hpp"
#include "ustream/signal.hpp"


void slotTest() {

    ustream::Slot<int&, float&> ps1(
        +[] (int& a, float& b) {
            a++;
            b += 0.5f;
        }
    );

    ustream::Slot<int&, float&> ps2(
        [] (int& a, float& b) {
            a++;
            b += 0.5f;
        }
    );

    int tv = 0;
    float tf = 0;

    ustream::Slot<int&, float&> ps3(
        [&tv, &tf] (int& a, float& b) {

            // tv = a;
            // tf = b;

            a++;
            b += 0.5f;
        }
    );

    ustream::Signal<int&, float&> sig;

    sig.connect(ps1);
    sig.connect(ps2);
    sig.connect(ps3);

    int i = 55;
    float f = 0.1f;
    sig.emit(i, f);

    //CHECK(i == 58);
    //CHECK(f == 1.6f);

}