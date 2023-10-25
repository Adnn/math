#include "catch.hpp"

#include "CustomCatchMatchers.h"

#include <math/EulerAngles.h>
#include <math/Transformations.h>


using namespace ad::math;


template <class T_value>
LinearMatrix<3, 3, T_value> naiveToRotationMatrix(EulerAngles<T_value> aEuler)
{
    return trans3d::rotateX(aEuler.x)
        * trans3d::rotateY(aEuler.y)
        * trans3d::rotateZ(aEuler.z);
}


SCENARIO("Euler angles to rotation matrix")
{
    GIVEN("Euler angles")
    {
        EulerAngles<float> angles{
            .x = Degree{60.f},
            .y = Degree{45.f},
            .z = Degree{77.f},
        };

        WHEN("They are converted to a rotation matrix")
        {
            LinearMatrix<3, 3, float> m = toRotationMatrix(angles);

            THEN("The conversion is equivalent to applying the euler angle rotations in sequence")
            {
                CHECK(m == naiveToRotationMatrix(angles));
            }
        }
    }

    GIVEN("Euler angles")
    {
        EulerAngles<float> angles{
            .x = Radian{0.543f},
            .y = Degree{66.f},
            .z = Turn{-1.25f},
        };

        WHEN("They are converted to a rotation matrix")
        {
            LinearMatrix<3, 3, float> m = toRotationMatrix(angles);

            THEN("The conversion is equivalent to applying the euler angle rotations in sequence")
            {
                CHECK(m == naiveToRotationMatrix(angles));
            }
        }
    }
}


SCENARIO("Euler angles to Quaternions")
{
    GIVEN("Euler angles")
    {
        const EulerAngles<float> angles{
            .x = Degree{30.f},
            .y = Degree{45.f},
            .z = Degree{90.f},
        };

        WHEN("They are converted to Quaternion")
        {
            Quaternion<float> q = toQuaternion(angles);

            THEN("The quaternion apply the same rotation as the Euler angles")
            {
                constexpr float tolerance = std::numeric_limits<float>::epsilon() * 100;
                const Vec<3, float> v{245.f, -0.28f, 6.5f};
                CHECK_THAT(q.rotate(v), Approximates(v * toRotationMatrix(angles), tolerance));
            }

            THEN("The quaternion conversion to matrix is equivalent to the angles conversion to matrix")
            {
                CHECK_THAT(q.toRotationMatrix(), Approximates(toRotationMatrix(angles)));
            }

            WHEN("The quaternion is converted back to Euler angles")
            {
                EulerAngles<float> anglesBis = toEulerAngles(q);

                THEN("This is equivalent to the starting angles")
                {
                    CHECK_THAT(anglesBis, Approximates(angles));
                }

                THEN("Offseting the result is not equivalent to the starting angles")
                {
                    anglesBis.x += Radian{0.00001f};
                    CHECK_THAT(anglesBis, !Approximates(angles));
                }
            }
        }
    }
}


SCENARIO("Quaternion to Euler Angles")
{
    GIVEN("A rotation quaternion")
    {
        Quaternion<double> q{
            UnitVec<3, double>{{0.5, -0.5, 0.2}},
            Degree{33.3}
        };

        WHEN("It is converted to Euler angles")
        {
            EulerAngles<double> euler = toEulerAngles(q);

            THEN("The quaternion apply the same rotation as the Euler angles")
            {
                constexpr double tolerance = std::numeric_limits<double>::epsilon() * 10;
                const Vec<3, double> v{0.f, -0.28f, 6.5f};
                CHECK_THAT(q.rotate(v), Approximates(v * toRotationMatrix(euler), tolerance));
            }

            THEN("The quaternion conversion to matrix is equivalent to the angles conversion to matrix")
            {
                constexpr double tolerance = std::numeric_limits<double>::epsilon() * 10;
                CHECK_THAT(q.toRotationMatrix(), Approximates(toRotationMatrix(euler), tolerance));
            }

            WHEN("The Euler angles are converted back to quaternion")
            {
                Quaternion<double> qBis = toQuaternion(euler);

                THEN("This is equivalent to the starting quaternion")
                {
                    CHECK_THAT(qBis, Approximates(q));
                }
            }
        }
    }
}