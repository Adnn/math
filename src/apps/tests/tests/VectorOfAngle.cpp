#include "catch.hpp"

#include "detection.h"
#include "operation_detectors.h"

#include <math/Angle.h>
#include <math/Vector.h>


using namespace ad;
using namespace ad::math;


SCENARIO("Vector of Angle construction")
{
    GIVEN("The type: vector of radians of dimension 3")
    {
        using VecRadian = Vec<3, Radian<double>>;
        WHEN("It is constructed as a zero valued vector")
        {
            // It is a verification that it compiles
            VecRadian zero = VecRadian::Zero();

            THEN("All angles are null")
            {
                for (int i = 0; i != 3; ++i)
                {
                    REQUIRE(zero[i].value() == 0.0);
                }
            }
        }

        GIVEN("An explicit instantiation with 3 angles")
        {
            using namespace angle_literals;
            VecRadian angles{3._rad, 10.5_rad, -2.0_rad};

            THEN("It stores the initialized from values")
            {
                REQUIRE(angles[0] == Radian<double>(3.0));
                REQUIRE(angles[1] == Radian<double>(10.5));
                REQUIRE(angles[2] == Radian<double>(-2.0));
            }
        }
    }
}


SCENARIO("Vector of Angle operations")
{
    using namespace angle_literals;
    using VecRadian = Vec<3, Radian<double>>;

    GIVEN("A vector of radians of dimension 3")
    {
        VecRadian angles{3._rad, 10.5_rad, -2.0_rad};

        GIVEN("A scalar value")
        {
            int scalar = 10;

            WHEN("The vector is compound-multiplied by a scalar")
            {
                angles *= scalar;
                THEN("The vector values are each multiplied accordingly")
                {
                    REQUIRE(angles[0] == Radian<double>(scalar * 3.0));
                    REQUIRE(angles[1] == Radian<double>(scalar * 10.5));
                    REQUIRE(angles[2] == Radian<double>(scalar * -2.0));
                }
            }
            WHEN("The vector is compound-divided by a scalar")
            {
                angles /= scalar;
                THEN("The vector values are each divided accordingly")
                {
                    REQUIRE(angles[0] == Radian<double>(3.0 / scalar));
                    REQUIRE(angles[1] == Radian<double>(10.5 / scalar));
                    REQUIRE(angles[2] == Radian<double>(-2.0 / scalar));
                }
            }

            WHEN("The vector is multiplied by the scalar")
            {
                auto result = angles * scalar;

                THEN("The source vector values are not changed")
                {
                    REQUIRE(angles[0] == Radian<double>(3.0));
                    REQUIRE(angles[1] == Radian<double>(10.5));
                    REQUIRE(angles[2] == Radian<double>(-2.0));
                }
                THEN("The resulting vector values are multiplied accordingly")
                {
                    REQUIRE(result[0] == Radian<double>(scalar * 3.0));
                    REQUIRE(result[1] == Radian<double>(scalar * 10.5));
                    REQUIRE(result[2] == Radian<double>(scalar * -2.0));
                }
            }
            WHEN("The vector is divided by the scalar")
            {
                auto result = angles / scalar;

                THEN("The source vector values are not changed")
                {
                    REQUIRE(angles[0] == Radian<double>(3.0));
                    REQUIRE(angles[1] == Radian<double>(10.5));
                    REQUIRE(angles[2] == Radian<double>(-2.0));
                }
                THEN("The resulting vector values are divided accordingly")
                {
                    REQUIRE(result[0] == Radian<double>(3.0 / scalar));
                    REQUIRE(result[1] == Radian<double>(10.5 / scalar));
                    REQUIRE(result[2] == Radian<double>(-2.0 / scalar));
                }
            }
        }

        GIVEN("Another vector of radians of dimension 3")
        {
            VecRadian otherAngles{100._rad, -100._rad, 0._rad};

            WHEN("They are added together")
            {
                VecRadian result = angles + otherAngles;

                THEN("The resulting vector values is componentwise addition")
                {
                    REQUIRE(result[0] == 103.0_rad);
                    REQUIRE(result[1] == -89.5_rad);
                    REQUIRE(result[2] ==  -2.0_rad);
                }
            }

            WHEN("They are compound-substracted together")
            {
                angles -= otherAngles;

                THEN("The first vector values contains componentwise substraction")
                {
                    REQUIRE(angles[0] == -97.0_rad);
                    REQUIRE(angles[1] == 110.5_rad);
                    REQUIRE(angles[2] ==  -2.0_rad);
                }
            }
        }
    }
}


SCENARIO("Vector of Angle available operations")
{
    THEN("Vector of angles with matching dimensions can be added together")
    {
        REQUIRE(is_detected_v<is_additive_t, Vec<2, Radian<float>>, Vec<2, Radian<float>>>);
    }

    THEN("Vector of angles cannot be added with a vector of scalars, even with matching dimensions")
    {
        REQUIRE_FALSE(is_detected_v<is_additive_t, Vec<2, Radian<float>>, Vec<2, float>>);
    }
}
