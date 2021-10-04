#include "catch.hpp"

#include <math/Box.h>

using namespace ad::math;


SCENARIO("Box usage")
{
    GIVEN("A box instance")
    {
        Box<double> box{ {0., 5., 10.}, {20., 30., 40.} };

        THEN("Its position and dimensions are readable")
        {
            REQUIRE(box.x() == 0.);
            REQUIRE(box.y() == 5.);
            REQUIRE(box.z() == 10.);

            REQUIRE(box.width()  == 20.);
            REQUIRE(box.height() == 30.);
            REQUIRE(box.depth()  == 40.);
            REQUIRE(box.dimension() == Size<3>{20., 30., 40.});
        }

        THEN("It can be assigned from another box")
        {
            Box<double> other{ {10., 10., 10.}, {200., 200., 200.} };
            REQUIRE(box != other);
            box = other;
            REQUIRE(box == other);
        }

        THEN("Its corners are accessible")
        {
            REQUIRE(box.origin() == Position<3>{0., 5., 10.});

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
            Box<double> expected{ {-10., -10., 30.}, {20., 30., 40.} };
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
}
