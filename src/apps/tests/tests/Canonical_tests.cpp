#include "catch.hpp"

#include <math/Angle.h>
#include <math/Canonical.h>

using namespace ad::math;

SCENARIO("Canonical Angle operations")
{
    using Radian = Radian<double>;
    GIVEN("An angle")
    {
        Radian threePi{pi<double>*3.};

        THEN("A canonical angle can be obtained")
        {
            Canonical<Radian> canon{threePi};

            THEN("It is equal to the reduced source angle")
            {
                // Comparison can occur in any order
                REQUIRE(canon == reduce(threePi));
                REQUIRE(reduce(threePi) == canon);
                REQUIRE(canon == canon);

                REQUIRE(canon < threePi);
                REQUIRE(threePi > canon);
                REQUIRE(canon >= canon);
                REQUIRE(canon <= canon);
            }
        }
    }
}