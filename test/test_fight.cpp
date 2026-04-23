#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "sim/fight.h"

TEST_CASE("ilvl_factor returns 1.0 at parity") {
    CHECK(ilvl_factor(200.0f, 200.0f) == doctest::Approx(1.0f));
}

TEST_CASE("ilvl_factor clamps at floor when heavily undergeared") {
    // delta=-0.5, raw factor=-0.4, clamped to 0.4
    CHECK(ilvl_factor(100.0f, 200.0f) == doctest::Approx(0.4f));
}

TEST_CASE("ilvl_factor clamps at ceiling when heavily overgeared") {
    // delta=2.0, raw factor=6.6, clamped to 1.25
    CHECK(ilvl_factor(300.0f, 100.0f) == doctest::Approx(1.25f));
}

TEST_CASE("ilvl_factor scales linearly within unclamped range") {
    // delta=0.05, factor = 1.0 + 0.05*2.8 = 1.14
    CHECK(ilvl_factor(210.0f, 200.0f) == doctest::Approx(1.14f));
}
