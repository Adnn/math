#include "catch.hpp"

#include "operation_detectors.h"

#include <math/Angle.h>
#include <math/Homogeneous.h>
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
    }

    GIVEN("An affine matrix of dimension 3 made from a rotation and a translation transformations.")
    {
        AffineMatrix<3> homoTransformation{trans2d::rotate(Degree<double>{90.}), Vec<2>{10., -10.}};

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
    }
}


SCENARIO("Affine matrices available operations")
{
    using Affine3 = AffineMatrix<3>;
    using Affine4 = AffineMatrix<4>;
    using Matrix4x4 = Matrix<4, 4>;
    using Matrix3x4 = Matrix<3, 4>;

    THEN("Affine matrices can be multiplied with 'plain' matrices of matching dimensions.")
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
