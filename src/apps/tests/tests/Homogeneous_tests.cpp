#include "catch.hpp"

#include "CustomCatchMatchers.h"
#include "operation_detectors.h"

#include <math/Angle.h>
#include <math/Homogeneous.h>
#include <math/LinearMatrix.h>
#include <math/Transformations.h>


using namespace ad;
using namespace ad::math;


SCENARIO("Homogeneous Vectors.")
{
    THEN("An homogeneous position can be constructed from factory")
    {
        Position<3> homoPos = homogeneous::makePosition<3>(10., -6.2);
        REQUIRE(homoPos.x() == 10.);
        REQUIRE(homoPos.y() == -6.2);
        REQUIRE(homoPos.at(2) == 1.);

        THEN("An homogeneous displacement can be constructed from factory")
        {
                Vec<3> homoVec = homogeneous::makeVec<3>(0., 54.2);
                REQUIRE(homoVec.x() == 0.);
                REQUIRE(homoVec.y() == 54.2);
                REQUIRE(homoVec.at(2) == 0.);

                THEN("The displacement can be added to the position")
                {
                    Position<3> offsetPos = homoPos + homoVec;
                    REQUIRE(offsetPos.x() == 10.);
                    REQUIRE(offsetPos.y() == 48.);
                    REQUIRE(offsetPos.at(2) == 1.);
                }
        }

        THEN("Another homogeneous position can be substracted from it, returning a displacement.")
        {
            Position<3> homoOtherPos = homogeneous::makePosition<3>(10., 3.8);
            REQUIRE((homoPos - homoOtherPos) == Vec<3>{0., -10., 0.});
        }
    }
}


SCENARIO("Homogeneous Vectors normalization.")
{
    GIVEN("Homogeneous postions and displacement")
    {
        Position<3> normalizedPos2D{2., -6., 1.};
        Position<3> pos2D{7., 0., -5.};

        Position<4> normalizedVec3D{0., 0.1, 0.01, 1.};
        Position<4> vec3D{7., -7., 7.7, 55.};

        THEN("They can be normalized")
        {
            REQUIRE(normalizedPos2D == homogeneous::homogenize(normalizedPos2D));
            REQUIRE_FALSE(pos2D == homogeneous::homogenize(pos2D));
            REQUIRE(homogeneous::homogenize(pos2D) == pos2D/pos2D.z());

            REQUIRE(normalizedVec3D == homogeneous::homogenize(normalizedVec3D));
            REQUIRE_FALSE(vec3D == homogeneous::homogenize(vec3D));
            REQUIRE(homogeneous::homogenize(vec3D) == vec3D/vec3D.w());
        }
    }
}


SCENARIO("There are separate homogenous types for position and displacement.")
{
    using HomoPos = decltype(homogeneous::makePosition<4>(1., 1., 1.));
    using HomoVec = decltype(homogeneous::makeVec<4>(1., 1., 1.));

    THEN("They are not implicitly convertible from one to the other.")
    {
        REQUIRE_FALSE(std::is_convertible<HomoPos, HomoVec>::value);
        REQUIRE_FALSE(std::is_convertible<HomoVec, HomoPos>::value);
    }

    THEN("Displacement can be added and substracted to another displacement or to a position")
    {
        REQUIRE(is_detected_v<is_additive_t, HomoVec, HomoVec>);
        REQUIRE(is_detected_v<is_additivecompound_t, HomoVec, HomoVec>);

        REQUIRE(is_detected_v<is_additive_t, HomoPos, HomoVec>);
        REQUIRE(is_detected_v<is_additivecompound_t, HomoPos, HomoVec>);

        REQUIRE(is_detected_v<is_substractive_t, HomoVec, HomoVec>);
        REQUIRE(is_detected_v<is_substractivecompound_t, HomoVec, HomoVec>);

        REQUIRE(is_detected_v<is_substractive_t, HomoPos, HomoVec>);
        REQUIRE(is_detected_v<is_substractivecompound_t, HomoPos, HomoVec>);
    }

    THEN("Position cannot be added to neither displacement nor position")
    {
        REQUIRE_FALSE(is_detected_v<is_additive_t, HomoVec, HomoPos>);
        REQUIRE_FALSE(is_detected_v<is_additivecompound_t, HomoVec, HomoPos>);

        REQUIRE_FALSE(is_detected_v<is_additive_t, HomoPos, HomoPos>);
        REQUIRE_FALSE(is_detected_v<is_additivecompound_t, HomoPos, HomoPos>);

    }

    THEN("Position cannot be substracted from a displacement")
    {
        REQUIRE_FALSE(is_detected_v<is_substractive_t, HomoVec, HomoPos>);
        REQUIRE_FALSE(is_detected_v<is_substractivecompound_t, HomoVec, HomoPos>);
    }


    THEN("Position can be substracted from another position")
    {
        REQUIRE(is_detected_v<is_substractive_t, HomoPos, HomoPos>);
        // Substraction two positions (Position) returns a displacement (Vec)
        REQUIRE(std::is_same_v<decltype(std::declval<HomoPos>() - std::declval<HomoPos>()),
                               HomoVec>);

        THEN("But it cannot be compound substracted")
        {
            REQUIRE_FALSE(is_detected_v<is_substractivecompound_t, HomoPos, HomoPos>);
        }
    }
}


