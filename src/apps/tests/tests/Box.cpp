#include "catch.hpp"

#include "CustomCatchMatchers.h"

#include <math/Box.h>
#include <math/Transformations.h>


using namespace ad::math;


SCENARIO("Box usage")
{
    GIVEN("A box instance")
    {
        Box<double> box{ {0., 5., -30.}, {20., 30., 40.} };

        THEN("Its position and dimensions are readable")
        {
            REQUIRE(box.x() == 0.);
            REQUIRE(box.y() == 5.);
            REQUIRE(box.z() == -30.);

            REQUIRE(box.width()  == 20.);
            REQUIRE(box.height() == 30.);
            REQUIRE(box.depth()  == 40.);
            REQUIRE(box.dimension() == Size<3>{20., 30., 40.});
        }

        THEN("It can be assigned from another box")
        {
            Box<double> other{ {10., 10., -190.}, {200., 200., 200.} };
            REQUIRE(box != other);
            box = other;
            REQUIRE(box == other);
        }

        THEN("Its corners are accessible")
        {
            REQUIRE(box.origin() == Position<3>{0., 5., -30.});

            REQUIRE(box.bottomLeftFront()  == Position<3>{ 0.,  5.,  10.});
            REQUIRE(box.bottomLeftBack()   == Position<3>{ 0.,  5., -30.});
            REQUIRE(box.bottomRightFront() == Position<3>{20.,  5.,  10.});
            REQUIRE(box.bottomRightBack()  == Position<3>{20.,  5., -30.});
            REQUIRE(box.topLeftFront()     == Position<3>{ 0., 35.,  10.});
            REQUIRE(box.topLeftBack()      == Position<3>{ 0., 35., -30.});
            REQUIRE(box.topRightFront()    == Position<3>{20., 35.,  10.});
            REQUIRE(box.topRightBack()     == Position<3>{20., 35., -30.});
            REQUIRE(box.center()           == Position<3>{10., 20., -10.});
        }

        THEN("It can generate a centered box")
        {
            Box<double> expected{ {-10., -10., -50.}, {20., 30., 40.} };
            REQUIRE(box.centered() == expected);
        }

        THEN("It can provide its front rectangle")
        {
            REQUIRE(box.frontRectangle() == Rectangle<double>{{0., 5.}, {20., 30.}});
        }


        GIVEN("A position inside the box")
        {
            Position<3, double> in{10., 15., 5.};

            THEN("Its inclusion can be tested")
            {
                REQUIRE(box.contains(in));
            }

            THEN("The closest position is itself")
            {
                REQUIRE(box.closestPoint(in) == in);
            }
        }

        GIVEN("Several positions outside the box")
        {
            Position<3, int> left{-2, 15, 5};
            Position<3, float> top{10.f, 45.f, 5.f};
            Position<3, double> right{100., 15., 5.};
            Position<3, unsigned int> bottom{3u, 2u, 5u};
            Position<3, double> top_right{100., 45., 5.};

            Position<3> front{10., 10.,  20.};
            Position<3> back {10., 10., -35.};

            THEN("The box does not contain them")
            {
                REQUIRE_FALSE(box.contains(left));
                REQUIRE_FALSE(box.contains(top));
                REQUIRE_FALSE(box.contains(right));
                REQUIRE_FALSE(box.contains(bottom));
                REQUIRE_FALSE(box.contains(top_right));
                REQUIRE_FALSE(box.contains(front));
                REQUIRE_FALSE(box.contains(back));
            }

            THEN("Their closest position can be computed")
            {
                REQUIRE(box.closestPoint(static_cast<Position<3, double>>(left))
                        == Position<3, double>{0., 15., 5.});
                REQUIRE(box.closestPoint(static_cast<Position<3, double>>(top))
                        == Position<3, double>{10., 35., 5.});
                REQUIRE(box.closestPoint(right)
                        == Position<3, double>{20., 15., 5.});
                REQUIRE(box.closestPoint(static_cast<Position<3, double>>(bottom))
                        == Position<3, double>{3., 5., 5.});
                REQUIRE(box.closestPoint(top_right)
                        == Position<3, double>{20., 35., 5.});
                REQUIRE(box.closestPoint(front)
                        == Position<3>{10., 10., 10.});
                REQUIRE(box.closestPoint(back)
                        == Position<3>{10., 10., -30.});
            }
        }
    }

    THEN("A centered box can be constructed")
    {
        Size<3, double> dimension{10., 100., 1000.};
        Box<double> expected{Position<3>::Zero(), dimension};
        REQUIRE(Box<double>::CenterOnOrigin(dimension) == expected.centered());
    }
}


