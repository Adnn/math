#include "catch.hpp"

#include <math/Base.h>


using namespace ad::math;


SCENARIO("Making orthonormal 3D bases.")
{
    using Base = OrthonormalBase<3>;

    GIVEN("A direction vector")
    {
        Vec<3> direction{2., 0., 0.};
        THEN("A bases can be instantiated")
        {
            Base from1 = Base::MakeFromSingleVector(direction);

            REQUIRE(from1.w().x() == 1);
            REQUIRE(from1.w() == direction.normalize());
            REQUIRE(from1.u().getNorm() == 1.);
            REQUIRE(from1.v().getNorm() == 1.);

            REQUIRE(from1.u().dot(from1.v()) == 0);
            REQUIRE(from1.u().dot(from1.w()) == 0);
            REQUIRE(from1.v().dot(from1.w()) == 0);
        }
    }

    GIVEN("A direction vector and an UP vector")
    {
        Vec<3> direction{1., 0.18, 0.};
        Vec<3> up{0., 1.2, 0.};

        THEN("A bases can be instantiated")
        {
            Base from2 = Base::MakeFromTwoVectors(direction, up);

            REQUIRE(from2.w() == direction.normalize());
            REQUIRE(from2.u().getNorm() == 1.);
            REQUIRE(from2.v().getNorm() == 1.);

            REQUIRE(from2.u().dot(from2.v()) == 0);
            REQUIRE(from2.u().dot(from2.w()) == 0);
            REQUIRE(from2.v().dot(from2.w()) == 0);

            REQUIRE(from2.u() == up.cross(direction).normalize());
            // the view direction is barely tilting up.
            REQUIRE(from2.v().dot(up.normalize()) < 0.999);
            REQUIRE(from2.v().dot(up.normalize()) > 0.98);
        }
    }
}