SCENARIO("Affine matrices construction")
{
    THEN("Unit affine matrice can be constructed directly.")
    {
        AffineMatrix<6> identity = AffineMatrix<6>::Identity();

        for(int i=0; i != 6; ++i)
        {
            for(int j=0; j != 6; ++j)
            {
                if (i == j)
                {
                    REQUIRE(identity[i][j] == 1);
                }
                else
                {
                    REQUIRE(identity[i][j] == 0);
                }
            }
        }
    }

    GIVEN("An affine matrix of dimension 3 made from a rotation transformation.")
    {
        AffineMatrix<3> homoRotation{trans2d::rotate(Degree<double>{90.})};

        THEN("It has expected elements")
        {
            REQUIRE(homoRotation.equalsWithinTolerance(Matrix<3, 3>{
                                                             0., 1., 0.,
                                                            -1., 0., 0.,
                                                             0., 0., 1.,
                                                       },
                                                       std::numeric_limits<double>::epsilon()));
        }

        THEN("It can rotate an homogeneous position.")
        {
            Position<3> homoPos = homogeneous::makePosition<3>(1., 1.);
            homoPos *= homoRotation;
            REQUIRE(homoPos.equalsWithinTolerance(Position<3>{-1., 1., 1.}, 
                                                  std::numeric_limits<double>::epsilon()));
        }

        THEN("It can rotate an homogeneous displacement.")
        {
            Vec<3> homoVec = homogeneous::makeVec<3>(0., -1.);
            homoVec *= homoRotation;
            REQUIRE(homoVec.equalsWithinTolerance(Vec<3>{1., 0., 0.}, 
                                                  std::numeric_limits<double>::epsilon()));
        }

        THEN("Its affine part is the zero vector")
        {
            REQUIRE(homoRotation.getAffine() == Vec<2>::Zero());
        }
    }

    GIVEN("An affine matrix of dimension 3 made from a rotation and a translation transformations.")
    {
        LinearMatrix<2, 2> rotation = trans2d::rotate(Degree<double>{90.});
        Vec<2> translation{10., -10.};
        AffineMatrix<3> homoTransformation{rotation, translation};

        THEN("It has expected elements")
        {
            REQUIRE(homoTransformation.equalsWithinTolerance(Matrix<3, 3>{
                                                                   0.,   1., 0.,
                                                                  -1.,   0., 0.,
                                                                  10., -10., 1.,
                                                             },
                                                             std::numeric_limits<double>::epsilon()));
        }

        THEN("It rotates and translates an homogeneous position.")
        {
            Position<3> homoPos = homogeneous::makePosition<3>(1., 1.);
            homoPos *= homoTransformation;
            REQUIRE(homoPos.equalsWithinTolerance(Position<3>{9., -9., 1.}, 
                                                  std::numeric_limits<double>::epsilon()));
        }

        THEN("It only rotates an homogeneous displacement.")
        {
            Vec<3> homoVec = homogeneous::makeVec<3>(0., -1.);
            homoVec *= homoTransformation;
            REQUIRE(homoVec.equalsWithinTolerance(Vec<3>{1., 0., 0.}, 
                                                  std::numeric_limits<double>::epsilon()));
        }

        THEN("Its affine part is the translation vector")
        {
            REQUIRE(homoTransformation.getAffine() == translation);
        }

        THEN("Its linear part is the rotation matrix")
        {
            REQUIRE(homoTransformation.getLinear() == rotation);
        }
    }
}


