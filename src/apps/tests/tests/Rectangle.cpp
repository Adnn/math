#include "catch.hpp"

#include <math/Rectangle.h>

using namespace ad::math;


SCENARIO("Rectangle usage")
{
    GIVEN("A rectangle instance")
    {
        Rectangle<double> rect{ {0., 5.}, {20., 30.} };

        THEN("Its position and dimensions are readable")
        {
            REQUIRE(rect.x() == 0.);
            REQUIRE(rect.y() == 5.);

            REQUIRE(rect.width() == 20.);
            REQUIRE(rect.height() == 30.);
        }

        THEN("It can be assigned from another rectangle")
        {
            Rectangle<double> other{ {10., 10.}, {200., 200.} };
            REQUIRE(rect != other);
            rect = other;
            REQUIRE(rect == other);
        }

        THEN("Its corners are accessible")
        {
            REQUIRE(rect.origin() == Position<2>{0., 5.});
            REQUIRE(rect.topRight() == Position<2>{20., 35.});
        }

        THEN("It can generate a centered rectangle")
        {
            Rectangle<double> expected{ {-10., -10.}, {20., 30.} };
            REQUIRE(rect.centered() == expected);
        }

        GIVEN("A position inside the rectangle")
        {
            Position<2, double> in{10., 15.};

            THEN("Its inclusion can be tested")
            {
                REQUIRE(rect.contains(in));
            }

            THEN("The closest position is itself")
            {
                REQUIRE(rect.closestPoint(in) == in);
            }
        }

        GIVEN("Several positions outside the rectangle")
        {
            Position<2, int> left{-2, 15};
            Position<2, float> top{10.f, 45.f};
            Position<2, double> right{100., 15.};
            Position<2, unsigned int> bottom{3u, 2u};
            Position<2, double> top_right{100., 45.};

            THEN("The rectangle does not contain them")
            {
                REQUIRE_FALSE(rect.contains(left));
                REQUIRE_FALSE(rect.contains(top));
                REQUIRE_FALSE(rect.contains(right));
                REQUIRE_FALSE(rect.contains(bottom));
                REQUIRE_FALSE(rect.contains(top_right));
            }

            THEN("Their closest position can be computed")
            {
                REQUIRE(rect.closestPoint(static_cast<Position<2, double>>(left))
                        == Position<2, double>{0., 15.});
                REQUIRE(rect.closestPoint(static_cast<Position<2, double>>(top))
                        == Position<2, double>{10., 35.});
                REQUIRE(rect.closestPoint(right)
                        == Position<2, double>{20., 15.});
                REQUIRE(rect.closestPoint(static_cast<Position<2, double>>(bottom))
                        == Position<2, double>{3., 5.});
                REQUIRE(rect.closestPoint(top_right)
                        == Position<2, double>{20., 35.});
            }
        }
    }
}
