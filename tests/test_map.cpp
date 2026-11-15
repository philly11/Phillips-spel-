#include <doctest/doctest.h>
#include "map.h"

//                            x     y     z     w     h     d
static Map MakeMap() { return Map({{0.0f, 0.0f, 0.0f, 2.0f, 1.0f, 2.0f}}); }

TEST_CASE("open space has ground height 0")
{
    CHECK(MakeMap().GetGroundHeight(10.0f, 10.0f, 1.0f) == doctest::Approx(0.0f));
}

TEST_CASE("a box is ground when feet are within step height of its hop")
{
    CHECK(MakeMap().GetGroundHeight(0.0f, 0.0f, 1.0f) == doctest::Approx(1.0f));
}

TEST_CASE("a tall box is not ground when feet are too high")
{
    CHECK(MakeMap().GetGroundHeight(0.0f, 0.0f, 0.0f) == doctest::Approx(0.0f));
}

TEST_CASE("wall overlap is positive when pressed into a tall box")
{
    CHECK(MakeMap().WallOverlap(1.2f, 0.0f, 0.0f, 0.3f) > 0.0f);
}

TEST_CASE("a box can step onto is not a wall")
{
    Map low({{0.0f, 0.0f, 0.0f, 2.0f, 0.25f, 2.0f}});
    CHECK(low.WallOverlap(0.0f, 0.0f, 0.0f, 0.3f) == doctest::Approx(0.0f));
}