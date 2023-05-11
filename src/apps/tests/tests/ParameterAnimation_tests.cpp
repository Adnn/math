#include "catch.hpp"

#include <math/Interpolation/ParameterAnimation.h>
#include <math/Vector.h>


using namespace ad::math;


// Note: This is the test that was implemented before ParameterAnimation
// was generalized with full range output and periodicity.
SCENARIO("ParameterAnimation class")
{
    GIVEN("A linear clamped ParameterAnimation and a duration")
    {
        float duration = 100.;
        ParameterAnimation<float, Clamp> animation{duration};

        THEN("The values are linearly animated")
        {
            CHECK_FALSE(animation.isCompleted(0));
            CHECK(animation.at(0.) == 0.);
            CHECK(animation.at(50.) == 0.5);
            CHECK(animation.at(100.) == 1);

            CHECK(animation.at(-10.) == 0.);
            CHECK_FALSE(animation.isCompleted(-10.));
            CHECK(animation.at(1000.) == 1.);
            CHECK(animation.isCompleted(100.));
        }
    }
}


SCENARIO("Clamped ParameterAnimation combinations.")
{
    double period = 10.;

    GIVEN("A clamped (non-periodic non-easing) animation.")
    {
        auto animation = makeParameterAnimation<Clamp>(period);

        THEN("It is finite, but not completed.")
        {
            REQUIRE(animation.IsFinite());
            REQUIRE_FALSE(animation.isCompleted(0));
        }

        THEN("It can be sampled along the period.")
        {
            CHECK(animation.at(0.) == 0.);
            CHECK(animation.at(period) == 1.);
            // Linear
            CHECK(animation.at(period * 0.25) == 1./4.);
            CHECK(animation.at(period / 2.)   == 1./2.);
            CHECK(animation.at(period * 0.75) == 3./4.);
        }

        THEN("Over-shooting the period clamps")
        {
            CHECK(animation.at(-period) == 0.);
            CHECK(animation.at(2 * period) == 1.);
        }

        THEN("Over-shoot and completion can be queried")
        {
            CHECK_FALSE(animation.isCompleted(0.));
            CHECK(animation.getOvershoot(0.) == 0.f);

            CHECK_FALSE(animation.isCompleted(period / 2.));
            CHECK(animation.getOvershoot(period / 2.) == 0.f);

            CHECK(animation.isCompleted(period));
            CHECK(animation.getOvershoot(period) == 0.f);

            // Overshoot by two periods
            CHECK(animation.at(2 * period) == animation.at(period));
            CHECK(animation.getOvershoot(2 * period) == period);
        }
    }


    GIVEN("A clamped easing(smoothstep) animation.")
    {
        auto animation = makeParameterAnimation<Clamp, None, ease::SmoothStep>(period);

        THEN("It is finite, but not completed.")
        {
            REQUIRE(animation.IsFinite());
            REQUIRE_FALSE(animation.isCompleted(0.));
        }

        THEN("It can be sampled along the period.")
        {
            CHECK(animation.at(0.) == 0.);
            CHECK(animation.at(period) == 1.);
            // Non-linear
            CHECK(animation.at(period * 0.25) < 1./4.);
            CHECK(animation.at(period / 2.)   == 1./2.);
            CHECK(animation.at(period * 0.75) > 3./4.);
        }

        THEN("Over-shooting the period clamps")
        {
            CHECK(animation.at(-period) == 0.);
            CHECK(animation.at(2 * period) == 1.);
        }

        THEN("Over-shoot and completion can be queried")
        {
            CHECK_FALSE(animation.isCompleted(0.));
            CHECK(animation.getOvershoot(0.) == 0.f);

            CHECK_FALSE(animation.isCompleted(period / 2.));
            CHECK(animation.getOvershoot(period / 2.) == 0.f);

            CHECK(animation.isCompleted(period));
            CHECK(animation.getOvershoot(period) == 0.f);

            // Overshoot by half period
            CHECK(animation.at(3./2. * period) == animation.at(period));
            CHECK(animation.getOvershoot(3./2. * period) == period / 2.);
        }
    }


    GIVEN("A clamped periodic(repeat) animation.")
    {
        auto animation = makeParameterAnimation<Clamp, periodic::Repeat>(period);

        THEN("It is not finite.")
        {
            REQUIRE_FALSE(animation.IsFinite());
            REQUIRE_FALSE(animation.isCompleted(0));
        }

        THEN("It can be sampled along the period.")
        {
            CHECK(animation.at(0.) == 0.);
            // Note: due to peridicity, value at full period wraps back to value at 0.
            CHECK(animation.at(period - period * std::numeric_limits<double>::epsilon()) == Approx(1.));
            // Linear
            CHECK(animation.at(period * 0.25) == 1./4.);
            CHECK(animation.at(period / 2.)   == 1./2.);
            CHECK(animation.at(period * 0.75) == 3./4.);
        }

        THEN("Over-shooting the period wraps around")
        {
            CHECK(animation.at(period) == animation.at(0.));
            CHECK(animation.at(-period) == animation.at(period));
            CHECK(animation.at(2 * period) == animation.at(period));

            CHECK(animation.at((6 * period) + (period * 0.25)) == animation.at(period * 0.25));
            CHECK(animation.at((6 * period) + (period / 2.)) == animation.at(period / 2.));
            CHECK(animation.at((6 * period) + (period * 0.75)) == animation.at(period * 0.75));
        }
    }


    GIVEN("A clamped periodic(repeat) easing(smoothstep) animation.")
    {
        auto animation = makeParameterAnimation<Clamp, periodic::Repeat, ease::SmoothStep>(period);

        THEN("It is not finite.")
        {
            REQUIRE_FALSE(animation.IsFinite());
            REQUIRE_FALSE(animation.isCompleted(0));
        }

        THEN("It can be sampled along the period.")
        {
            CHECK(animation.at(0.) == 0.);
            // Note: due to peridicity, value at full period wraps back to value at 0.
            CHECK(animation.at(period - period * std::numeric_limits<double>::epsilon()) == Approx(1.));
            // Non-linear
            CHECK(animation.at(period * 0.25) < 1./4.);
            CHECK(animation.at(period / 2.)   == 1./2.);
            CHECK(animation.at(period * 0.75) > 3./4.);
        }

        THEN("Over-shooting the period wraps around")
        {
            CHECK(animation.at(period) == animation.at(0.));
            CHECK(animation.at(-period) == animation.at(period));
            CHECK(animation.at(2 * period) == animation.at(period));

            CHECK(animation.at((6 * period) + (period * 0.25)) == animation.at(period * 0.25));
            CHECK(animation.at((6 * period) + (period / 2.)) == animation.at(period / 2.));
            CHECK(animation.at((6 * period) + (period * 0.75)) == animation.at(period * 0.75));
        }
    }
}


