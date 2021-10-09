#include "catch.hpp"

#include "detection.h"
#include "operation_detectors.h"

#include <math/LinearMatrix.h>


using namespace ad;
using namespace ad::math;


SCENARIO("LinearMatrix available operations")
{
    using Linear4 = LinearMatrix<4, 4>;
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
        // Only disabled by static assert
        //REQUIRE_FALSE(is_detected_v<is_multiplicative_assignable_t, LinearMatrix<3,4>, LinearMatrix<3, 4>>);
    }
}
