#include "catch.hpp"

#include "detection.h"
#include "operation_detectors.h"

#include <math/LinearMatrix.h>


using namespace ad;
using namespace ad::math;


SCENARIO("Linear matrix construction")
{
    THEN("An Linear matrix can be constructed by specifying each element.")
    {
        LinearMatrix<2, 2> linear{
            1.f, 2.f,
            3.f, 4.f
        };

        double expected = 0.f;
        for (auto element : linear)
        {
            REQUIRE(element == ++expected);
        }
    }
    THEN("An identity Linear matrix can be constructed.")
    {
        LinearMatrix<4, 4> identity = LinearMatrix<4, 4>::Identity();
        REQUIRE(identity == Matrix<4, 4>::Identity());
    }
}


SCENARIO("LinearMatrix available operations")
{
    using Linear4 = LinearMatrix<4, 4>;

    THEN("A 'plain' matrix cannot be converted to a Linear matrix")
    {
        REQUIRE_FALSE(is_detected_v<is_staticcastable_t, Matrix<4, 4>, Linear4>);
    }

    THEN("A Linar matrix is implicitly convertible to a 'plain' matrix")
    {
        REQUIRE(std::is_convertible_v<Linear4, Matrix<4, 4>>);
    }

    THEN("Two square matrices of same dimension can be compound multiplied")
    {
        REQUIRE(is_detected_v<is_multiplicative_assignable_t, Linear4, Linear4>);
        REQUIRE(std::is_same_v<decltype(std::declval<Linear4>() * std::declval<Linear4>()),
                               Linear4>);
    }
    THEN("Two square matrices of different dimension cannot be compound multiplied")
    {
        REQUIRE_FALSE(is_detected_v<is_multiplicative_assignable_t, LinearMatrix<3,3>, LinearMatrix<4, 4>>);
    }
    THEN("Two multipliable matrices of different size cannot be compound multiplied")
    {
        REQUIRE(is_detected_v<is_multiplicative_t, LinearMatrix<3,3>, LinearMatrix<3, 4>>);
        REQUIRE_FALSE(is_detected_v<is_multiplicative_assignable_t, LinearMatrix<3,3>, LinearMatrix<3, 4>>);
    }
    THEN("Two non-square matrices of same dimension cannot be compound multiplied")
    {
        REQUIRE_FALSE(is_detected_v<is_multiplicative_assignable_t, LinearMatrix<3,4>, LinearMatrix<3, 4>>);
    }
}
