#include "catch.hpp"

#include <math/Angle.h>
#include <math/Color.h>
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
    GIVEN("A Position type")
    {
        THEN("It is recognized as a form of MatrixBase")
        {
            REQUIRE(from_matrix_v<Position<3, int>>);
        }
    }
    GIVEN("A Size type")
    {
        THEN("It is recognized as a form of MatrixBase")
        {
            REQUIRE(from_matrix_v<Size<3, int>>);
        }
    }
    GIVEN("A Color type")
    {
        THEN("It is recognized as a form of MatrixBase")
        {
            REQUIRE(from_matrix_v<sdr::Rgba>);
            REQUIRE(from_matrix_v<hdr::Rgb_d>);
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
    GIVEN("A Color type")
    {
        THEN("It is not a position.")
        {
            REQUIRE_FALSE(is_position_v<sdr::Rgba>);
            REQUIRE_FALSE(is_position_v<hdr::Rgb_d>);
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


SCENARIO("Vec type traits.")
{
    GIVEN("Different scalar types")
    {
        THEN("They are **not** Vec")
        {
            REQUIRE_FALSE(is_vec_v<int>);
            REQUIRE_FALSE(is_vec_v<double>);
            REQUIRE_FALSE(is_vec_v<Radian<float>>);
            REQUIRE_FALSE(is_vec_v<std::string>);
        }
    }
    GIVEN("A Position type")
    {
        THEN("It is not a Vec.")
        {
            REQUIRE_FALSE(is_vec_v<Position<3, int>>);
        }
    }
    GIVEN("A Size type")
    {
        THEN("It is not a Vec.")
        {
            REQUIRE_FALSE(is_vec_v<Size<2, float>>);
        }
    }
    GIVEN("A Color type")
    {
        THEN("It is not a Vec.")
        {
            REQUIRE_FALSE(is_vec_v<sdr::Rgba>);
            REQUIRE_FALSE(is_vec_v<hdr::Rgb_d>);
        }
    }
    GIVEN("A Matrix type")
    {
        THEN("It is not a Vec.")
        {
            REQUIRE_FALSE(is_vec_v<Matrix<3, 4, double>>);
        }
    }

    GIVEN("A few Vec types.")
    {
        THEN("They are recognized as Vec.")
        {
            REQUIRE(is_vec_v<Vec<3, int>>);
            REQUIRE(is_vec_v<Vec<6, double>>);
            REQUIRE(is_vec_v<Vec<1, std::uint8_t>>);
        }
    }
}


SCENARIO("Size type traits.")
{
    GIVEN("Different scalar types")
    {
        THEN("They are **not** Size")
        {
            REQUIRE_FALSE(is_size_v<int>);
            REQUIRE_FALSE(is_size_v<double>);
            REQUIRE_FALSE(is_size_v<Radian<float>>);
            REQUIRE_FALSE(is_size_v<std::string>);
        }
    }
    GIVEN("A Position type")
    {
        THEN("It is not a Size.")
        {
            REQUIRE_FALSE(is_size_v<Position<3, int>>);
        }
    }
    GIVEN("A Vec type")
    {
        THEN("It is not a Size.")
        {
            REQUIRE_FALSE(is_size_v<Vec<2, float>>);
        }
    }
    GIVEN("A Color type")
    {
        THEN("It is not a Size.")
        {
            REQUIRE_FALSE(is_size_v<sdr::Rgba>);
            REQUIRE_FALSE(is_size_v<hdr::Rgb_d>);
        }
    }
    GIVEN("A Matrix type")
    {
        THEN("It is not a Size.")
        {
            REQUIRE_FALSE(is_size_v<Matrix<3, 4, double>>);
        }
    }

    GIVEN("A few Size types.")
    {
        THEN("They are recognized as Size.")
        {
            REQUIRE(is_size_v<Size<3, int>>);
            REQUIRE(is_size_v<Size<6, double>>);
            REQUIRE(is_size_v<Size<1, std::uint8_t>>);
        }
    }
}


SCENARIO("Color type traits.")
{
    GIVEN("Different scalar types")
    {
        THEN("They are **not** Color")
        {
            REQUIRE_FALSE(is_color_v<int>);
            REQUIRE_FALSE(is_color_v<double>);
            REQUIRE_FALSE(is_color_v<Radian<float>>);
            REQUIRE_FALSE(is_color_v<std::string>);
        }
    }
    GIVEN("A Position type")
    {
        THEN("It is not a Color.")
        {
            REQUIRE_FALSE(is_color_v<Position<3, int>>);
        }
    }
    GIVEN("A Vec type")
    {
        THEN("It is not a Color.")
        {
            REQUIRE_FALSE(is_color_v<Vec<2, float>>);
        }
    }
    GIVEN("A Size type")
    {
        THEN("It is not a Color.")
        {
            REQUIRE_FALSE(is_color_v<Size<2, float>>);
        }
    }
    GIVEN("A Matrix type")
    {
        THEN("It is not a Color.")
        {
            REQUIRE_FALSE(is_color_v<Matrix<3, 4, double>>);
        }
    }

    GIVEN("A few Color types.")
    {
        THEN("They are recognized as Color.")
        {
            REQUIRE(is_color_v<sdr::Rgb>);
            REQUIRE(is_color_v<sdr::Rgba>);
            REQUIRE(is_color_v<sdr::Grayscale>);

            REQUIRE(is_color_v<hdr::Rgb_d>);
            REQUIRE(is_color_v<hdr::Rgba_d>);
            REQUIRE(is_color_v<hdr::Grayscale_d>);


            REQUIRE(is_color_v<Rgb_base<int>>);
            REQUIRE(is_color_v<RgbAlpha_base<float>>);
        }
    }
}
