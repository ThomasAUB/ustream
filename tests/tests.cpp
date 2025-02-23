#include <iostream>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "test_list.h"

#include "ustream/islot.hpp"
#include "ustream/signal.hpp"
#include "ustream/broadcast.hpp"
#include "ustream/slot.hpp"


TEST_CASE("uStream tests") {

    connectedTest();

    broadcastTest();

    slotTest();

}

