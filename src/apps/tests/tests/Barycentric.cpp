#include "catch.hpp"

#include <math/Barycentric.h>

using namespace ad::math;


SCENARIO("Computing barycentric coordinates")
{
    GIVEN("A barycentric based on a 2D triangle")
    {
        auto barycentric = Barycentric<double>( {0., 0.}, {0., 2.}, {2., 0.} );

        THEN("Barycentice coordinates of points can be computed")
        {
            REQUIRE(barycentric.getCoordinates({0., 0.})
                    == ad::math::Barycentric<double>::Coordinates{1., 0., 0.});

            REQUIRE(barycentric.getCoordinates({0., 2.})
                    == ad::math::Barycentric<double>::Coordinates{0., 1., 0.});

            REQUIRE(barycentric.getCoordinates({2., 0.})
                    == ad::math::Barycentric<double>::Coordinates{0., 0., 1.});

            REQUIRE(barycentric.getCoordinates({1., 1.})
                    == ad::math::Barycentric<double>::Coordinates{0., 0.5, 0.5});

            REQUIRE(barycentric.getCoordinates({.5, .5})
                    == ad::math::Barycentric<double>::Coordinates{0.5, 0.25, 0.25});

            REQUIRE(barycentric.getCoordinates({2., 2.})
                    == ad::math::Barycentric<double>::Coordinates{-1., 1., 1.});
        }
    }
}

