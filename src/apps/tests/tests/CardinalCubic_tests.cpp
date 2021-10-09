#include "catch.hpp"

#include <math/Curves/CardinalCubic.h>


using namespace ad::math;


constexpr double gEpsilon{0.000001};


SCENARIO("Cardinal Cubic curve API.")
{
    GIVEN("A cardinal cubic curve with no tension")
    {
        Position<2> a{-100.,  -60.};
        Position<2> b{ -20.,   20.};
        Position<2> c{  20.,   20.};
        Position<2> d{ 100.,  -60.};

        CardinalCubic<2, double> catmullRom{0., a, b, c, d};

        std::array<double, 5> params{0., 1/4., 2/4., 3./4, 1.};
        std::array<Vec<4>, 5> expectedCoeffs{
            Vec<4>{0., 1., 0., 0.},
            Vec<4>{-0.0703125, 0.867188, 0.226562, -0.0234375},
            Vec<4>{-0.0625, 0.5625, 0.5625, -0.0625},
            Vec<4>{-0.0234375, 0.226562, 0.867188, -0.0703125},
            Vec<4>{0., 0., 1., 0.},
        };

        std::array<Position<2>, 5> expectedPositions{
            Position<2>{-20., 20.},
            Position<2>{-8.125, 27.5},
            Position<2>{0., 30.},
            Position<2>{8.125, 27.5},
            Position<2>{20., 20.},
        };

        THEN("It can be evaluated alongside the parametric space")
        {
            int id = 0;
            for (auto t : params)
            {
                REQUIRE(expectedCoeffs[id].equalsWithinTolerance(
                    catmullRom.getBlendingCoefficients(t), gEpsilon
                ));
                REQUIRE(expectedPositions[id] == catmullRom.evaluate(t));
                ++id;
            }
        }

        THEN("It can be converted to Bezier")
        {
            Bezier<4, 2, double> expectedBezier{
                b,
                Position<2>{0., 33.333333333333329},
                Position<2>{0., 33.333333333333329},
                c
            };

            REQUIRE(catmullRom.toBezier() == expectedBezier);
        }
    }
}

SCENARIO("Cardinal Cubic comparison.")
{
    Position<2> a{-100.,  -60.};
    Position<2> b{ -20.,   20.};
    Position<2> c{  20.,   20.};
    Position<2> d{ 100.,  -60.};

    GIVEN("Two identical cardinal cubics")
    {
        CardinalCubic<2, double> first{0., a, b, c, d};
        CardinalCubic<2, double> second{0., a, b, c, d};

        THEN("They compare equal")
        {
            REQUIRE(first == second);
            REQUIRE_FALSE(first != second);
        }
    }

    GIVEN("Two different cardinal cubics")
    {
        CardinalCubic<2, double> first{0., a, b, c, d};
        CardinalCubic<2, double> second{0., c, b, d, a};

        THEN("They do not compare equal")
        {
            REQUIRE_FALSE(first == second);
            REQUIRE(first != second);
        }
    }

    GIVEN("Two identical cardinal cubics with different tensions")
    {
        CardinalCubic<2, double> first{0., a, b, c, d};
        CardinalCubic<2, double> second{0.1, a, b, c, d};

        THEN("They do not compare equal")
        {
            REQUIRE_FALSE(first == second);
            REQUIRE(first != second);
        }
    }
}
