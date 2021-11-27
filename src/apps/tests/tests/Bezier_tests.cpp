#include "catch.hpp"

#include <math/Interpolation/Interpolation.h>
#include <math/Curves/Bezier.h>


using namespace ad::math;


SCENARIO("Bezier construction and basic accesses.")
{
    GIVEN("A degree 2 Bezier (line)")
    {
        Position<3> a{100., 100., 0.};
        Position<3> b{500., 100., 0.};
        Bezier<2, 3> bezier{a, b};

        THEN("Endpoints can be accessed")
        {
            REQUIRE(bezier[0] == a);
            REQUIRE(bezier.start() == a);

            REQUIRE(bezier[1] == b);
            REQUIRE(bezier.end() == b);
        }
    }
}


SCENARIO("Bezier evaluation and subdivision")
{
    GIVEN("A degree 2 Bezier (line)")
    {
        Position<3> a{100., 100., 0.};
        Position<3> b{500., 100., 0.};
        Bezier<2, 3> bezier{a, b};

        WHEN("It is evaluated in the middle")
        {
            const double t = 0.5;
            Position<3> middle = evaluate(bezier, t);

            THEN("The resulting position is the midpoint of endpoints")
            {
                REQUIRE(middle == lerp(a, b, t));
            }

            WHEN("It is subdived in the middle")
            {
                auto [left, right] = subdivide(bezier, t);

                THEN("The left endpoint is the right startpoint")
                {
                    REQUIRE(left.end() == right.start());
                }

                THEN("The common endpoint corresponds to the evaluated point.")
                {
                    REQUIRE(left.end() == middle);
                }
            }
        }
    }

    GIVEN("A degree 4 Bezier (cubic)")
    {
        Position<2> p0{-200.,   0.};
        Position<2> p1{-100., 500.};
        Position<2> p2{ 100., 500.};
        Position<2> p3{ 200.,   0.};
        Bezier<4, 2> bezier{p0, p1, p2, p3};

        WHEN("It is evaluated in the middle")
        {
            const double t = 0.5;
            Position<2> middle = evaluate(bezier, t);

            THEN("The resulting position is on the curve.")
            {
                // Precomputed
                REQUIRE(middle == Position<2>{0., 375.});
            }

            WHEN("It is subdived in the middle")
            {
                auto [left, right] = subdivide(bezier, t);

                THEN("The left endpoint is the right startpoint")
                {
                    REQUIRE(left.end() == right.start());
                }

                THEN("The common endpoint corresponds to the evaluated point.")
                {
                    REQUIRE(left.end() == middle);
                }

                THEN("The common endpint handles can be matched")
                {
                    REQUIRE( (right[1] - right[0]) == (left[3] - left[2]) );
                }

                THEN("The subdivision evaluations can be matched against toplevel curve evaluations.")
                {
                    // sensitive to numerical errors...
                    {
                        double p = 0.5;
                        REQUIRE(evaluate(bezier, t * p) == evaluate(left, p));
                        REQUIRE(evaluate(bezier, t + t * p) == evaluate(right, p));
                    }
                }
            }
        }
    }
}
