#include "catch.hpp"

#include <math/Interpolation/Interpolation.h>
#include <math/Vector.h>


using namespace ad::math;


SCENARIO("Clamped type")
{
    GIVEN("A few Clamped doubles.")
    {
        Clamped<> a{-5};
        Clamped<> b{0.5};
        Clamped<> c{1.5};

        THEN("They are clamped as expected")
        {
            REQUIRE(a == 0.);
            REQUIRE(b == 0.5);
            REQUIRE(c == 1.);
        }
    }

    GIVEN("A few Clamped integers")
    {
        using Clamp_t = Clamped<int, 3, 10>;
        Clamp_t a{-5};
        Clamp_t b{4};
        Clamp_t c{11};

        THEN("They are clamped as expected")
        {
            REQUIRE(a == 3);
            REQUIRE(b == 4);
            REQUIRE(c == 10);
        }
    }
}


SCENARIO("Linear interpolation")
{
    GIVEN("Two vectors")
    {
        Vec<2> a{10., 10.};
        Vec<2> b{20., 40.};

        THEN("They can lerp")
        {
            REQUIRE(lerp(a, b,  0.) == a);
            REQUIRE(lerp(a, b,  1.) == b);
            REQUIRE(lerp(a, b,  3.) == b);
            REQUIRE(lerp(a, b, -1.) == a);

            REQUIRE(lerp(a, b,  0.5) == Vec<2>{15., 25.});
        }


        THEN("They can unbound (extrapolations")
        {
            REQUIRE(lerpUnbound(a, b,  0.) == a);
            REQUIRE(lerpUnbound(a, b,  1.) == b);
            REQUIRE(lerpUnbound(a, b,  3.) == Vec<2>{40., 100.});
            REQUIRE(lerpUnbound(a, b, -1.) == Vec<2>{ 0., -20.});

            REQUIRE(lerpUnbound(a, b,  0.5) == Vec<2>{15., 25.});
        }
    }
}


SCENARIO("Interpolation class")
{
    GIVEN("A linear and a smoothstep Interpolations betwen two vectors")
    {
        Vec<2> a{10., 10.};
        Vec<2> b{20., 40.};

        double duration = 10;
        auto linear = makeInterpolation(a, b, duration);
        auto smoothstep = makeInterpolation<None ,ease::SmoothStep>(a, b, duration);

        WHEN("The interpolations advances")
        {
            Vec<2> interpolatedHalf = linear.advance(duration/2.);
            // Note that this advance will be on top of previous advance
            Vec<2> interpolatedThreeQuarter = linear.advance(duration/4.);

            Vec<2> smoothHalf = smoothstep.advance(duration/2.);
            // Note that this advance will be on top of previous advance
            Vec<2> smoothThreeQuarter = smoothstep.advance(duration/4.);

            REQUIRE_FALSE(linear.isCompleted());
            REQUIRE_FALSE(smoothstep.isCompleted());
            REQUIRE(linear.getOvershoot() == 0.);
            REQUIRE(smoothstep.getOvershoot() == 0.);

            THEN("They can be reset")
            {
                linear.reset();
                REQUIRE(linear.advance(0.) == a);
                smoothstep.reset();
                REQUIRE(smoothstep.advance(0.) == a);
            }

            THEN("The result is interpolated")
            {
                REQUIRE(interpolatedHalf         == a + 1./2. * (b - a));
                REQUIRE(interpolatedThreeQuarter == a + 3./4. * (b - a));

                REQUIRE(interpolatedHalf         == smoothHalf);
                REQUIRE(interpolatedThreeQuarter.x() <  smoothThreeQuarter.x());
                REQUIRE(interpolatedThreeQuarter.y() <  smoothThreeQuarter.y());
            }

            WHEN("They are advanced to or past the end.")
            {
                linear.advance(duration/4.);
                smoothstep.advance(duration);

                THEN("They indicate completion")
                {
                    REQUIRE(linear.isCompleted());
                    REQUIRE(linear.getOvershoot() == 0.);
                    REQUIRE(smoothstep.isCompleted());
                    REQUIRE(smoothstep.getOvershoot() == 3./4. * duration);

                    THEN("They can be reset")
                    {
                        linear.reset();
                        REQUIRE_FALSE(linear.isCompleted());
                        REQUIRE(linear.advance(0.) == a);
                        smoothstep.reset();
                        REQUIRE_FALSE(smoothstep.isCompleted());
                        REQUIRE(smoothstep.advance(0.) == a);
                    }
                }
            }
        }
    }
}
