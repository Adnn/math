#include "catch.hpp"

#include <math/Angle.h>
#include <math/Canonical.h>
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

        THEN("The underlying data can be accessed and manipulated")
        {
            REQUIRE(circle.data() == value);
            circle.data() += value;
            REQUIRE(circle.data() == 2 * value);
        }

        THEN("Its absolute value is itself, since it is positive")
        {
            REQUIRE(abs(circle) == circle);
        }

        THEN("It can be negated")
        {
            auto negation = -circle;
            REQUIRE(negation.value() == -value);

            THEN("The negated absolute value is the original angle")
            {
                REQUIRE(abs(negation) == circle);
            }
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


SCENARIO("Angles heterogeneous operations.")
{
    GIVEN("An angle in Radian, another in Degree.")
    {
        Radian<double> rad{pi<double>};
        Degree<double> deg{180.};

        THEN("They can be added.")
        {
            REQUIRE(rad + deg == Degree<double>{360.});
            REQUIRE(rad + deg == Radian<double>{2 * pi<double>});

            REQUIRE(deg + rad == Degree<double>{360.});
            REQUIRE(deg + rad == Radian<double>{2 * pi<double>});
        }

        THEN("They can be substracted.")
        {
            REQUIRE(rad - deg == Degree<double>{0.});
            REQUIRE(rad - deg == Radian<double>{0.});

            REQUIRE(deg - rad == Degree<double>{0.});
            REQUIRE(deg - rad == Radian<double>{0.});
        }
    }
    GIVEN("An angle in Radian, another in Turn.")
    {
        Radian<double> rad{pi<double>};
        Turn<double> turn{.5};

        THEN("They can be added.")
        {
            REQUIRE(rad + turn == Turn<double>{1.});
            REQUIRE(rad + turn == Radian<double>{2 * pi<double>});

            REQUIRE(turn + rad == Turn<double>{1.});
            REQUIRE(turn + rad == Radian<double>{2 * pi<double>});
        }

        THEN("They can be substracted.")
        {
            REQUIRE(rad - turn == Turn<double>{0.});
            REQUIRE(rad - turn == Radian<double>{0.});

            REQUIRE(turn - rad == Turn<double>{0.});
            REQUIRE(turn - rad == Radian<double>{0.});
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

        THEN("It can be casted to turns")
        {
            Turn<double> circle_turn = circle_deg.as<Turn>();
            REQUIRE(circle_turn.value() == 1.);

            THEN("Converting back to degrees gives the same value")
            {
                REQUIRE(circle_turn.as<Degree>().value() == 360.0);
            }
        }

        THEN("It can be implicitly converted to radians")
        {
            REQUIRE(std::is_convertible<Degree<double>, Radian<double>>::value);
        }

        THEN("It can*not* be implicitly converted to turns")
        {
            REQUIRE_FALSE(std::is_convertible<Degree<double>, Turn<double>>::value);
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

        THEN("It can be casted to turns")
        {
            Turn<float> half_turn = half_rad.as<Turn>();
            REQUIRE(half_turn.value() == 0.5f);

            THEN("Converting back to degrees gives the same value")
            {
                REQUIRE(half_turn.as<Radian>().value() == pi<float>);
            }
        }


        THEN("It can*not* be implicitly converted to degrees")
        {
            REQUIRE_FALSE(std::is_convertible<Radian<float>, Degree<float>>::value);
        }

        THEN("It can*not* be implicitly converted to turns")
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

    GIVEN("An angle in turns")
    {
        Turn<float> half_turn{0.5f};
        REQUIRE(half_turn.value() == 0.5f);

        THEN("It can be casted to degrees")
        {
            Degree<float> half_deg = half_turn.as<Degree>();
            REQUIRE(half_deg.value() == 180.0f);

            THEN("Converting back to radians gives the same value")
            {
                REQUIRE(half_deg.as<Turn>().value() == 0.5f);
            }
        }

        THEN("It can be casted to radians")
        {
            Radian<float> half_rad = half_turn.as<Radian>();
            REQUIRE(half_rad.value() == pi<float>);

            THEN("Converting back to degrees gives the same value")
            {
                REQUIRE(half_rad.as<Turn>().value() == 0.5f);
            }
        }

        THEN("It can be implicitly converted to radians")
        {
            REQUIRE(std::is_convertible<Turn<float>, Radian<float>>::value);
        }

        THEN("It can*not* be implicitly converted to degrees")
        {
            REQUIRE_FALSE(std::is_convertible<Turn<float>, Degree<float>>::value);
        }

        THEN("It can be output to formatted stream")
        {
            std::ostringstream oss;
            oss << half_turn;

            std::ostringstream expected;
            expected << 0.5 << " turn";

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

    THEN("Literals are available for Turn")
    {
        Turn<double> twoDouble  = 1.0_turn;
        Turn<float> twoFloat    = 1.0_turnf;
        Turn<long double> twoLD = 1.0_turnl;
        Turn<int> twoInt        = 1_turn;

        REQUIRE(twoDouble.value() == 1.0);
        REQUIRE(twoFloat.value()  == 1.0f);
        REQUIRE(twoLD.value()     == 1.0l);
        REQUIRE(twoInt.value()    == 1);
    }
}

SCENARIO("Angle canonical form")
{
    GIVEN("Angles in radians in range ]-pi, +pi]")
    {
        Radian<double> zero{0.};
        Radian<double> halfPi{pi<Radian<double>> / 2.};
        Radian<double> minusHalfPi = -halfPi;

        THEN("They are already reduced")
        {
            REQUIRE(reduce(zero) == zero);
            REQUIRE(reduce(halfPi) == halfPi);
            REQUIRE(reduce(minusHalfPi) == minusHalfPi);

            using Canon = Canonical<Radian<double>>;
            REQUIRE(Canon{zero} == zero);
            REQUIRE(Canon{halfPi} == halfPi);
            REQUIRE(Canon{minusHalfPi} == minusHalfPi);
        }
    }

    GIVEN("Angles in degrees in range ]-180, +180]")
    {
        Degree<double> zero{0.};
        Degree<double> half{180};
        Degree<double> minusHalf{-179.999};

        THEN("They are already reduced")
        {
            REQUIRE(reduce(zero) == zero);
            REQUIRE(reduce(half) == half);
            REQUIRE(reduce(minusHalf) == minusHalf);

            using Canon = Canonical<Degree<double>>;
            REQUIRE(Canon{zero} == zero);
            REQUIRE(Canon{half} == half);
            REQUIRE(Canon{minusHalf} == minusHalf);
        }
    }

    GIVEN("Angles in turns in range ]-0.5, +0.5]")
    {
        Turn<double> zero{0.};
        Turn<double> half{0.5};
        Turn<double> minusHalf{-0.499999};

        THEN("They are already reduced")
        {
            REQUIRE(reduce(zero) == zero);
            REQUIRE(reduce(half) == half);
            REQUIRE(reduce(minusHalf) == minusHalf);

            using Canon = Canonical<Turn<double>>;
            REQUIRE(Canon{zero} == zero);
            REQUIRE(Canon{half} == half);
            REQUIRE(Canon{minusHalf} == minusHalf);
        }
    }

    GIVEN("An angle in radian out of ]-pi, +pi]")
    {
        Radian<double> minusPi = -pi<Radian<double>>;

        Radian<double> reduced{2.};
        Radian<double> expanded = reduced + 4 * pi<Radian<double>>;

        THEN("It can be reduced")
        {
            CHECK(reduce(minusPi) == pi<Radian<double>>);
            CHECK(reduce(expanded) == reduced);

            using Canon = Canonical<Radian<double>>;
            CHECK(Canon{minusPi} == pi<Radian<double>>);
            CHECK(Canon{expanded} == reduced);
        }
    }

    GIVEN("An angle in degrees out of ]-180, +180]")
    {
        Degree<double> minusHalf{-180.};
        Degree<double> full{360.};

        Degree<double> reduced{-155.};
        Degree<double> expanded = reduced - (7 * full);

        THEN("It can be reduced")
        {
            CHECK(reduce(minusHalf) == Degree<double>{180.});
            CHECK(reduce(full) == Degree<double>{0.});
            CHECK(reduce(expanded) == reduced);

            using Canon = Canonical<Degree<double>>;
            CHECK(Canon{minusHalf} == Degree<double>{180.});
            CHECK(Canon{full} == Degree<double>{0.});
            CHECK(Canon{expanded} == reduced);
        }
    }

    GIVEN("An angle in turns out of ]-0.5, +0.5]")
    {
        Turn<double> minusHalf{-0.5};
        Turn<double> full{1.};

        // Unfortunately direct comparison does not work with a lot of different
        // value
        Turn<double> reduced{0.5};
        Turn<double> expanded = reduced - (7 * full);

        THEN("It can be reduced")
        {
            CHECK(reduce(minusHalf) == Turn<double>{0.5});
            CHECK(reduce(full) == Turn<double>{0.});
            CHECK(reduce(expanded).value() == reduced.value());

            using Canon = Canonical<Turn<double>>;
            CHECK(Canon{minusHalf} == Turn<double>{0.5});
            CHECK(Canon{full} == Turn<double>{0.});
            CHECK(Canon{expanded} == reduced);
        }
    }
}
