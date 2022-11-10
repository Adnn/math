#include "catch.hpp"

#include <math/Spherical.h>
#include <math/Transformations.h>


using namespace ad;
using namespace ad::math;


SCENARIO("Spherical construction and access.")
{
    GIVEN("A radius and two angles.")
    {
        float r = 10.f;
        Radian polar{pi<float>/4};
        Degree azimuthal{360.f};
        WHEN("A Spherical position is instantiated.")
        {
            Spherical s{r, polar, azimuthal};

            THEN("The values can be read back")
            {
                CHECK(s.radius() == r);
                CHECK(s.polar() == polar);
                CHECK(s.azimuthal() == azimuthal);
            }

            THEN("The values can be modified")
            {
                s.radius() *= 2;
                CHECK(s.radius() == 2 * r);
                s.polar() += Degree{10.f};
                CHECK(s.polar() == polar + Degree{10.f});
                s.azimuthal() = Radian{0.f};
                CHECK(s.azimuthal() == Radian{0.f});
            }
        }
    }
}


SCENARIO("Spherical to cartesian.")
{
    GIVEN("A spherical (X)")
    {
        Spherical spherical{1., Degree{90.}, Degree{90.}};

        WHEN("It is converted to cartesian.")
        {
            Position<3, double> cartesian = spherical.toCartesian();

            THEN("They represent the same point")
            {
                CHECK(cartesian.x() == 1.);
                CHECK_THAT(cartesian.y(), Catch::Matchers::WithinAbs(0., 1E-16));
                CHECK_THAT(cartesian.z(), Catch::Matchers::WithinAbs(0., 1E-16));
            }
        }
    }

    GIVEN("A spherical (5 * Y)")
    {
        Spherical spherical{5., Degree{0.}, Degree{90.}};

        WHEN("It is converted to cartesian.")
        {
            Position<3, double> cartesian = spherical.toCartesian();

            THEN("They represent the same point")
            {
                CHECK_THAT(cartesian.x(), Catch::Matchers::WithinAbs(0., 1E-16));
                CHECK(cartesian.y() == 5.);
                CHECK_THAT(cartesian.z(), Catch::Matchers::WithinAbs(0., 1E-16));
            }
        }
    }
}


// TODO replace with matcher from catch v3
// see: https://github.com/catchorg/Catch2/blob/devel/docs/matchers.md#generic-range-matchers
#define APPROX_VEC(a, b, e)                                     \
    CHECK_THAT(a.x(), Catch::Matchers::WithinAbs(b.x(), e));    \
    CHECK_THAT(a.y(), Catch::Matchers::WithinAbs(b.y(), e));    \
    CHECK_THAT(a.z(), Catch::Matchers::WithinAbs(b.z(), e));

SCENARIO("Spherical tangent base.")
{
    GIVEN("A spherical position")
    {
        Spherical spherical{6.5, Radian{pi<float> * 3./4.}, Degree{180.}};

        WHEN("The corresponding tangent frame is computed.")
        {
            Frame<3, double> tangent = spherical.computeTangentFrame();

            THEN("The frame origin is the point represented by spherical.")
            {
                CHECK(tangent.origin == spherical.toCartesian());
            }

            THEN("The orthonormal basis of the frame is tangent to the sphere surface")
            {
                OrthonormalBase<3, double> & base = tangent.base;
                // TODO This commented-out line should be able to replace our custom macro with Catch v3.x
                //CHECK(base.u() == Catch::Matchers::AllMatch(Vec<3, double>{-1., 0., 0.})); 
                APPROX_VEC(base.u(), (Vec<3, double>{-1., 0., 0.}), 1E-10);
                APPROX_VEC(base.v(), (Vec<3, double>{0., 1., 0.} * trans3d::rotateX(Degree{-45.})), 1E-5);
                APPROX_VEC(base.w(), (Vec<3, double>{0., 0., 1.} * trans3d::rotateX(Degree{135.})), 1E-5);

            }
        }
    }
}

#undef APPROX_VEC
