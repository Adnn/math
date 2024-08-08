#include "catch.hpp"

#include "CustomCatchMatchers.h"

#include <math/LinearMatrix.h>
#include <math/Quaternion.h>
#include <math/Transformations.h>

#include <math/Interpolation/QuaternionInterpolation.h>


using namespace ad::math;


const double gEpsilon = std::numeric_limits<double>::epsilon();


SCENARIO("Quaternion instantiation, access, comparison.")
{
    GIVEN("The 4 component values of a quaternion rotating 180° about Y.")
    {
        double x = 0., y = 1., z = 0., w = 0.;

        WHEN("A quaternion is instantiated.")
        {
            Quaternion q{x, y, z, w};

            THEN("Its individual components can be accessed.")
            {
                REQUIRE(q.x() == x);
                REQUIRE(q.y() == y);
                REQUIRE(q.z() == z);
                REQUIRE(q.w() == w);
            }

            THEN("It can be compared against other quaternions.")
            {
                REQUIRE(q == q);
                REQUIRE_FALSE(q != q);
                REQUIRE(q.equalsWithinTolerance(q, gEpsilon));

                Quaternion other{1., 0., 0., 0.};
                REQUIRE(q != other);
                REQUIRE_FALSE(q == other);
                REQUIRE_FALSE(q.equalsWithinTolerance(other, gEpsilon));
            }

            THEN("It is equivalent to a quaternion instantiated from axis and angle.")
            {
                REQUIRE(q.equalsWithinTolerance(
                            Quaternion{UnitVec{Vec<3>{0., 1., 0.}}, Radian<double>{pi<double>}},
                            gEpsilon));
            }
        }
    }

}


SCENARIO("Identity quaternion.")
{
    GIVEN("The identity quaternion.")
    {
        auto identity = Quaternion<double>::Identity();

        THEN("Its individual components can be accessed.")
        {
            REQUIRE(identity.x() == 0);
            REQUIRE(identity.y() == 0);
            REQUIRE(identity.z() == 0);
            REQUIRE(identity.w() == 1);
        }
    }
}


SCENARIO("Quaternion conjugate and inverse.")
{
    GIVEN("The identity quaternion.")
    {
        auto identity = Quaternion<double>::Identity();

        THEN("Its is its own inverse and conjugate.")
        {
            REQUIRE(identity.conjugate() == identity);
            REQUIRE(identity.inverse() == identity);
        }
    }

    GIVEN("A non-trivial rotation quaternion.")
    {
        Quaternion q{UnitVec{Vec<3>{1., 2., 3.}}, Degree<double>{58.}};

        THEN("It conjugate is as expected.")
        {
            Quaternion conjugate = q.conjugate();

            REQUIRE(conjugate.x() == -q.x());
            REQUIRE(conjugate.y() == -q.y());
            REQUIRE(conjugate.z() == -q.z());
            REQUIRE(conjugate.w() ==  q.w());
        }

        THEN("It negation is as expected.")
        {
            Quaternion negation = -q;

            REQUIRE(negation.x() == -q.x());
            REQUIRE(negation.y() == -q.y());
            REQUIRE(negation.z() == -q.z());
            REQUIRE(negation.w() == -q.w());
        }

        THEN("The conjugate of unit quaternion is also the inverse.")
        {
            REQUIRE(q.conjugate() == q.inverse());
        }
    }
}



