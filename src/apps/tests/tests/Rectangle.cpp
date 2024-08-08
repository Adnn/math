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
            REQUIRE(rect.dimension() == Size<2>{20., 30.});
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
            REQUIRE(rect.origin()      == Position<2>{ 0.,  5.});
            REQUIRE(rect.bottomLeft()  == Position<2>{ 0.,  5.});
            REQUIRE(rect.bottomRight() == Position<2>{20.,  5.});
            REQUIRE(rect.topLeft()     == Position<2>{ 0., 35.});
            REQUIRE(rect.topRight()    == Position<2>{20., 35.});
            REQUIRE(rect.center()      == Position<2>{10., 20.});
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

    THEN("A centered rectangle can be constructed")
    {
        Size<2, double> dimension{10., 100.};
        Rectangle<double> expected{Position<2>::Zero(), dimension};
        REQUIRE(Rectangle<double>::CenterOnOrigin(dimension) == expected.centered());
    }

    THEN("A rectangle extending from the origin can be constructed")
    {
        Size<2, double> dimension{15., 200.};
        Rectangle<double> expected{Position<2>::Zero(), dimension};
        REQUIRE(Rectangle<double>::AtOrigin(dimension) == expected);
    }
}


SCENARIO("Rectangle growing.")
{
    GIVEN("A rectangle")
    {
        const Rectangle<double> base{ {10., -10.}, {5., 5.} };
        Rectangle<double> growing = base;

        WHEN("The rectangle is extended to points already inside it.")
        {
            const Position<2> inside{11., -9.};
            const Position<2> border{10., -10.};
            REQUIRE(growing.contains(inside));
            REQUIRE(growing.contains(border));

            growing.extendTo(inside);
            growing.extendTo(border);

            THEN("It stays the same")
            {
                REQUIRE(growing == base);
            }
        }


        GIVEN("Points around the rectangle")
        {
            const Position<2> left{-10., -10.};
            THEN("It grows the rectangle just enough.")
            {
                growing.extendTo(left);
                REQUIRE(growing.bottomLeft() == left);
                REQUIRE(growing.topRight() == base.topRight());
            }

            const Position<2> top{15., 0.};
            THEN("It grows the rectangle just enough.")
            {
                growing.extendTo(top);
                REQUIRE(growing.topRight() == top);
                REQUIRE(growing.bottomLeft() == base.bottomLeft());
            }
        }

        GIVEN("A point outside the rectangle")
        {
            const Position<2> topLeft{-10., 0.};
            THEN("It grows the rectangle just enough.")
            {
                growing.extendTo(topLeft);
                REQUIRE(growing.topLeft() == topLeft);
                REQUIRE(growing.bottomRight() == base.bottomRight());
            }
        }

        GIVEN("A point outside the rectangle")
        {
            const Position<2> topRight{100., 10.};
            THEN("It grows the rectangle just enough.")
            {
                growing.extendTo(topRight);
                REQUIRE(growing.topRight() == topRight);
                REQUIRE(growing.bottomLeft() == base.bottomLeft());
            }
        }

        GIVEN("A point outside the rectangle")
        {
            const Position<2> bottomRight{100., -100.};
            THEN("It grows the rectangle just enough.")
            {
                growing.extendTo(bottomRight);
                REQUIRE(growing.bottomRight() == bottomRight);
                REQUIRE(growing.topLeft() == base.topLeft());
            }
        }

        GIVEN("A point outside the rectangle")
        {
            const Position<2> bottomLeft{6., -50.};
            THEN("It grows the rectangle just enough.")
            {
                growing.extendTo(bottomLeft);
                REQUIRE(growing.bottomLeft() == bottomLeft);
                REQUIRE(growing.topRight() == base.topRight());
            }
        }
    }
}
