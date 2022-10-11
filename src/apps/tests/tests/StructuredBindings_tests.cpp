#include "catch.hpp"
#include <bits/utility.h>
#include <math/Matrix.h>
#include <math/Vector.h>
#include <math/StructuredBindings.h>

using namespace ad::math;

SCENARIO("Structured bindings usage")
{
    GIVEN("A Matrix and a const Matrix")
    {
        Matrix<3, 2, int> matrix{1, 2, 3, 4, 5, 6};
        const Matrix<3, 2, int> constMatrix{1, 2, 3, 4, 5, 6};

        THEN("It can be binded to variables by value")
        {
            auto [a, b, c, d, e, f] = matrix;
            REQUIRE(a == 1);
            REQUIRE(b == 2);
            REQUIRE(c == 3);
            REQUIRE(d == 4);
            REQUIRE(e == 5);
            REQUIRE(f == 6);
        }

        THEN("It can be binded to variables by reference")
        {
            auto & [a, b, c, d, e, f] = matrix;
            REQUIRE(a == 1);
            REQUIRE(b == 2);
            REQUIRE(c == 3);
            REQUIRE(d == 4);
            REQUIRE(e == 5);
            REQUIRE(f == 6);
        }

        THEN("It can be binded to variables by value")
        {
            const auto [a, b, c, d, e, f] = constMatrix;
            REQUIRE(a == 1);
            REQUIRE(b == 2);
            REQUIRE(c == 3);
            REQUIRE(d == 4);
            REQUIRE(e == 5);
            REQUIRE(f == 6);
        }

        THEN("It can be binded to variables by reference")
        {
            const auto & [a, b, c, d, e, f] = constMatrix;
            REQUIRE(a == 1);
            REQUIRE(b == 2);
            REQUIRE(c == 3);
            REQUIRE(d == 4);
            REQUIRE(e == 5);
            REQUIRE(f == 6);
        }
    }

    GIVEN("A Vector and a const Vector")
    {
        Vec<3, int> vec{1, 2, 3};
        const Vec<3, int> constVec{1, 2, 3};

        THEN("It can be binded to variables by value")
        {
            auto [a, b, c] = vec;
            REQUIRE(a == 1);
            REQUIRE(b == 2);
            REQUIRE(c == 3);
        }

        THEN("It can be binded to variables by reference")
        {
            auto & [a, b, c] = vec;
            REQUIRE(a == 1);
            REQUIRE(b == 2);
            REQUIRE(c == 3);
        }

        THEN("It can be binded to const variables by value")
        {
            const auto [a, b, c] = constVec;
            REQUIRE(a == 1);
            REQUIRE(b == 2);
            REQUIRE(c == 3);
        }

        THEN("It can be binded to const variables by reference")
        {
            const auto & [a, b, c] = constVec;
            REQUIRE(a == 1);
            REQUIRE(b == 2);
            REQUIRE(c == 3);
        }
    }
}