SCENARIO("Quaternion multiplication.")
{
    GIVEN("A quaternion rotating 180° about Y.")
    {
        Quaternion y{0., 1., 0., 0.};

        THEN("Multiplying by the identity quaternion leaves it unchanged.")
        {
            REQUIRE(y * Quaternion<double>::Identity() == y);
            REQUIRE(Quaternion<double>::Identity() * y == y);

            Quaternion yCopy{y};
            REQUIRE((yCopy *= Quaternion<double>::Identity()) == y);
        }

        THEN("Multiplying by its inverse result in the identity quaternion.")
        {
            auto identity = Quaternion<double>::Identity();
            REQUIRE(y * y.inverse() == identity);
            REQUIRE(y.inverse() * y == identity);

            Quaternion yCopy{y};
            REQUIRE((yCopy *= y.inverse()) == identity);
            REQUIRE((y.inverse() *= y) == identity);
        }

        THEN("It is equal to the multiplication of two quaternion rotation 90° about Y.")
        {
            Quaternion half{UnitVec{Vec<3>{0., 1., 0.}}, Degree<double>{90.}};
            REQUIRE(y.equalsWithinTolerance(half * half, gEpsilon));
        }
    }

    GIVEN("Two rotation quaternions.")
    {
        Quaternion q1{0.4082483, 0.4082483, 0.4082483, 0.7071068};
        Quaternion q2{0., 0.5286528, 0.203328, 0.8241262};
        
        GIVEN("Their precomputed product.")
        {
            // Precomputed via an online calculator:
            // https://www.omnicalculator.com/math/quaternion
            Quaternion expected{0.20363482359, 0.62725379951, 0.69604433846, 0.28391532285};

            THEN("Their product matches the expected result")
            {
                REQUIRE((q1 * q2).equalsWithinTolerance(expected, 10E-12));
            }

            THEN("The assignment-product matches the expected result")
            {
                REQUIRE((q1 *= q2).equalsWithinTolerance(expected, 10E-12));
            }

            THEN("Multiplication is not commutative.")
            {
                REQUIRE_FALSE((q1 * q2).equalsWithinTolerance(q2 * q1, 10E-12));
            }
        }
    }
}


SCENARIO("Quaternion rotation.")
{
    GIVEN("A quaternion rotating 180° about Y.")
    {
        Quaternion y{0., 1., 0., 0.};

        THEN("It can rotate vectors.")
        {
            Vec<3> i{1., 0., 0.};
            Vec<3> j{0., 1., 0.};
            Vec<3> k{0., 0., 1.};

            REQUIRE(y.rotate(i) == -i);
            REQUIRE(y.rotate(j) == j);
            REQUIRE(y.rotate(k) == -k);
        }

        THEN("It can rotate positions.")
        {
            Position<3> pos{1., 2., -3.};

            REQUIRE(y.rotate(pos) == Position<3>{-1., 2., 3.});
        }

        THEN("It matches the corresponding rotation matrix.")
        {
            LinearMatrix<3, 3, double> rotation = trans3d::rotateY(Degree<double>{180.});
            REQUIRE(y.toRotationMatrix().equalsWithinTolerance(rotation, gEpsilon));
        }
    }

    GIVEN("A non-trivial rotation quaternion.")
    {
        Vec<3> axis{1., 2., 3.};
        Degree<double> angle{58.};
        Quaternion q{UnitVec{axis}, angle};

        GIVEN("The corresponding rotation matrix.")
        {
            LinearMatrix<3, 3, double> rotation = trans3d::rotate(UnitVec{axis}, angle);

            THEN("The quaternion can be converted to the rotation matrix form.")
            {
                REQUIRE(q.toRotationMatrix().equalsWithinTolerance(rotation, gEpsilon));
            }

            THEN("The rotation matrix can be converted to the quaternion form.")
            {
                CHECK(toQuaternion(rotation).equalsWithinTolerance(q, gEpsilon));
            }

            GIVEN("Vectors and positions.")
            {
                Vec<3> vec{1.2, 0.6, -8.};
                Position<3> pos{0., 10., 100.};

                THEN("Rotation from quaternion matches rotation from matrix")
                {
                    CHECK(q.rotate(vec).equalsWithinTolerance(vec * rotation, 10E-12));
                    CHECK(q.rotate(pos).equalsWithinTolerance(pos * rotation, 10E-12));
                }
            }
        }
    }
}


