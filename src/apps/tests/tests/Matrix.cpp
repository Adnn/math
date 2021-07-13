#include "catch.hpp"

#include "detection.h"
#include "operation_detectors.h"

#include <math/Matrix.h>
#include <math/Range.h>
#include <math/Vector.h>

#include <algorithm>
#include <iostream>
#include <iomanip>


using namespace ad;
using namespace ad::math;


SCENARIO("Matrices types properties")
{
    REQUIRE(std::is_trivially_copyable<Matrix<4,8>>::value);
}


SCENARIO("Matrices have a different factories")
{
    WHEN("Using the Identity factory")
    {
        Matrix<4, 4> identity = Matrix<4, 4>::Identity();

        THEN("The Identity matrix diagonal elements are 1, others are 0")
        {
            bool isDiagonal = true;

            for(std::size_t row : range(identity.Rows))
            {
                for(std::size_t col : range(identity.Cols))
                {
                    if ( ((col==row) && (identity[row][col] != 1.))
                        || ((col!=row) && (identity[row][col] != 0.)) )
                    {
                        isDiagonal = false;
                    }
                }
            }

            REQUIRE(isDiagonal);
        }

        THEN("It can be zeroed")
        {
            identity.setZero();
            REQUIRE(std::all_of(identity.begin(), identity.end(), [](double aValue){ return aValue == 0; }));
        }
    }

    WHEN("Using the Zero factory")
    {
        auto zero = Matrix<3,5>::Zero();
        THEN("All elements are 0")
        {
            REQUIRE(std::all_of(zero.begin(), zero.end(), [](double aValue){ return aValue == 0; }));
        }
    }
}


SCENARIO("Matrix conversion")
{
    GIVEN("A 2x2 Matrix storing doubles")
    {
        Matrix<2, 2> source = {
            1., 2.,
            2., 1.
        };

        THEN("It can be converted to a 2x2 Matrix of ints")
        {
            Matrix<2, 2, int> converted = static_cast<Matrix<2, 2, int>>(source);
            REQUIRE(std::equal(source.begin(), source.end(), converted.begin()));
        }
    }

}


SCENARIO("Basic operations are available on Matrix instances")
{

   GIVEN("A 3x3 Matrix")
    {
        Matrix<3, 3> matrix = {
            1., 2.,  3.,
            5., 55., 5.,
            0., 11., 1.15,
        };

        THEN("Its raw data is contiguous")
        {
            REQUIRE((&matrix[2][2] - &matrix[0][0]) == 8);
        }

        THEN("Its raw data can be accessed as a pointer to first element")
        {
            REQUIRE(*matrix.data() == 1.);
            REQUIRE(*(matrix.data() + 8) == 1.15);
        }

        GIVEN("A scalar factor")
        {
            double factor = 2.67;

            THEN("The matrix can be multiplied by the factor")
            {
                Matrix<3, 3> scaled = matrix * factor;

                REQUIRE(std::equal(matrix.cbegin(), matrix.cend(), scaled.cbegin(),
                                   [factor](double base, double scaled){ return scaled == factor*base; })
                       );

                REQUIRE (matrix != scaled);
                Matrix<3, 3> matrixCopy(matrix);
                REQUIRE( (matrixCopy *= factor) == scaled );
            }

            THEN("The matrix can be divided by the factor")
            {
                Matrix<3, 3> divided = matrix / factor;

                REQUIRE(std::equal(matrix.cbegin(), matrix.cend(), divided.cbegin(),
                                   [factor](double base, double divided){ return divided == base/factor; })
                       );

                Matrix<3, 3> matrixCopy(matrix);
                REQUIRE( (matrixCopy /= factor) == divided);
            }
        }

        THEN("It can be negated with unary minus")
        {
            Matrix<3, 3> negation = -matrix;

            REQUIRE(std::equal(matrix.cbegin(), matrix.cend(), negation.cbegin(),
                               [](double base, double negated){ return negated == -base; })
                   );
        }

        GIVEN("A second 3x3 matrix")
        {
            Matrix<3, 3> second = {
                6., -11., 13.,
                4., -1.,  3.,
                3., 4.,   -2.,
            };

            THEN("First matrix can be assigned to the second")
            {
                REQUIRE_FALSE(std::equal(matrix.begin(), matrix.end(), second.begin()));
                second = matrix;
                REQUIRE(std::equal(matrix.begin(), matrix.end(), second.begin()));
            }

            THEN("They can be multiplied together.")
            {
                Matrix<3, 3> expectedResult = {
                    23.,   -1.,   13.,
                    265.,  -90.,  220.,
                    47.45, -6.4, 30.7,
                };

                REQUIRE((matrix * second) == expectedResult);

                THEN("They can even use compound multiplication.")
                {
                    matrix *= second;
                    REQUIRE(matrix == expectedResult);
                }
            }

            THEN("They can be componentwise multiplied together (Hadamard product).")
            {
                Matrix<3, 3> expectedResult = {
                    6., -22., 39.,
                    20., -55., 15.,
                    0., 44., -2.3,
                };

                REQUIRE((matrix.cwMul(second)) == expectedResult);

                second.cwMulAssign(matrix);
                REQUIRE(second == expectedResult);
            }

            THEN("They can be componentwise divided together (Hadamard division).")
            {
                Matrix<3, 3> expectedResult = {
                    1./6., 2./-11.,  3./13.,
                    5./4., 55./-1, 5./3.,
                    0./3., 11./4., 1.15/-2.,
                };

                REQUIRE((matrix.cwDiv(second)) == expectedResult);

                matrix.cwDivAssign(second);
                REQUIRE(matrix == expectedResult);
            }
        }

        GIVEN("A second 4x3 matrix")
        {
            Matrix<3, 4> second = {
                6., -11., 13., 5.,
                4.,  -1.,  3., 8.,
                3.,   4., -2., 15.,
            };

            THEN("They can be multiplied together.")
            {
                Matrix<3, 4> expectedResult = {
                    23.,   -1.,   13.,  66.,
                    265.,  -90.,  220., 540.,
                    47.45, -6.4,  30.7, 105.25,
                };

                REQUIRE((matrix * second) == expectedResult);
            }
        }
    }
}


