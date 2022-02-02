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


SCENARIO("Conversions between HDR and SDR.")
{
    GIVEN("A SDR rgb color")
    {
        sdr::Rgb rgb_sdr{5, 50, 150};

        THEN("It can be converted to HDR rgb")
        {
            hdr::Rgb rgb_hdr{to_hdr(rgb_sdr)};

            REQUIRE(rgb_hdr.r() == 5./255);
            REQUIRE(rgb_hdr.g() == 50./255);
            REQUIRE(rgb_hdr.b() == 150./255);
        }
    }

    GIVEN("A SDR rgba color")
    {
        sdr::Rgba rgba_sdr{5, 50, 150, 33};

        THEN("It can be converted to HDR rgba")
        {
            hdr::Rgba rgba_hdr{to_hdr(rgba_sdr)};

            REQUIRE(rgba_hdr.r() == 5./255);
            REQUIRE(rgba_hdr.g() == 50./255);
            REQUIRE(rgba_hdr.b() == 150./255);
            REQUIRE(rgba_hdr.a() == 33./255);
        }
    }

    GIVEN("A HDR rgb color")
    {
        // Overshoot in blue
        hdr::Rgb rgb_hdr{0.5, 0.001, 1.2};

        THEN("It can be converted to SDR rgb")
        {
            sdr::Rgb rgb_sdr{to_sdr(rgb_hdr)};

            REQUIRE(rgb_sdr.r() == 127);
            REQUIRE(rgb_sdr.g() == 0);
            REQUIRE(rgb_sdr.b() == 255);
        }
    }
    
    GIVEN("A HDR rgba color")
    {
        // Overshoot in blue
        hdr::Rgba rgba_hdr{0.5, 0.001, 1.2, 1./255};

        THEN("It can be converted to SDR rgba")
        {
            sdr::Rgba rgba_sdr{to_sdr(rgba_hdr)};

            REQUIRE(rgba_sdr.r() == 127);
            REQUIRE(rgba_sdr.g() == 0);
            REQUIRE(rgba_sdr.b() == 255);
            REQUIRE(rgba_sdr.a() == 1);
        }
    }
}
