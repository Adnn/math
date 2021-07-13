#include "catch.hpp"

#include <math/Color.h>

#include <type_traits>


using namespace ad;
using namespace ad::math;


SCENARIO("Color types properties")
{
    // Trivially copyable types have great properties for low level manipulation
    // (e.g. file serialization)
    // see: https://en.cppreference.com/w/cpp/types/is_trivially_copyable#Notes
    REQUIRE(std::is_trivially_copyable<sdr::Rgb>::value);
}
