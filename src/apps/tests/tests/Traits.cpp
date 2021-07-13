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