SCENARIO("Box growing from points.")
{
    GIVEN("A box")
    {
        const Box<double> base{ {10., -10., -5.}, {5., 5., 5.} };
        Box<double> growing = base;

        WHEN("The box is extended to points already inside it.")
        {
            const Position<3> inside{11., -9., -1.};
            const Position<3> border{10., -10., 0.};
            REQUIRE(growing.contains(inside));
            REQUIRE(growing.contains(border));

            growing.extendTo(inside);
            growing.extendTo(border);

            THEN("It stays the same")
            {
                REQUIRE(growing == base);
            }
        }

        //
        // Front
        //
        GIVEN("A point outside the box")
        {
            const Position<3> topLeftFront{-10., 0., 5.};
            THEN("It grows the box just enough.")
            {
                growing.extendTo(topLeftFront);
                REQUIRE(growing.topLeftFront() == topLeftFront);
                REQUIRE(growing.bottomRightBack() == base.bottomRightBack());
            }
        }

        GIVEN("A point outside the box")
        {
            const Position<3> topRightFront{100., 10., 5.};
            THEN("It grows the box just enough.")
            {
                growing.extendTo(topRightFront);
                REQUIRE(growing.topRightFront() == topRightFront);
                REQUIRE(growing.bottomLeftBack() == base.bottomLeftBack());
            }
        }

        GIVEN("A point outside the box")
        {
            const Position<3> bottomRightFront{100., -100., 5.};
            THEN("It grows the box just enough.")
            {
                growing.extendTo(bottomRightFront);
                REQUIRE(growing.bottomRightFront() == bottomRightFront);
                REQUIRE(growing.topLeftBack() == base.topLeftBack());
            }
        }

        GIVEN("A point outside the box")
        {
            const Position<3> bottomLeftFront{6., -50., 5.};
            THEN("It grows the box just enough.")
            {
                growing.extendTo(bottomLeftFront);
                REQUIRE(growing.bottomLeftFront() == bottomLeftFront);
                REQUIRE(growing.topRightBack() == base.topRightBack());
            }
        }

        //
        // Back
        //
        GIVEN("A point outside the box")
        {
            const Position<3> topLeftBack{-10., 0., -15.};
            THEN("It grows the box just enough.")
            {
                growing.extendTo(topLeftBack);
                REQUIRE(growing.topLeftBack() == topLeftBack);
                REQUIRE(growing.bottomRightFront() == base.bottomRightFront());
            }
        }

        GIVEN("A point outside the box")
        {
            const Position<3> topRightBack{100., 10., -15.};
            THEN("It grows the box just enough.")
            {
                growing.extendTo(topRightBack);
                REQUIRE(growing.topRightBack() == topRightBack);
                REQUIRE(growing.bottomLeftFront() == base.bottomLeftFront());
            }
        }

        GIVEN("A point outside the box")
        {
            const Position<3> bottomRightBack{100., -100., -15.};
            THEN("It grows the box just enough.")
            {
                growing.extendTo(bottomRightBack);
                REQUIRE(growing.bottomRightBack() == bottomRightBack);
                REQUIRE(growing.topLeftFront() == base.topLeftFront());
            }
        }

        GIVEN("A point outside the box")
        {
            const Position<3> bottomLeftBack{6., -50., -15.};
            THEN("It grows the box just enough.")
            {
                growing.extendTo(bottomLeftBack);
                REQUIRE(growing.bottomLeftBack() == bottomLeftBack);
                REQUIRE(growing.topRightFront() == base.topRightFront());
            }
        }
    }
}


