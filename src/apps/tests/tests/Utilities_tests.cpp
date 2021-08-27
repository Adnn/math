#include "catch.hpp"


#include <math/Constants.h>
#include <math/VectorUtilities.h>


using namespace ad::math;


SCENARIO("Vector utilities")
{
    GIVEN("Two vectors a and b")
    {
        Vec<2, double> a{0., 1.};
        Vec<2, double> b{-1., 0.};

        THEN("The oriented angle from a to b can be computed")
        {
            REQUIRE(getOrientedAngle(a, b) == pi<Radian<double>>/2.);
        }

        THEN("The oriented angle from b to a is the negative of the angle from a to b")
        {
            REQUIRE(getOrientedAngle(b, a) == - getOrientedAngle(a, b));
        }
    }
}
