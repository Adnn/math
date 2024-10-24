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

                    THEN("RGBA can be swizzled to RGB, and compares equal to RGB")
                    {
                        CHECK(sdr::Rgb{rgba} == rgb);
                    }
                }
            }
        }
    }
    
    GIVEN("An hdr RGB color")
    {
        hdr::Rgb_d rgb{0.2, 0.645, 0.003};

        THEN("It can be implicitly converted to hdr RGBA")
        {
            hdr::Rgba_d rgba = rgb;

            REQUIRE(rgba.r() == rgb.r());
            REQUIRE(rgba.g() == rgb.g());
            REQUIRE(rgba.b() == rgb.b());
            REQUIRE(rgba.a() == 1.0);

            THEN("Equality can be tested between RGBA and RGB, relying on implicit conversion.")
            {
                REQUIRE(rgba == rgb);
                REQUIRE_FALSE(rgba != rgb);

                // The test is order independent.
                REQUIRE_FALSE(rgba == hdr::gBlack<>);
                REQUIRE_FALSE(hdr::gBlack<> == rgba);
                REQUIRE(rgba != hdr::gBlack<>);
                REQUIRE(hdr::gBlack<> != rgba);
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
            hdr::Rgb_d rgb_hdr{to_hdr(rgb_sdr)};

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
            hdr::Rgba_d rgba_hdr{to_hdr(rgba_sdr)};

            REQUIRE(rgba_hdr.r() == 5./255);
            REQUIRE(rgba_hdr.g() == 50./255);
            REQUIRE(rgba_hdr.b() == 150./255);
            REQUIRE(rgba_hdr.a() == 33./255);
        }
    }

    GIVEN("A HDR rgb color")
    {
        // Overshoot in blue
        hdr::Rgb_d rgb_hdr{0.499, 0.001, 1.2};

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
        // Overshoot in blue, undershoot in green
        hdr::Rgba_d rgba_hdr{0.5, -0.01, 1.2, 1./255};

        THEN("It can be converted to SDR rgba")
        {
            sdr::Rgba rgba_sdr{to_sdr(rgba_hdr)};

            REQUIRE(rgba_sdr.r() == 128); // midway rounds away from zero
            REQUIRE(rgba_sdr.g() == 0);
            REQUIRE(rgba_sdr.b() == 255);
            REQUIRE(rgba_sdr.a() == 1);
        }
    }
}


SCENARIO("Conversions from sRGB color space.")
{
    GIVEN("A SDR RGB color in sRGB color space.")
    {
        sdr::Rgb sRgb{0, 127, 255};

        WHEN("It is decoded to linear space.")
        {
            sdr::Rgb linearRgb = decode_sRGB(sRgb);
            THEN("The values are as expected.")
            {
                sdr::Rgb expected(0, 54, 255);
                CHECK(linearRgb == expected);
            }
        }
    }
    
    GIVEN("A SDR RGBA color in sRGB color space.")
    {
        sdr::Rgba sRgba{100, 200, 10, 127};

        WHEN("It is decoded to linear space.")
        {
            sdr::Rgba linearRgba = decode_sRGB(sRgba);
            THEN("The color channels values are as expected.")
            {
                sdr::Rgb expected(32, 147, 1);
                CHECK(sdr::Rgb{linearRgba} == expected);
            }
            THEN("The opacity channel is not affected.")
            {
                CHECK(linearRgba.a() == sRgba.a());
            }
        }
    }

    GIVEN("A HDR RGB color in sRGB color space.")
    {
        hdr::Rgb_f sRgb{0.f, 0.5f, 1.f};

        WHEN("It is decoded to linear space.")
        {
            hdr::Rgb_f linearRgb = decode_sRGB(sRgb);
            THEN("The values are as expected.")
            {
                hdr::Rgb_f expected(0.f, decode_sRGBChannel(sRgb.g()), 1.f);
                CHECK(linearRgb == expected);
            }
        }
    }
    
    GIVEN("A SDR RGBA color in sRGB color space.")
    {
        hdr::Rgba_f sRgba{100/255.f, 200/255.f, 10/255.f, 0.321f};

        WHEN("It is decoded to linear space.")
        {
            hdr::Rgba_f linearRgba = decode_sRGB(sRgba);
            THEN("The color channels values are as expected.")
            {
                hdr::Rgb_f expected{
                    decode_sRGBChannel(sRgba.r()), 
                    decode_sRGBChannel(sRgba.g()), 
                    decode_sRGBChannel(sRgba.b()), 
                };
                CHECK(hdr::Rgb_f{linearRgba} == expected);
            }
            THEN("The opacity channel is not affected.")
            {
                CHECK(linearRgba.a() == sRgba.a());
            }
        }
    }
}