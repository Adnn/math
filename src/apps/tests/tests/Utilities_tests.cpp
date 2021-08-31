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

    GIVEN("An aspect ratio and a matching size")
    {
        double ratio = 16./9.;
        Size<2, double> expected{1920., 1080.};

        THEN("A size can be obtained from a height and this ratio")
        {
            REQUIRE(makeSizeFromHeight(1080., ratio) == expected);
        }

        THEN("A size can be obtained from a height and this ratio")
        {
            REQUIRE(makeSizeFromWidth(1920., ratio) == expected);
        }

        THEN("The ratio can be found from the size")
        {
            REQUIRE(getRatio<double>(expected) == ratio);
        }
    }
}
