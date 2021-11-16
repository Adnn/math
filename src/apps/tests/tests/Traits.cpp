#include "catch.hpp"

#include <math/Angle.h>
#include <math/MatrixTraits.h>
#include <math/Vector.h>

#include <string>


using namespace ad::math;


SCENARIO("Matrix type traits")
{
    GIVEN("Different scalar types")
    {
        THEN("They are **not** coming form MatrixBase")
        {
            REQUIRE_FALSE(from_matrix_v<int>);
            REQUIRE_FALSE(from_matrix_v<double>);
            REQUIRE_FALSE(from_matrix_v<Radian<float>>);
            REQUIRE_FALSE(from_matrix_v<std::string>);
        }
    }
    GIVEN("A Vec type")
    {
        THEN("It is recognized as a form of MatrixBase")
        {
            REQUIRE(from_matrix_v<Vec<3, int>>);
        }
    }
    GIVEN("A Matrix type")
    {
        THEN("It is recognized as a form of MatrixBase")
        {
            REQUIRE(from_matrix_v<Matrix<3, 4, double>>);
        }
    }
}


SCENARIO("Position type traits.")
{
    GIVEN("Different scalar types")
    {
        THEN("They are **not** position")
        {
            REQUIRE_FALSE(is_position_v<int>);
            REQUIRE_FALSE(is_position_v<double>);
            REQUIRE_FALSE(is_position_v<Radian<float>>);
            REQUIRE_FALSE(is_position_v<std::string>);
        }
    }
    GIVEN("A Vec type")
    {
        THEN("It is not a position.")
        {
            REQUIRE_FALSE(is_position_v<Vec<3, int>>);
        }
    }
    GIVEN("A Size type")
    {
        THEN("It is not a position.")
        {
            REQUIRE_FALSE(is_position_v<Size<2, float>>);
        }
    }
    GIVEN("A Matrix type")
    {
        THEN("It is not a position.")
        {
            REQUIRE_FALSE(is_position_v<Matrix<3, 4, double>>);
        }
    }

    GIVEN("A few Position types.")
    {
        THEN("They are recognized as positions.")
        {
            REQUIRE(is_position_v<Position<3, int>>);
            REQUIRE(is_position_v<Position<6, double>>);
            REQUIRE(is_position_v<Position<1, std::uint8_t>>);
        }
    }
}
