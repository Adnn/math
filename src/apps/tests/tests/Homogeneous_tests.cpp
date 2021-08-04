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


SCENARIO("Affine matrices API")
{
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