SCENARIO("Affine matrices conversions")
{
    THEN("AffineMatrix can be casted to plain Matrix")
    {
        REQUIRE(is_detected_v<is_staticcastable_t, AffineMatrix<2>, Matrix<2, 2>>);
        REQUIRE(is_detected_v<is_staticcastable_t, AffineMatrix<3>, Matrix<3, 3>>);

        AffineMatrix<2> affine{
            LinearMatrix<1, 1>{1.},
            Vec<1>{3.}
        };

        Matrix<2, 2> plain = static_cast<Matrix<2 ,2>>(affine);

        REQUIRE(plain == Matrix<2, 2>{
            1., 0.,
            3., 1.
        });
    }

    THEN("Plain matrices cannot be cast to Affine Matrices of equal dimension")
    {
        // Note: this very case with conversion to AffineMatrix<2> is a very good reason
        // for making the MatrixBase(Element ...) ctor explicit: otherwise the
        // ctor for AffineMatrix<2> taking a Matrix<1, 1> becomes available, since
        // MatrixBase(Element ...) would be availabe to attempt an implicit conversion
        // from Matrix<2, 2> to the single Element of a Matrix<1, 1>. 
        CHECK_FALSE(is_detected_v<is_staticcastable_t, Matrix<2, 2>, AffineMatrix<2>>);
        CHECK_FALSE(is_detected_v<is_staticcastable_t, Matrix<3, 3>, AffineMatrix<3>>);
    }

    THEN("Affine Matrices of dimension N cannot be cast to Affine Matrices of adjacent dimensions.")
    {
        // Note: these cases from N to N+1 is a rationale for making explicit 
        // the ctor taking a Matrix<N, N>: an AffineMatrix<N> is-a Matrix<N,N>,
        // which would then be implicitly convertible to AffineMatrix<N+1>.
        CHECK_FALSE(std::is_convertible_v<AffineMatrix<2>, AffineMatrix<3>>);
        CHECK_FALSE(std::is_convertible_v<AffineMatrix<3>, AffineMatrix<4>>);

        CHECK_FALSE(is_detected_v<is_staticcastable_t, AffineMatrix<2>, AffineMatrix<3>>);
        CHECK_FALSE(is_detected_v<is_staticcastable_t, AffineMatrix<3>, AffineMatrix<4>>);
        CHECK_FALSE(is_detected_v<is_staticcastable_t, AffineMatrix<2>, AffineMatrix<4>>);

        CHECK_FALSE(is_detected_v<is_staticcastable_t, AffineMatrix<3>, AffineMatrix<2>>);
        CHECK_FALSE(is_detected_v<is_staticcastable_t, AffineMatrix<4>, AffineMatrix<3>>);
        CHECK_FALSE(is_detected_v<is_staticcastable_t, AffineMatrix<4>, AffineMatrix<2>>);
    } 
}

