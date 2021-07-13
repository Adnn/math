#include "catch.hpp"

#include <math/Polynomial.h>

using namespace ad::math;


SCENARIO("Polynomials usage")
{
    GIVEN("A polynomial of degree 2 with one real root")
    {
        auto polynomial = Polynomial<2>(1., 2., 1.);
        THEN("It can be evaluated")
        {
            REQUIRE(polynomial.evaluate(1.) == 4.);
            REQUIRE(polynomial.evaluate(3.) == (1. + 2*3. + 1*3.*3.));
        }
        THEN("It can be solved")
        {
            std::vector<double> roots = solve(polynomial);
            REQUIRE(roots.size() == 1);
            REQUIRE(roots.front() == -1.);
        }
    }

    GIVEN("A polynomial of degree 2 with two real roots")
    {
        auto polynomial = Polynomial<2>(1., -4., 2.);
        THEN("It can be solved")
        {
            std::vector<double> roots = solve(polynomial);
            REQUIRE(roots.size() == 2);
            REQUIRE(roots.at(0) == Approx(1. - 1./std::sqrt(2)));
            REQUIRE(roots.at(1) == Approx(1. + 1./std::sqrt(2)));
        }
    }

    GIVEN("A polynomial of degree 2 with two real roots")
    {
        auto polynomial = Polynomial<2>(1., -4., -0.5);
        THEN("It can be solved")
        {
            std::vector<double> roots = solve(polynomial);
            REQUIRE(roots.size() == 2);
            REQUIRE(roots.at(0) == Approx(-8.24264));
            REQUIRE(roots.at(1) == Approx(0.242641));
        }
    }

    GIVEN("A polynomial of degree 4")
    {
        auto polynomial = Polynomial<4>(1., 2., 3., 4., 5.);
        THEN("It can be evaluated")
        {
            REQUIRE(polynomial.evaluate(0.) == polynomial.coefficient(0));
            REQUIRE(polynomial.evaluate(6.3) == Approx(9009.3385));
        }
    }
}
