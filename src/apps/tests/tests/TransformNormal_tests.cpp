#include "catch.hpp"

#include <math/Transformations.h>
#include <math/Vector.h>


using namespace ad::math;


SCENARIO("Normal need a different transformation matrix.")
{
    GIVEN("A 2D line and its normal.")
    {
        Position<2> a{2., 0.};
        Position<2> b{2., 5.};

        Vec<2> normal{1., 0.};

        // Sanity check: the normal is indeed perpendicular to the line
        REQUIRE((b-a).dot(normal) == 0.);

        WHEN("They are transformed by a shearing matrix")
        {
            Matrix<2, 2> shearing = trans2d::shearY(2.);

            Position<2> aTrans = a*shearing;
            Position<2> bTrans = b*shearing;

            // Sanity check: the direction of AB did not change, since the shear is colinear with the segment.
            REQUIRE((bTrans-aTrans).dot(normal) == 0.);

            THEN("The same transform does not correctly apply to normals")
            {
                Vec<2> normalProblem = normal * shearing;
                REQUIRE_FALSE((bTrans-aTrans).dot(normalProblem) == 0.);
            }

            THEN("The transform can be adapted to apply to normals")
            {
                Vec<2> normalTrans = normal * shearing.toNormalTansformScaled();
                REQUIRE((bTrans-aTrans).dot(normalTrans) == 0.);
            }

        }
    }
}