SCENARIO("Quaternion difference.")
{
    GIVEN("A quaternion rotating 180° about Y.")
    {
        Quaternion y{0., 1., 0., 0.};

        WHEN("It rotates orientations represented as quaternions.")
        {
            constexpr Quaternion o_1{Quaternion<double>::Identity()};
            /*constexpr*/ Quaternion o_2{UnitVec<3>{{3., 2., 1.}}, Degree<double>{16.}};

            Quaternion r_1 = y * o_1;
            Quaternion r_2 = y * o_2;

            THEN("The difference between the two orientations is the rotation quaternion.")
            {
                CHECK(difference(o_1, r_1) == y);
                CHECK(difference(o_2, r_2).equalsWithinTolerance(y, gEpsilon));
            }

            THEN("The difference from the destionation orientation to the source orientation is the opposite angulare deplacement.")
            {
                CHECK(difference(r_1, o_1).equalsWithinTolerance(y.conjugate(), gEpsilon));
                CHECK(difference(r_2, o_2).equalsWithinTolerance(y.conjugate(), gEpsilon));
            }

            THEN("The difference between an orientation and itself is the identity.")
            {
                CHECK(difference(o_1, o_1).equalsWithinTolerance(Quaternion<double>::Identity(), gEpsilon));
                CHECK(difference(o_2, o_2).equalsWithinTolerance(Quaternion<double>::Identity(), gEpsilon));
                CHECK(difference(r_1, r_1).equalsWithinTolerance(Quaternion<double>::Identity(), gEpsilon));
                CHECK(difference(r_2, r_2).equalsWithinTolerance(Quaternion<double>::Identity(), gEpsilon));
            }
        }
    }

    GIVEN("A non-trivial rotation quaternion.")
    {
        Vec<3> axis{1., 2., 3.};
        Degree<double> angle{58.};
        Quaternion q{UnitVec{axis}, angle};

        WHEN("It rotates orientations represented as quaternions.")
        {
            constexpr Quaternion o_1{Quaternion<double>::Identity()};
            /*constexpr*/ Quaternion o_2{UnitVec<3>{{3., 2., 1.}}, Degree<double>{16.}};

            Quaternion r_1 = q * o_1;
            Quaternion r_2 = q * o_2;

            THEN("The difference between the two orientations is the rotation quaternion.")
            {
                CHECK(difference(o_1, r_1).equalsWithinTolerance(q, gEpsilon));
                CHECK(difference(o_2, r_2).equalsWithinTolerance(q, gEpsilon));
            }

            THEN("The difference from the destionation orientation to the source orientation is the opposite angulare deplacement.")
            {
                CHECK(difference(r_1, o_1).equalsWithinTolerance(q.conjugate(), gEpsilon));
                CHECK(difference(r_2, o_2).equalsWithinTolerance(q.conjugate(), gEpsilon));
            }
        }
    }
}


SCENARIO("Quaternion interpolation.")
{
    GIVEN("Two quaternions representing different rotations around the same axis.")
    {
        UnitVec<3, double> axis{{1., 1., 1.}};
        Degree<double> angle1{90.};
        Degree<double> increment{180.};
        Degree<double> angle2 = angle1 + increment;

        Quaternion q1{axis, angle1};
        Quaternion q2{axis, angle2};

        THEN("They can be interpolated.")
        {
            double parameter = 0.5;
            REQUIRE(lerp(q1, q2, Clamped{parameter})
                .equalsWithinTolerance(Quaternion{axis, angle1 + increment * parameter}, gEpsilon));

            // Only midpoint is correct with linear interpolation
            THEN("Linear interpolation is diverging away from the expected values.")
            {
                parameter = 0.1;
                REQUIRE_FALSE(lerp(q1, q2, Clamped{parameter})
                    .equalsWithinTolerance(Quaternion{axis, angle1 + increment * parameter}, gEpsilon));

                parameter = 0.78;
                REQUIRE_FALSE(lerp(q1, q2, Clamped{parameter})
                    .equalsWithinTolerance(Quaternion{axis, angle1 + increment * parameter}, gEpsilon));
            }
        }

        THEN("They can be spherically interpolated.")
        {
            double parameter = 0.5;
            REQUIRE(slerp(q1, q2, Clamped{parameter})
                .equalsWithinTolerance(Quaternion{axis, angle1 + increment * parameter}, gEpsilon));

            // All points are correct
            parameter = 0.1;
            REQUIRE(slerp(q1, q2, Clamped{parameter})
                .equalsWithinTolerance(Quaternion{axis, angle1 + increment * parameter}, gEpsilon));

            parameter = 0.78;
            REQUIRE(slerp(q1, q2, Clamped{parameter})
                .equalsWithinTolerance(Quaternion{axis, angle1 + increment * parameter}, gEpsilon));
        }
    }

    GIVEN("Two quaternions 90 degrees appart.")
    {
        UnitVec<3, double> axis{{0., 0., 1.}};
        Degree<double> angle1{270.};
        Degree<double> angle2{0.};
        Degree<double> difference{90.};

        Quaternion q1{axis, angle1};
        Quaternion q2{axis, angle2};

        THEN("They lerp along the shortest path.")
        {
            double parameter = 0.5;

            // NOTE: This negates the resulting quaternion, but it corresponds to the same rotation.
            REQUIRE(lerp(q1, q2, Clamped{parameter})
                .equalsWithinTolerance(-Quaternion{axis, angle1 + difference * parameter}, gEpsilon));
        }

        THEN("They spherically interpolate along the shortest path.")
        {
            double parameter = 0.5;

            // NOTE: This negates the resulting quaternion, but it corresponds to the same rotation.
            REQUIRE(slerp(q1, q2, Clamped{parameter})
                .equalsWithinTolerance(-Quaternion{axis, angle1 + difference * parameter}, gEpsilon));
        }
    }

    GIVEN("A single quaternion")
    {
        UnitVec<3, double> axis{{1., 1., 1.}};
        Degree<double> angle{90.};
        Quaternion q{axis, angle};


        THEN("Linear interpolation with itself in the same quaterion.")
        {
            double parameter = 0.5;
            CHECK(lerp(q, q, Clamped{parameter}).equalsWithinTolerance(q, gEpsilon));
        }

        THEN("Spherical interpolation with itself in the same quaterion.")
        {
            // Note: tests the extreme case where angle between quaternions might tend toward zero.
            double parameter = 0.5;
            CHECK(slerp(q, q, Clamped{parameter}).equalsWithinTolerance(q, gEpsilon));
        }
    }
}