SCENARIO("Matrix available operations")
{
    THEN("Two square matrices of same dimension can be compound multiplied")
    {
        REQUIRE(is_detected_v<is_multiplicative_assignable_t,
                                  Matrix<4, 4>, Matrix<4,4>>);

    }
    THEN("Two square matrices of different dimension cannot be compound multiplied")
    {
        REQUIRE_FALSE(is_detected_v<is_multiplicative_assignable_t, Matrix<3,3>, Matrix<4, 4>>);
    }
    THEN("Two multipliable matrices of different size cannot be compound multiplied")
    {
        REQUIRE(is_detected_v<is_multiplicative_t, Matrix<3,3>, Matrix<3, 4>>);
        REQUIRE_FALSE(is_detected_v<is_multiplicative_assignable_t, Matrix<3,3>, Matrix<3, 4>>);
    }
    THEN("Two non-square matrices of same dimension cannot be compound multiplied")
    {
        // Only disabled by static assert
        //REQUIRE_FALSE(is_detected_v<is_multiplicative_assignable_t, Matrix<3,4>, Matrix<3, 4>>);
    }
}


SCENARIO("Naive multiplication")
{
    GIVEN("A 2x2 Matrix")
    {
        Matrix<2, 2> left{
            1., 2.,
            2., 1.
        };

        GIVEN("An 2x2 Identity matrix")
        {
            Matrix<2, 2> right = Matrix<2, 2>::Identity();

            THEN("Mutlipliying them results in the first matrix")
            {
                REQUIRE( (left*right) == left );
            }
        }
    }
}


SCENARIO("Matrix transposition")
{
    GIVEN("A square 2x2 Matrix")
    {
        Matrix<2, 2> source{
            1., 2.,
            3., 4.
        };

        WHEN("It is transposed")
        {
            auto transposed = source.transpose();

            Matrix<2, 2> expected{
                1., 3.,
                2., 4.
            };

            THEN("The result is a square 2x2 Matrix")
            {
                REQUIRE(transposed.Rows == 2);
                REQUIRE(transposed.Cols == 2);
            }

            THEN("The result is transposed")
            {
                REQUIRE(transposed == expected);
            }
        }
    }

    GIVEN("A 4x3 Matrix")
    {
        Matrix<4, 3> source{
             1.,  2.,  3.,
             4.,  5.,  6.,
             7.,  8.,  9.,
            10., 11., 12.,
        };

        WHEN("It is transposed")
        {
            auto transposed = source.transpose();

            Matrix<3, 4> expected{
                 1.,  4.,  7., 10.,
                 2.,  5.,  8., 11.,
                 3.,  6.,  9., 12.,
            };

            THEN("The result is a 3x4 Matrix")
            {
                REQUIRE(transposed.Rows == 3);
                REQUIRE(transposed.Cols == 4);
            }

            THEN("The result is transposed")
            {
                REQUIRE(transposed == expected);
            }
        }
    }
}
