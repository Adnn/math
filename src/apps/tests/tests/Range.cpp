#include "catch.hpp"

#include <math/Range.h>

using namespace ad::math;


SCENARIO("MultiRange usage")
{
    /// \todo remove the double brace, required befor CWG
    static const std::array<std::array<int, 3>, 16> expected{{
        {{0, 0, 0}},
        {{1, 0, 0}},
        {{0, 1, 0}},
        {{1, 1, 0}},
        {{0, 2, 0}},
        {{1, 2, 0}},
        {{0, 3, 0}},
        {{1, 3, 0}},
        {{0, 0, 1}},
        {{1, 0, 1}},
        {{0, 1, 1}},
        {{1, 1, 1}},
        {{0, 2, 1}},
        {{1, 2, 1}},
        {{0, 3, 1}},
        {{1, 3, 1}}
    }};

    GIVEN("A 3 dimensional MultiRange")
    {
        MultiRange<int, 3> range3{2, 4, 2};

        THEN("It can be used to iterate all dimensions")
        {
            std::size_t id = 0;
            for (auto it = range3.begin();
                 it != range3.end();
                 ++id, ++it)
            {
                REQUIRE(expected[id] == *it);
            }
        }
    }

    WHEN("Using the convenience maker")
    {
        auto range3 = make_multirange(2, 4, 2);  

        THEN("An equivalent MultiRange is instanciated")
        {
            std::size_t id = 0;
            for (auto it = range3.begin();
                 it != range3.end();
                 ++id, ++it)
            {
                REQUIRE(expected[id] == *it);
            }
        }
    }
}