SCENARIO("Quaternion conversions")
{
    auto procedure = [](Radian<float> halfTheta, UnitVec<3, float> axis, std::size_t maxIdx)
    {
        constexpr float epsilon = 10E-7f;

        float s = sin(halfTheta);
        float c = cos(halfTheta);
        const Quaternion<float> q{s * axis.x(), s * axis.y(), s * axis.z(), c};

        //// Sanity checks
        {
            REQUIRE(q.getNormSquared() == 1.f);
            auto v = q.asVec();
            REQUIRE(v.getMaxMagnitudeElement() == (v.begin() + maxIdx));
        }

        WHEN("It is converted to rotation matrix")
        {
            const LinearMatrix<3, 3, float> matrix = q.toRotationMatrix();

            THEN("Rotation from quaternion matches rotation from matrix")
            {
                Vec<3, float> vec{0.2f, -0.6f, -8.8f};
                Position<3, float> pos{5.f, 10.f, -0.1f};

                CHECK_THAT(q.rotate(vec), Approximates(vec * matrix, epsilon));
                CHECK_THAT(q.rotate(pos), Approximates(pos * matrix, epsilon));
            }

            WHEN("The matrix is converted back to a quaternion")
            {
                const Quaternion<float> qBack = toQuaternion(matrix);

                THEN("The converted quaternion matches the initial quaternion.")
                {
                    // A quaternion and its negation represent the same rotation, allow both.
                    CHECK_THAT(q, Approximates(qBack) || Approximates(-qBack));
                }
            }
        }
    };

    GIVEN("A quaternion whose maximal value is on x")
    {
        procedure(Radian<float>{pi<float>/3}, UnitVec<3, float>{{1.f, 1.f, 1.f}}, 0);
    }

    GIVEN("A quaternion whose maximal value is on y")
    {
        procedure(Radian<float>{pi<float>/3}, UnitVec<3, float>{{-1.f, 2.f, 0.f}}, 1);
    }

    GIVEN("A quaternion whose maximal value is on z")
    {
        procedure(Radian<float>{pi<float>/3}, UnitVec<3, float>{{0.f, 2.f, -4.f}}, 2);
    }

    GIVEN("A quaternion whose maximal value is on w")
    {
        procedure(Radian<float>{pi<float>/5}, UnitVec<3, float>{{1.f, 0.f, 0.f}}, 3);
    }
}