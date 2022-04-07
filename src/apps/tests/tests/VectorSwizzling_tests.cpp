#include "catch.hpp"

#include <math/Vector.h>


using namespace ad;
using namespace ad::math;


SCENARIO("Vector swizzling.")
{
    GIVEN("A vector of dimension 4")
    {
        const Vec<4, int> v4{1, 2, 3, 4};

        THEN("It can be used to construct a vector of dimension 3")
        {
            const Vec<3, int> v3{v4};
            REQUIRE(v3.x() == v4.x());
            REQUIRE(v3.y() == v4.y());
            REQUIRE(v3.z() == v4.z());
        }

        THEN("It can be used to construct a vector of dimension 2")
        {
            const Vec<2, int> v2{v4};
            REQUIRE(v2.x() == v4.x());
            REQUIRE(v2.y() == v4.y());
        }

        THEN("It can be used to construct a vector of dimension 1")
        {
            const Vec<1, int> v1{v4};
            REQUIRE(v1.x() == v4.x());
        }
    }

    GIVEN("A position of dimension 5")
    {
        const Position<5> p5{1., 2., 3., 4., 5.};

        THEN("It can be swizzled to lower dimensions.")
        {
            REQUIRE(p5.xyzw() == Position<4>{p5});
            REQUIRE(p5.xyz()  == Position<3>{p5});
            REQUIRE(p5.xy()   == Position<2>{p5});
        }
    }

    GIVEN("A vector of dimension 5")
    {
        const Vec<5> v5{1., 2., 3., 4., 5.};

        THEN("It can be swizzled to lower dimensions.")
        {
            REQUIRE(v5.xyzw() == Vec<4>{v5});
            REQUIRE(v5.xyz()  == Vec<3>{v5});
            REQUIRE(v5.xy()   == Vec<2>{v5});
        }
    }
}