SCENARIO("Affine matrices available operations")
{
    using Affine3 = AffineMatrix<3>;
    using Affine4 = AffineMatrix<4>;
    using Linear2 = LinearMatrix<2, 2>;
    using Linear3 = LinearMatrix<3, 3>;
    using Linear5 = LinearMatrix<5, 5>;
    using Matrix3x3 = Matrix<3, 3>;
    using Matrix3x4 = Matrix<3, 4>;
    using Matrix4x4 = Matrix<4, 4>;

    THEN("Affine matrices can be multiplied with 'plain' matrices of matching dimensions,"
         " resulting in plain matrices.")
    {
        // Affine x Matrix -> Matrix
        REQUIRE(is_detected_v<is_multiplicative_t, Affine4, Matrix4x4>);
        REQUIRE(std::is_same_v<decltype(std::declval<Affine4>() * std::declval<Matrix4x4>()),
                               Matrix4x4>);

        THEN("They cannot be compound multiplied.")
        {
            REQUIRE_FALSE(is_detected_v<is_multiplicative_assignable_t, Affine4, Matrix4x4>);
        }

        THEN("Multiplication is not available if dimensions don't match")
        {
            REQUIRE_FALSE(is_detected_v<is_multiplicative_t, Affine4, Matrix3x4>);
            // Available for Linear, not for plain of dimensions N-1
            REQUIRE_FALSE(is_detected_v<is_multiplicative_t, Affine4, Matrix3x3>);
        }
    }

    THEN("Affine matrices of N dimension can be multiplied with Linear matrices of N-1 dimensions,"
         " resulting in Affine matrices of N dimension.")
    {
        // Affine<N> x LinearMatrix<N-1> -> AffineMatrix<N>
        REQUIRE(is_detected_v<is_multiplicative_t, Affine4, Linear3>);
        REQUIRE(std::is_same_v<decltype(std::declval<Affine4>() * std::declval<Linear3>()),
                               Affine4>);

        THEN("They can be compound multiplied.")
        {
            REQUIRE(is_detected_v<is_multiplicative_assignable_t, Affine4, Linear3>);
        }

        THEN("Multiplication is not available for lower or higher dimensions")
        {
            REQUIRE_FALSE(is_detected_v<is_multiplicative_t, Affine4, Linear2>);
            REQUIRE_FALSE(is_detected_v<is_multiplicative_t, Affine4, Linear5>);
        }
    }

    THEN("'Plain' matrices can be multiplied with affine matrices of matching dimensions.")
    {
        // Matrix x Affine -> Matrix
        REQUIRE(is_detected_v<is_multiplicative_t, Matrix4x4, Affine4>);
        REQUIRE(std::is_same_v<decltype(std::declval<Matrix4x4>() * std::declval<Affine4>()),
                               Matrix4x4>);

        REQUIRE(is_detected_v<is_multiplicative_t, Matrix3x4, Affine4>);
        REQUIRE(std::is_same_v<decltype(std::declval<Matrix3x4>() * std::declval<Affine4>()),
                               Matrix3x4>);

        THEN("They can be compound multiplied.")
        {
            REQUIRE(is_detected_v<is_multiplicative_assignable_t, Matrix4x4, Affine4>);
            REQUIRE(is_detected_v<is_multiplicative_assignable_t, Matrix3x4, Affine4>);
            // Available for Linear, not for plain of dimensions N-1
            REQUIRE_FALSE(is_detected_v<is_multiplicative_t, Matrix3x3, Affine4>);
        }
    }

    THEN("Linear matrices of N-1 dimension can be multiplied with Affine matrices of N dimension,"
         " resulting in Affine matrices of N dimension.")
    {
        // LinearMatrix<N-1> x Affine<N> -> AffineMatrix<N>
        REQUIRE(is_detected_v<is_multiplicative_t, Linear3, Affine4>);
        REQUIRE(std::is_same_v<decltype(std::declval<Linear3>() * std::declval<Affine4>()),
                               Affine4>);

        THEN("They cannot be compound multiplied into the Linear matrix.")
        {
            REQUIRE_FALSE(is_detected_v<is_multiplicative_assignable_t, Linear3, Affine4>);
        }

        THEN("Multiplication is not available for lower or higher dimensions")
        {
            REQUIRE_FALSE(is_detected_v<is_multiplicative_t, Linear2, Affine4>);
            REQUIRE_FALSE(is_detected_v<is_multiplicative_t, Linear5, Affine4>);
        }
    }

    THEN("Affine matrices can be multiplied with affine matrices of matching dimensions.")
    {
        // Affine x Affine -> Affine
        REQUIRE(is_detected_v<is_multiplicative_t, Affine4, Affine4>);
        REQUIRE(std::is_same_v<decltype(std::declval<Affine4>() * std::declval<Affine4>()),
                               Affine4>);

        THEN("They can be compound multiplied.")
        {
            REQUIRE(is_detected_v<is_multiplicative_assignable_t, Affine4, Affine4>);
        }

        THEN("Multiplication is not available if dimensions don't match")
        {
            REQUIRE_FALSE(is_detected_v<is_multiplicative_t, Affine4, Affine3>);
        }
    }

    THEN("Affine matrices can be multiplied by a scalar, yielding 'plain' matrices.")
    {
        // Affine x Scalar -> Matrix
        REQUIRE(is_detected_v<is_multiplicative_t, Affine4, double>);
        REQUIRE(std::is_same_v<decltype(std::declval<Affine4>() * std::declval<double>()),
                               Matrix4x4>);

        // Scalar x Affine -> Matrix
        REQUIRE(is_detected_v<is_multiplicative_t, double, Affine4>);
        REQUIRE(std::is_same_v<decltype(std::declval<double>() * std::declval<Affine4>()),
                               Matrix4x4>);

        THEN("They cannot be compound multiplied.")
        {
            REQUIRE_FALSE(is_detected_v<is_multiplicative_assignable_t, Affine4, double>);
        }
    }

    THEN("Affine matrices can be divided by a scalar, yielding 'plain' matrices.")
    {
        // Affine / Scalar -> Matrix
        REQUIRE(is_detected_v<is_divisive_t, Affine4, double>);
        REQUIRE(std::is_same_v<decltype(std::declval<Affine4>() / std::declval<double>()),
                               Matrix4x4>);

        // Scalar / Affine -> Matrix
        THEN("A scalar cannot be divided by an affine matrix.")
        {
            REQUIRE_FALSE(is_detected_v<is_divisive_t, double, Affine4>);
        }

        THEN("They cannot be compound divided.")
        {
            REQUIRE_FALSE(is_detected_v<is_multiplicative_assignable_t, Affine4, double>);
        }
    }

    THEN("Affine matrices can be added to both 'plain' and affine matrices.")
    {
        // Affine + Affine -> Matrix
        REQUIRE(is_detected_v<is_additive_t, Affine4, Affine4>);
        REQUIRE(std::is_same_v<decltype(std::declval<Affine4>() + std::declval<Affine4>()),
                               Matrix4x4>);

        THEN("Two affine matrices cannot be compound added.")
        {
            REQUIRE_FALSE(is_detected_v<is_additivecompound_t, Affine4, Affine4>);
        }

        // Affine + Matrix -> Matrix
        REQUIRE(is_detected_v<is_additive_t, Affine4, Matrix4x4>);
        REQUIRE(std::is_same_v<decltype(std::declval<Affine4>() + std::declval<Matrix4x4>()),
                               Matrix4x4>);

        THEN("A plain matrice cannot be compound added to an affine matrix.")
        {
            REQUIRE_FALSE(is_detected_v<is_additivecompound_t, Affine4, Matrix4x4>);
        }

        // Matrix + Affine -> Matrix
        REQUIRE(is_detected_v<is_additive_t, Matrix4x4, Affine4>);
        REQUIRE(std::is_same_v<decltype(std::declval<Matrix4x4>() + std::declval<Affine4>()),
                               Matrix4x4>);

        THEN("An affine matrix can be compound added to a plain matrix.")
        {
            REQUIRE(is_detected_v<is_additivecompound_t, Matrix4x4, Affine4>);
        }

        THEN("Addition is not available if dimensions don't match")
        {
            REQUIRE_FALSE(is_detected_v<is_additive_t, Affine4, Affine3>);
            REQUIRE_FALSE(is_detected_v<is_additive_t, Affine4, Matrix3x4>);
        }
    }

    THEN("Affine matrices can be substracted with both 'plain' and affine matrices.")
    {
        // Affine - Affine -> Matrix
        REQUIRE(is_detected_v<is_substractive_t, Affine4, Affine4>);
        REQUIRE(std::is_same_v<decltype(std::declval<Affine4>() - std::declval<Affine4>()),
                               Matrix4x4>);

        THEN("Two affine matrices cannot be compound substracted.")
        {
            REQUIRE_FALSE(is_detected_v<is_substractivecompound_t, Affine4, Affine4>);
        }

        // Affine - Matrix -> Matrix
        REQUIRE(is_detected_v<is_substractive_t, Affine4, Matrix4x4>);
        REQUIRE(std::is_same_v<decltype(std::declval<Affine4>() - std::declval<Matrix4x4>()),
                               Matrix4x4>);

        THEN("A plain matrice cannot be compound substracted from an affine matrix.")
        {
            REQUIRE_FALSE(is_detected_v<is_substractivecompound_t, Affine4, Matrix4x4>);
        }

        // Matrix - Affine -> Matrix
        REQUIRE(is_detected_v<is_substractive_t, Matrix4x4, Affine4>);
        REQUIRE(std::is_same_v<decltype(std::declval<Matrix4x4>() - std::declval<Affine4>()),
                               Matrix4x4>);

        THEN("An affine matrix can be compound added to a plain matrix.")
        {
            REQUIRE(is_detected_v<is_substractivecompound_t, Matrix4x4, Affine4>);
        }

        THEN("Substraction is not available if dimensions don't match")
        {
            REQUIRE_FALSE(is_detected_v<is_substractive_t, Affine4, Affine3>);
            REQUIRE_FALSE(is_detected_v<is_substractive_t, Affine4, Matrix3x4>);
        }
    }
}