SCENARIO("Box boolean operations.")
{
    GIVEN("A box")
    {
        const Box<double> base{ {10., -10., -5.}, {5., 5., 5.} };

        THEN("Self union results in the same box.")
        {
            CHECK(base.unite(base) == base);
        }

        GIVEN("Another box entirely contained in the first")
        {
            const Box<double> other{ {11., -8., -5.}, {2., 2., 5.} };

            THEN("Union gives the original box, in both directions.")
            {
                CHECK(base.unite(other) == base);
                CHECK(other.unite(base) == base);
            }
        }

        GIVEN("A distinct box (not contained).")
        {
            const Box<double> other{ {80., 80., 120.}, {10., 20., 30.} };

            THEN("Union gives the the expected grown box.")
            {
                const Box<double> expected{ {10., -10., -5.}, {80., 110., 155.} };
                CHECK(base.unite(other) == expected);
                CHECK(other.unite(base) == expected);

                THEN("Assignment-union mutates the left operand to match expectations.")
                {
                    auto otherCopy = other;
                    REQUIRE(otherCopy == other); // Sanity check

                    CHECK(otherCopy.uniteAssign(base) == expected);
                    CHECK(otherCopy == expected);
                    CHECK(otherCopy != other);
                }
            }
        }
    }
}


SCENARIO("Box transformations.")
{
    GIVEN("A box")
    {
        const Box<double> base{ {0., 10., -10.}, {20., 10., 20.} };
        GIVEN("A translation and scaling transformation.")
        {
            Size<3> scaling{2., -1., 0.5};
            Vec<3> translation{10., -10., 10.};

            AffineMatrix<4> transformation = trans3d::scale(scaling) * trans3d::translate(translation);

            THEN("The box can be transformed.")
            {
                Box<double> transformed = base * transformation;
                //CHECK(transformed.origin() == Position<3>{10., -20., 5.});
                //CHECK(transformed.dimension() == base.dimension().cwMul(scaling));
                CHECK(transformed.origin() == Position<3>{10., -30., 5.});
                CHECK(transformed.dimension() == Size<3>{40., 10., 10.});
            }
        }
    }

    GIVEN("A cube centered on origin")
    {
        const auto cube = Box<double>::CenterOnOrigin({10., 10., 10.});
        Position<3> origin{-5., -5., -5.};

        // Sanity check
        REQUIRE(cube.origin() == origin);

        GIVEN("A 45deg rotation transformation (not axis aligned).")
        {
            LinearMatrix<3, 3> rotation = trans3d::rotateY(Radian{pi<double>/4});

            THEN("The box can be transformed.")
            {
                const double sq2 = std::sqrt(2);
                Box<double> transformed = cube * rotation;
                CHECK_THAT(transformed.origin(), Approximates(origin.cwMul({sq2, 1., sq2})));
                CHECK_THAT(transformed.dimension(), Approximates(cube.dimension().cwMul({sq2, 1., sq2})));
            }
        }
    }

    GIVEN("A unit cube whose origin is at the origin")
    {
        const auto cube = Box<double>({0., 0., 0.}, {1., 1., 1.});

        GIVEN("A 45deg rotation transformation (not axis aligned).")
        {
            LinearMatrix<3, 3> rotation = trans3d::rotateY(Radian{pi<double>/4});

            THEN("The box can be transformed.")
            {
                const double sq2 = std::sqrt(2);
                Box<double> transformed = cube * rotation;
                CHECK_THAT(transformed.origin(), Approximates(Position<3>{0., 0., -sq2 / 2.}));
                CHECK_THAT(transformed.dimension(), Approximates(cube.dimension().cwMul({sq2, 1., sq2})));
            }
        }
    }
}