SCENARIO("Full-Range ParameterAnimation combinations.")
{
    double period = 10.;
    double speed = 2.;

    GIVEN("A full-range (non-periodic non-easing) animation (i.e. a trivial animation).")
    {
        double targetOutput = 56.3;
        THEN("It is constructed from speed.")
        {
            auto animation = makeParameterAnimation<FullRange>(speed);
            // The input value at which the animation output is expected to reach the target
            double reachValue = targetOutput / speed;

            THEN("It is not finite.")
            {
                REQUIRE_FALSE(animation.IsFinite());
                REQUIRE_FALSE(animation.isCompleted(0.));
            }

            THEN("It can be sampled along the period.")
            {
                CHECK(animation.at(0.) == 0.);
                CHECK(animation.at(reachValue) == targetOutput);
                // Linear
                CHECK(animation.at(reachValue * 0.25) == targetOutput * 0.25);
                CHECK(animation.at(reachValue / 2.)   == targetOutput / 2.);
                CHECK(animation.at(reachValue * 0.75) == targetOutput * 0.75);
            }

            THEN("Over-shooting the period extrapolates")
            {
                CHECK(animation.at(-reachValue) == -targetOutput);
                CHECK(animation.at(3 * reachValue) == 3 * targetOutput);
            }

            THEN("Over-shoot and completion can be queried")
            {
                CHECK_FALSE(animation.isCompleted(0.));
                CHECK(animation.getOvershoot(0.) == 0.f);

                CHECK_FALSE(animation.isCompleted(reachValue / 2.));
                CHECK(animation.getOvershoot(reachValue / 2.) == 0.f);

                // It is not finite
                CHECK_FALSE(animation.isCompleted(reachValue));
                CHECK(animation.getOvershoot(reachValue) == 0.f);
            }
        }
    }


    GIVEN("A full-range easing(smoothstep) animation.")
    {
        WHEN("It is constructed from period alone.")
        {
            auto animation = makeParameterAnimation<FullRange, None, ease::SmoothStep>(period);

            THEN("It is finite.")
            {
                REQUIRE(animation.IsFinite());
                REQUIRE_FALSE(animation.isCompleted(0.));
            }

            THEN("It can be sampled along the period.")
            {
                CHECK(animation.at(0.) == 0.);
                CHECK(animation.at(period) == period);
                // Non-linear
                CHECK(animation.at(period * 0.25) < period * 0.25);
                CHECK(animation.at(period / 2.)   == period / 2.);
                CHECK(animation.at(period * 0.75) > period * 0.75);
            }

            THEN("Over-shooting the period clamps to the range [0 period]")
            {
                CHECK(animation.at(-period) == 0.);
                CHECK(animation.at(2 * period) == period);
            }

            THEN("Over-shoot and completion can be queried")
            {
                CHECK_FALSE(animation.isCompleted(0.));
                CHECK(animation.getOvershoot(0.) == 0.f);

                CHECK_FALSE(animation.isCompleted(period / 2.));
                CHECK(animation.getOvershoot(period / 2.) == 0.f);

                CHECK(animation.isCompleted(period));
                CHECK(animation.getOvershoot(period) == 0.f);

                // Overshoot by one period
                CHECK(animation.at(2. * period) == animation.at(period));
                CHECK(animation.getOvershoot(2. * period) == period);
            }
        } 
        
        WHEN("It is constructed from both period and speed.")
        {
            auto animation = makeParameterAnimation<FullRange, None, ease::SmoothStep>(period, speed);
            double completionValue = period / speed;
            REQUIRE(completionValue != Approx(period));

            THEN("It is finite.")
            {
                REQUIRE(animation.IsFinite());
                REQUIRE_FALSE(animation.isCompleted(0.));
            }

            THEN("It can be sampled along the period.")
            {
                CHECK(animation.at(0.) == 0.);
                CHECK(animation.at(completionValue * 0.99) < period);
                CHECK(animation.at(completionValue) == period);
                // Non-linear
                CHECK(animation.at(completionValue * 0.25) < period * 0.25);
                CHECK(animation.at(completionValue / 2.)   == period / 2.);
                CHECK(animation.at(completionValue * 0.75) > period * 0.75);
            }

            THEN("Over-shooting the period clamps to the range [0 period]")
            {
                CHECK(animation.at(-period) == 0.);
                CHECK(animation.at(2 * completionValue) == period);
            }

            THEN("Over-shoot and completion can be queried")
            {
                CHECK_FALSE(animation.isCompleted(0.));
                CHECK(animation.getOvershoot(0.) == 0.f);

                CHECK_FALSE(animation.isCompleted(completionValue / 2.));
                CHECK(animation.getOvershoot(completionValue / 2.) == 0.f);

                CHECK(animation.isCompleted(completionValue));
                CHECK(animation.getOvershoot(completionValue) == 0.f);

                // Overshoot by two periods
                CHECK(animation.at(3. * period) == animation.at(period));
                CHECK(animation.getOvershoot(completionValue + 2. * period) == 2. * period);
            }
        }
    }


    GIVEN("A full-range periodic(pingpong) animation.")
    {
        WHEN("It is constructed from period alone.")
        {
            auto animation = makeParameterAnimation<FullRange, periodic::PingPong>(period);

            THEN("It is not finite.")
            {
                REQUIRE_FALSE(animation.IsFinite());
                REQUIRE_FALSE(animation.isCompleted(0.));
            }

            THEN("It can be sampled along the period.")
            {
                CHECK(animation.at(0.) == 0.);
                // Note: due to peridicity, value at full period wraps back to value at 0.
                CHECK(animation.at(period - period * std::numeric_limits<double>::epsilon()) 
                      == Approx(period));
                // Linear
                CHECK(animation.at(period * 0.25) == period * 0.25);
                CHECK(animation.at(period / 2.)   == period / 2.);
                CHECK(animation.at(period * 0.75) == period * 0.75);
            }

            THEN("Over-shooting the period ping-pongs.")
            {
                CHECK(animation.at(period) == animation.at(-period));
                CHECK(animation.at(2 * period) == animation.at(0.));

                // It ping-pongs for an odd number of periods
                // Important: Notice the crossing of 0.25 0.75 on each line
                CHECK(animation.at((7 * period) + (period * 0.25)) == animation.at(period * 0.75));
                CHECK(animation.at((9 * period) + (period / 2.)) == animation.at(period / 2.));
                CHECK(animation.at((3 * period) + (period * 0.75)) == animation.at(period * 0.25));

                // It is periodic for an even number of periods
                CHECK(animation.at((6 * period) + (period * 0.25)) == animation.at(period * 0.25));
                CHECK(animation.at((6 * period) + (period / 2.)) == animation.at(period / 2.));
                CHECK(animation.at((6 * period) + (period * 0.75)) == animation.at(period * 0.75));
            }
        }

        WHEN("It is constructed from both period and speed.")
        {
            auto animation = makeParameterAnimation<FullRange, periodic::PingPong>(period, speed);
            double completionValue = period / speed;
            REQUIRE(completionValue != Approx(period));

            THEN("It is not finite.")
            {
                REQUIRE_FALSE(animation.IsFinite());
                REQUIRE_FALSE(animation.isCompleted(0.));
            }

            THEN("It can be sampled along the period.")
            {
                CHECK(animation.at(0.) == 0.);
                // Note: due to peridicity, value at full period wraps back to value at 0.
                CHECK(animation.at(completionValue - completionValue * std::numeric_limits<double>::epsilon()) == Approx(period));
                // Linear
                CHECK(animation.at(completionValue * 0.25) == period * 0.25);
                CHECK(animation.at(completionValue / 2.)   == period / 2.);
                CHECK(animation.at(completionValue * 0.75) == period * 0.75);
            }

            THEN("Over-shooting the period ping-pongs.")
            {
                CHECK(animation.at(completionValue) == animation.at(-completionValue));
                CHECK(animation.at(2 * completionValue) == animation.at(0.));

                // Sanity checks
                REQUIRE(animation.at(completionValue * 0.25) != animation.at(completionValue * 0.75));
                REQUIRE(animation.at(period + (completionValue * 0.25)) != animation.at(completionValue * 0.75));

                // It ping-pongs for an odd number of completion values
                // Important: Notice the crossing of 0.25 0.75 on each line
                CHECK(animation.at((7 * completionValue) + (completionValue * 0.25)) 
                      == animation.at(completionValue * 0.75));
                CHECK(animation.at((9 * completionValue) + (completionValue / 2.)) 
                      == animation.at(completionValue / 2.));
                CHECK(animation.at((3 * completionValue) + (completionValue * 0.75)) 
                      == animation.at(completionValue * 0.25));

                // It is periodic for an even number of completion values
                CHECK(animation.at((6 * completionValue) + (completionValue * 0.25)) 
                      == animation.at(completionValue * 0.25));
                CHECK(animation.at((6 * completionValue) + (completionValue / 2.)) 
                      == animation.at(completionValue / 2.));
                CHECK(animation.at((6 * completionValue) + (completionValue * 0.75)) 
                      == animation.at(completionValue * 0.75));
            }
        }
    }


    GIVEN("A full-range periodic(repeat) easing(smoothstep) animation.")
    {
        WHEN("It is constructed from period alone.")
        {
            auto animation = makeParameterAnimation<FullRange, periodic::Repeat, ease::SmoothStep>(period);

            THEN("It is not finite.")
            {
                REQUIRE_FALSE(animation.IsFinite());
                REQUIRE_FALSE(animation.isCompleted(0.));
            }

            THEN("It can be sampled along the period.")
            {
                CHECK(animation.at(0.) == 0.);
                // Note: due to peridicity, value at full period wraps back to value at 0.
                CHECK(animation.at(period - period * std::numeric_limits<double>::epsilon()) == Approx(period));
                // Non-linear
                CHECK(animation.at(period * 0.25) < period * 0.25);
                CHECK(animation.at(period / 2.)   == period / 2.);
                CHECK(animation.at(period * 0.75) > period * 0.75);
            }

            THEN("Over-shooting the period wraps around")
            {
                CHECK(animation.at(period) == animation.at(0.));
                CHECK(animation.at(-period) == animation.at(period));
                CHECK(animation.at(2 * period) == animation.at(period));

                CHECK(animation.at(( 6 * period) + (period * 0.25)) 
                      == animation.at(period * 0.25));
                CHECK(animation.at(( 7 * period) + (period / 2.)) 
                      == animation.at(period / 2.));
                CHECK(animation.at((-1 * period) + (period * 0.75)) 
                      == animation.at(period * 0.75));
            }
        }

        WHEN("It is constructed from both period and speed.")
        {

            auto animation = makeParameterAnimation<FullRange, periodic::Repeat, ease::SmoothStep>(period, speed);
            double completionValue = period / speed;
            REQUIRE(completionValue != Approx(period));

            THEN("It is not finite.")
            {
                REQUIRE_FALSE(animation.IsFinite());
                REQUIRE_FALSE(animation.isCompleted(0.));
            }

            THEN("It can be sampled along the period.")
            {
                CHECK(animation.at(0.) == 0.);
                // Note: due to peridicity, value at full period wraps back to value at 0.
                CHECK(animation.at(completionValue - completionValue * std::numeric_limits<double>::epsilon())
                      == Approx(period));
                // Non-linear
                CHECK(animation.at(completionValue * 0.25) < period * 0.25);
                CHECK(animation.at(completionValue / 2.)   == period / 2.);
                CHECK(animation.at(completionValue * 0.75) > period * 0.75);
            }

            THEN("Over-shooting the period wraps around")
            {
                CHECK(animation.at(completionValue) == animation.at(0.));
                CHECK(animation.at(-completionValue) == animation.at(completionValue));
                CHECK(animation.at(2 * completionValue) == animation.at(completionValue));

                CHECK(animation.at(( 6 * completionValue) + (completionValue * 0.25))
                      == animation.at(completionValue * 0.25));
                CHECK(animation.at(( 7 * completionValue) + (completionValue / 2.)) 
                      == animation.at(completionValue / 2.));
                CHECK(animation.at((-1 * completionValue) + (completionValue * 0.75)) 
                      == animation.at(completionValue * 0.75));
            }
        }
    }
}
