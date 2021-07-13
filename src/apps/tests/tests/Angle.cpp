#include "catch.hpp"

#include <math/Angle.h>
#include <math/Constants.h>

#include <iostream>
#include <sstream>

using namespace ad::math;
using namespace ad::math::angle_literals;

SCENARIO("Angles operations")
{
    GIVEN("A default constructed angle")
    {
        Radian<double> def;

        THEN("It is a null angle")
        {
            REQUIRE(def.value() == 0);
        }
    }

    GIVEN("An angle")
    {
        double value = 2*pi<double>;
        Radian<double> circle{value};

        THEN("Its value can be fetched")
        {
            REQUIRE(circle.value() == value);
        }

        THEN("It can be negated")
        {
            circle = -circle;
            REQUIRE(circle.value() == -value);
        }

        THEN("It can be multiplied by a scalar")
        {
            auto result = 3*circle;
            REQUIRE(result.value() == 3*value);
            REQUIRE((circle*=5.0).value() == 5.0*value); // post mult
        }

        THEN("It can be divided by a scalar")
        {
            auto result = circle/10;
            REQUIRE(result.value() == value/10);
            REQUIRE((result/=0.1).value() == value);
        }

        GIVEN("Another Angle")
        {
            Radian<double> other{value};

            THEN("They can be added")
            {
                auto result = circle + other;
                REQUIRE(result.value() == 2*value);

                other += circle;
                REQUIRE(other == 2*circle);
            }

            THEN("They can be substracted")
            {
                auto result = circle - other;
                REQUIRE(result.value() == 0);

                other -= circle;
                REQUIRE(other == circle-circle);
            }

            THEN("They can be compared")
            {
                REQUIRE(circle == other);
                REQUIRE_FALSE(circle != other);
                REQUIRE(circle != other*2);

                REQUIRE(circle >= other);
                REQUIRE_FALSE(circle > other);
                REQUIRE(circle > other/2);

                REQUIRE(circle <= other);
                REQUIRE_FALSE(circle < other);
                REQUIRE(circle < circle+other);
            }
        }
    }
}

SCENARIO("Angles conversions and io")
{
    GIVEN("An angle in degrees")
    {
        Degree<double> circle_deg{360};
        REQUIRE(circle_deg.value() == 360.0);

        THEN("It can be casted to radians")
        {
            Radian<double> circle_rad = circle_deg.as<Radian>();
            REQUIRE(circle_rad.value() == 2*pi<double>);

            THEN("Converting back to degrees gives the same value")
            {
                REQUIRE(circle_rad.as<Degree>().value() == 360.0);
            }
        }

        THEN("It can be implicitly converted to radians")
        {
            REQUIRE(std::is_convertible<Degree<double>, Radian<double>>::value);
        }

        THEN("It can be output to formatted stream")
        {
            std::ostringstream oss;
            oss << circle_deg;

            REQUIRE(oss.str() == "360 deg");
        }
    }

    GIVEN("An angle in radians")
    {
        Radian<float> half_rad{pi<float>};
        REQUIRE(half_rad.value() == pi<float>);

        THEN("It can be casted to degrees")
        {
            Degree<float> half_deg = half_rad.as<Degree>();
            REQUIRE(half_deg.value() == 180.0f);

            THEN("Converting back to radians gives the same value")
            {
                REQUIRE(half_deg.as<Radian>().value() == pi<float>);
            }
        }

        THEN("It can*not* be implicitly converted to radians")
        {
            REQUIRE_FALSE(std::is_convertible<Radian<float>, Degree<float>>::value);
        }

        THEN("It can be output to formatted stream")
        {
            std::ostringstream oss;
            oss << half_rad;

            std::ostringstream expected;
            expected << pi<float> << " rad";

            REQUIRE(oss.str() == expected.str());
        }
    }
}

SCENARIO("Angle literals")
{
    THEN("Literals are available for Radian")
    {
        Radian<double> twoDouble  = 2.0_rad;
        Radian<float> twoFloat    = 2.0_radf;
        Radian<long double> twoLD = 2.0_radl;
        Radian<int> twoInt        = 2_rad;

        REQUIRE(twoDouble.value() == 2.0);
        REQUIRE(twoFloat.value()  == 2.0f);
        REQUIRE(twoLD.value()     == 2.0l);
        REQUIRE(twoInt.value()    == 2);

        REQUIRE(twoFloat == 2.0_radF);
        REQUIRE(twoLD    == 2.0_radL);
    }

    THEN("Literals are available for Degree")
    {
        Degree<double> twoDouble  = 36.0_deg;
        Degree<float> twoFloat    = 36.0_degf;
        Degree<long double> twoLD = 36.0_degl;
        Degree<int> twoInt        = 36_deg;

        REQUIRE(twoDouble.value() == 36.0);
        REQUIRE(twoFloat.value()  == 36.0f);
        REQUIRE(twoLD.value()     == 36.0l);
        REQUIRE(twoInt.value()    == 36);
    }
}