SCENARIO("Affine matrices available componentwise operations")
{
    using Affine4 = AffineMatrix<4>;
    using Matrix4x4 = Matrix<4, 4>;

    THEN("Affine matrices can be component-wise multiplied with both plain and affine matrices.")
    {
        REQUIRE(is_detected_v<is_cwmul_t, Affine4, Affine4>);
        REQUIRE(std::is_same_v<decltype(std::declval<Affine4>().cwMul(std::declval<Affine4>())),
                               Matrix4x4>);
        REQUIRE(is_detected_v<is_cwmul_t, Matrix4x4, Affine4>);
        REQUIRE(std::is_same_v<decltype(std::declval<Matrix4x4>().cwMul(std::declval<Affine4>())),
                               Matrix4x4>);
        REQUIRE(is_detected_v<is_cwmul_t, Affine4, Matrix4x4>);
        REQUIRE(std::is_same_v<decltype(std::declval<Affine4>().cwMul(std::declval<Matrix4x4>())),
                               Matrix4x4>);
    }

    THEN("Affine matrices can be component-wise divided with both plain and affine matrices.")
    {
        REQUIRE(is_detected_v<is_cwdiv_t, Affine4, Affine4>);
        REQUIRE(std::is_same_v<decltype(std::declval<Affine4>().cwDiv(std::declval<Affine4>())),
                               Matrix4x4>);
        REQUIRE(is_detected_v<is_cwdiv_t, Matrix4x4, Affine4>);
        REQUIRE(std::is_same_v<decltype(std::declval<Matrix4x4>().cwDiv(std::declval<Affine4>())),
                               Matrix4x4>);
        REQUIRE(is_detected_v<is_cwdiv_t, Affine4, Matrix4x4>);
        REQUIRE(std::is_same_v<decltype(std::declval<Affine4>().cwDiv(std::declval<Matrix4x4>())),
                               Matrix4x4>);
    }
}


