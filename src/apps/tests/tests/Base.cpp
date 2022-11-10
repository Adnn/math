#include "catch.hpp"

#include <math/Base.h>


using namespace ad::math;


SCENARIO("Making orthonormal 2D bases.")
{
    using Base = OrthonormalBase<2>;

    GIVEN("A direction vector")
    {
        Vec<2> direction{2., 0.};
        THEN("A bases can be instantiated")
        {
            Base from1 = Base::MakeFromSingleVector(direction);

            REQUIRE(from1.u().x() == 1);
            REQUIRE(from1.u() == direction.normalize());
            REQUIRE(from1.u().getNorm() == 1.);
            REQUIRE(from1.v().getNorm() == 1.);

            REQUIRE(from1.u().dot(from1.v()) == 0.);
            REQUIRE(from1.v().x() == 0.);
            REQUIRE(from1.v().y() == 1.);
        }
    }
}


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

    GIVEN("A W direction vector and an Up vector")
    {
        Vec<3> direction{1., 0.18, 0.};
        // Note that the up vector is initially not perpendicular to W direction.
        Vec<3> up{0., 1.2, 0.};

        WHEN("A bases is instantiated from W and V (up)")
        {
            Base from2 = Base::MakeFromWUp(direction, up);
            
            THEN("The first vector direction is maintained.")
            {
                REQUIRE(from2.w() == direction.normalize());
            }

            THEN("The base is orthonormal")
            {
                REQUIRE(from2.u().getNorm() == 1.);
                REQUIRE(from2.u().getNorm() == 1.);
                // Already checked by the fact that w is a normalized vector above.
                REQUIRE(from2.w().getNorm() == 1.);

                REQUIRE(from2.u().dot(from2.v()) == 0);
                REQUIRE(from2.u().dot(from2.w()) == 0);
                REQUIRE(from2.v().dot(from2.w()) == 0);
            }

            THEN("The side vector is orthogonal to both original vectors used to construct the base.")
            {
                REQUIRE(from2.u() == up.cross(direction).normalize());
            }

            THEN("The V vector is kept as close as possible to the original Up direction.")
            {
                // the view direction is barely tilting up.
                REQUIRE(from2.v().dot(up.normalize()) <= 1.);
                REQUIRE(from2.v().dot(up.normalize()) > 0.98);
            }
        }
    }

    GIVEN("A W direction vector and an Side vector, strongly out of square.")
    {
        Vec<3> direction{-1., 1.5, 0.};
        // Note that the up vector is initially not perpendicular to W direction.
        Vec<3> side{0., 1., 0.};

        WHEN("A bases is instantiated from W and U (side)")
        {
            Base from2 = Base::MakeFromWSide(direction, side);
            
            THEN("The first vector direction is maintained.")
            {
                REQUIRE(from2.w() == direction.normalize());
            }

            THEN("The base is orthonormal")
            {
                REQUIRE(from2.u().getNorm() == 1.);
                REQUIRE(from2.u().getNorm() == 1.);
                // Already checked by the fact that w is a normalized vector above.
                REQUIRE(from2.w().getNorm() == 1.);

                REQUIRE(from2.u().dot(from2.v()) == 0);
                REQUIRE(from2.u().dot(from2.w()) == 0);
                REQUIRE(from2.v().dot(from2.w()) == 0);
            }

            THEN("The up vector is orthogonal to both original vectors used to construct the base.")
            {
                REQUIRE(from2.v() == direction.cross(side).normalize());
            }

            THEN("The U vector keep the general side direction, yet changes a lot due to the fact direction and side are out of square.")
            {
                // Note: The literal magic value has been taken from runtime on 2022/11/10.
                REQUIRE(from2.u().dot(side.normalize()) == Approx(0.5547));
            }
        }
    }
}
