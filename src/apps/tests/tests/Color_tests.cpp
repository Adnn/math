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


SCENARIO("RGBA colors constructions")
{
    GIVEN("A sdr RGB color")
    {
        sdr::Rgb rgb{5, 50, 150};

        THEN("It can be implicitly converted to sdr RGBA")
        {
            sdr::Rgba rgba = rgb;

            REQUIRE(rgba.r() == rgb.r());
            REQUIRE(rgba.g() == rgb.g());
            REQUIRE(rgba.b() == rgb.b());
            REQUIRE(rgba.a() == 255);

            THEN("Equality can be tested between RGBA and RGB, relying on implicit conversion.")
            {
                REQUIRE(rgba == rgb);
                REQUIRE_FALSE(rgba != rgb);

                // The test is order independent.
                REQUIRE_FALSE(rgba == sdr::gBlack);
                REQUIRE_FALSE(sdr::gBlack == rgba);
                REQUIRE(rgba != sdr::gBlack);
                REQUIRE(sdr::gBlack != rgba);
            }

            WHEN("RGBA alpha is changed to not be 100%")
            {
                rgba.a() = 127;

                THEN("RGBA does not compare equal to RGB anymore")
                {
                    REQUIRE(rgba != rgb);
                    REQUIRE_FALSE(rgba == rgb);
                    REQUIRE(rgb != rgba);
                    REQUIRE_FALSE(rgb == rgba);
                }
            }
        }
    }
    
    GIVEN("An hdr RGB color")
    {
        hdr::Rgb rgb{0.2, 0.645, 0.003};

        THEN("It can be implicitly converted to hdr RGBA")
        {
            hdr::Rgba rgba = rgb;

            REQUIRE(rgba.r() == rgb.r());
            REQUIRE(rgba.g() == rgb.g());
            REQUIRE(rgba.b() == rgb.b());
            REQUIRE(rgba.a() == 1.0);

            THEN("Equality can be tested between RGBA and RGB, relying on implicit conversion.")
            {
                REQUIRE(rgba == rgb);
                REQUIRE_FALSE(rgba != rgb);

                // The test is order independent.
                REQUIRE_FALSE(rgba == hdr::gBlack);
                REQUIRE_FALSE(hdr::gBlack == rgba);
                REQUIRE(rgba != hdr::gBlack);
                REQUIRE(hdr::gBlack != rgba);
            }

            WHEN("RGBA alpha is changed to not be 100%")
            {
                rgba.a() = 0.5;

                THEN("RGBA does not compare equal to RGB anymore")
                {
                    REQUIRE(rgba != rgb);
                    REQUIRE_FALSE(rgba == rgb);
                    REQUIRE(rgb != rgba);
                    REQUIRE_FALSE(rgb == rgba);
                }
            }
        }
    }
}