SCENARIO("Affine matrices mutliplication.")
{
    GIVEN("An affine matrix of dimension 3.")
    {
        AffineMatrix<3, int> matrix{
            {1, 2,
             1, 1},
            {0, 1}
        };

        THEN("Multiplying by identity give the same matrix")
        {
            REQUIRE(matrix * AffineMatrix<3, int>::Identity() == matrix);
        }

        GIVEN("A second affine matrix of dimension 3.")
        {
            AffineMatrix<3, int> second{
                {5, 5,
                 5, 5},
                {2, 2}
            };

            AffineMatrix<3, int> expected{
                {
                    15, 15,
                    10, 10
                },
                {7,  7}
            };
            THEN("They can be multiplied together")
            {
                REQUIRE(matrix * second == expected);

                matrix *= second;
                REQUIRE(matrix == expected);
            }
        }

        GIVEN("A scalar")
        {
            int scalar = 100;

            Matrix<3, 3, int> expected{
                100, 200,   0,
                100, 100,   0,
                  0, 100, 100
            };

            THEN("The matrix can be multiplied by the scalar.")
            {
                REQUIRE(matrix * scalar == expected);
            }
        }
    }
}


SCENARIO("Affine matrices addition/substraction.")
{
    GIVEN("An affine matrix of dimension 3.")
    {
        AffineMatrix<3, int> matrix{
            {1, 0,
             3, 1},
            {0, 1}
        };

        THEN("Adding the zero matrix give the plain matrix with same values")
        {
            REQUIRE(matrix + Matrix<3, 3, int>::Zero() == matrix);
        }

        GIVEN("A second affine matrix of dimension 3.")
        {
            AffineMatrix<3, int> second{
                {5, 5,
                 5, 5},
                {2, 2}
            };

            Matrix<3, 3, int> expectedAdd{
                6,  5,  0,
                8,  6,  0,
                2,  3,  2
            };

            Matrix<3, 3, int> expectedSub{
                -4,  -5,  0,
                -2,  -4,  0,
                -2,  -1,  0
            };

            THEN("They can be added together")
            {
                REQUIRE(matrix + second == expectedAdd);
                REQUIRE(second + matrix == expectedAdd);

                Matrix<3, 3, int> secondPlain{second};
                secondPlain += matrix;
                REQUIRE(secondPlain == expectedAdd);
            }

            THEN("They can be substracted")
            {
                REQUIRE(matrix - second == expectedSub);

                Matrix<3, 3, int> firstPlain{matrix};
                firstPlain -= second;
                REQUIRE(firstPlain == expectedSub);
            }
        }

        GIVEN("A plain matrix of dimension 3.")
        {
            Matrix<3, 3, int> second{
                5,  0,  2,
                0,  5, -5,
                2,  0, -2,
            };

            Matrix<3, 3, int> expectedAdd{
                6,  0,  2,
                3,  6, -5,
                2,  1, -1, 
            };

            Matrix<3, 3, int> expectedSub{
                -4,   0, -2,
                 3,  -4,  5,
                -2,   1,  3 
            };

            THEN("They can be added together")
            {
                REQUIRE(matrix + second == expectedAdd);
                REQUIRE(second + matrix == expectedAdd);

                second += matrix;
                REQUIRE(second == expectedAdd);
            }

            THEN("They can be substracted")
            {
                REQUIRE(matrix - second == expectedSub);

                Matrix<3, 3, int> firstPlain{matrix};
                firstPlain -= second;
                REQUIRE(firstPlain == expectedSub);
            }
        }
    }
}


SCENARIO("Affine matrices inversion.")
{
    GIVEN("An affine matrix of dimension 4.")
    {
        AffineMatrix<4, double> affine{
            { 1.,  0.,   5.,
              3.,  1.5, -2.,
             -0., -1.5,  2.},
            {0., 1., -8.8}
        };

        WHEN("It is converted to a 'plain' matrix with the same values.")
        {
            Matrix<4, 4, double> plain{affine};

            THEN("Both inverses are equal.")
            {
                CHECK_THAT(affine.inverse(), Approximates(plain.inverse()));
            }
        }
    }
}
