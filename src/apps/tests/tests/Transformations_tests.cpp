#include "catch.hpp"

#include <math/Base.h>
#include <math/Constants.h>
#include <math/Homogeneous.h>
#include <math/Transformations.h>
#include <math/Vector.h>


using namespace ad;
using namespace ad::math;


template <int N_dimensions>
bool approxEqual(Vec<N_dimensions, double> a, Vec<N_dimensions, double> b)
{
    return std::equal(a.begin(), a.end(), b.begin(), b.end(), [](auto left, auto right)
            {
                // zero causes a lot of complications, so we hardcode a small enough margin
                // see: https://github.com/catchorg/Catch2/issues/1444
                return Approx(left).margin(1E-14) == right;
            });
}

template <int N_rows, int N_cols>
bool approxEqual(Matrix<N_rows, N_cols, double> a, Matrix<N_rows, N_cols, double> b)
{
    return std::equal(a.begin(), a.end(), b.begin(), b.end(), [](auto left, auto right)
            {
                // zero causes a lot of complications, so we hardcode a small enough margin
                // see: https://github.com/catchorg/Catch2/issues/1444
                return Approx(left).margin(1E-15) == right;
            });
}


#define APPROX_EQUAL(a, b)          \
    {                               \
        CAPTURE(a, b);              \
        REQUIRE(approxEqual(a, b)); \
    }


SCENARIO("2D rotations")
{
    GIVEN("A few 2D vectors")
    {
        Vec<2> z{0., 0.};
        Vec<2> i{1., 0.};
        Vec<2> j{0., 1.};

        Vec<2> a{ 0.5, -2.0};
        Vec<2> b{-0.5,  2.0};

        GIVEN("A +90° rotation matrix")
        {
            Matrix<2, 2> rotation = trans2d::rotate(Degree<double>{90});

            THEN("The rotation matrix is equivalent to a +Pi/2 radians rotation matrix")
            {
                REQUIRE(rotation == trans2d::rotate(Radian<double>{pi<double>/2}));
            }

            THEN("The vectors can be rotated")
            {
                REQUIRE(z * rotation ==  z);
                APPROX_EQUAL(i * rotation,  j);
                APPROX_EQUAL(j * rotation, -i);

                APPROX_EQUAL(a * rotation, (Vec<2>{ 2.0,  0.5}));
                APPROX_EQUAL(b * rotation, (Vec<2>{-2.0, -0.5}));
            }
        }
    }
}


SCENARIO("2D scaling")
{
    GIVEN("A few 2D vectors")
    {
        Vec<2> zero{0., 0.};
        Vec<2> i{1., 0.};
        Vec<2> j{0., 1.};

        Vec<2> a{ 0.5, -2.0};
        Vec<2> b{-0.5,  2.0};

        GIVEN("A scaling matrix along the cardinal axes, with factors 3 and 2")
        {
            Matrix<2, 2> scaling = trans2d::scale(3., 2.);

            THEN("The vectors can be scaled")
            {
                REQUIRE(zero * scaling ==  zero);
                APPROX_EQUAL(i * scaling, 3*i);
                APPROX_EQUAL(j * scaling, 2*j);

                APPROX_EQUAL(a * scaling, (Vec<2>{a.x()*3, a.y()*2}));
                APPROX_EQUAL(b * scaling, (Vec<2>{b.x()*3, b.y()*2}));
            }
        }
    }

    GIVEN("Diagonal vectors")
    {
        Vec<2> zero{0., 0.};

        Vec<2> pp{ 0.5,  0.5};
        Vec<2> pn{ 2.0, -2.0};
        Vec<2> nn{-3.0, -3.0};
        Vec<2> np{-1.5,  1.5};

        GIVEN("A scaling matrix along the positive diagonal, with factor 5")
        {
            Matrix<2, 2> scaling = trans2d::scale(5., UnitVec<2>{ {1., 1.} });

            THEN("The vectors can be scaled")
            {
                REQUIRE(zero * scaling ==  zero);

                APPROX_EQUAL(pp * scaling, 5*pp);
                APPROX_EQUAL(pn * scaling, pn);
                APPROX_EQUAL(nn * scaling, 5*nn);
                APPROX_EQUAL(np * scaling, np);
            }
        }
    }

    THEN("A non-uniform non-axis-aligned scaling matrix can be computed")
    {
        // This the example from FoCG 3rd p122.
        Matrix<2, 2> scaling = trans2d::scale(2.618, 0.382, Degree<double>(31.7));

        Matrix<2, 2> reference{
            2., 1.,
            1., 1.,
        };

        REQUIRE(scaling.equalsWithinTolerance(reference, 0.001));
    }
}


SCENARIO("2D orthographic projection and reflection")
{
    GIVEN("A few 2D vectors")
    {
        Vec<2> zero{0., 0.};
        Vec<2> i{1., 0.};
        Vec<2> j{0., 1.};

        Vec<2> a{ 0.5, -2.0};
        Vec<2> b{-0.5,  2.0};

        GIVEN("An orthographic projection matrix onto X")
        {
            constexpr Matrix<2, 2> projection = trans2d::projectOrthographicOntoX();

            THEN("The vectors can be projected")
            {
                REQUIRE(zero * projection ==  zero);
                APPROX_EQUAL(i * projection, i);
                APPROX_EQUAL(j * projection, zero);

                APPROX_EQUAL(a * projection, (Vec<2>{a.x(), 0.}));
                APPROX_EQUAL(b * projection, (Vec<2>{b.x(), 0.}));
            }
        }

        GIVEN("An orthographic projection matrix onto Y")
        {
            constexpr Matrix<2, 2> projection = trans2d::projectOrthographicOntoY();

            THEN("The vectors can be projected")
            {
                REQUIRE(zero * projection ==  zero);
                APPROX_EQUAL(i * projection, zero);
                APPROX_EQUAL(j * projection, j);

                APPROX_EQUAL(a * projection, (Vec<2>{0., a.y()}));
                APPROX_EQUAL(b * projection, (Vec<2>{0., b.y()}));
            }
        }

        GIVEN("An orthographic projection matrix onto an arbitrary axis")
        {
            UnitVec<2> onto{ {3., 1.} };
            UnitVec<2> along{ {onto.y(), -onto.x()} };

            Matrix<2, 2> projection = trans2d::projectOrthographicAlong(along);

            THEN("The vectors can be projected")
            {
                REQUIRE(zero * projection ==  zero);

                APPROX_EQUAL(i * projection, i.dot(onto) * onto);
                APPROX_EQUAL(j * projection, j.dot(onto) * onto);

                APPROX_EQUAL(a * projection, a.dot(onto) * onto);
                APPROX_EQUAL(b * projection, b.dot(onto) * onto);
            }
        }

        GIVEN("A reflection matrix along X")
        {
            constexpr Matrix<2, 2> reflection = trans2d::reflectAlongX();

            THEN("The vectors can be reflected")
            {
                REQUIRE(zero * reflection ==  zero);
                APPROX_EQUAL(i * reflection, -i);
                APPROX_EQUAL(j * reflection, j);

                APPROX_EQUAL(a * reflection, (Vec<2>{-a.x(), a.y()}));
                APPROX_EQUAL(b * reflection, (Vec<2>{-b.x(), b.y()}));
            }
        }

        GIVEN("A reflection matrix along Y")
        {
            constexpr Matrix<2, 2> reflection = trans2d::reflectAlongY();

            THEN("The vectors can be reflected")
            {
                REQUIRE(zero * reflection ==  zero);
                APPROX_EQUAL(i * reflection, i);
                APPROX_EQUAL(j * reflection, -j);

                APPROX_EQUAL(a * reflection, (Vec<2>{a.x(), -a.y()}));
                APPROX_EQUAL(b * reflection, (Vec<2>{b.x(), -b.y()}));
            }
        }

        GIVEN("A reflection matrix along an arbitrary axis")
        {
            UnitVec<2> along{ {1., -1.} };

            Matrix<2, 2> reflection = trans2d::reflectAlong(along);

            THEN("The reflection matrix is equivalent ot a reflection matrix along the axis in opposite direction")
            {
                APPROX_EQUAL(reflection, trans2d::reflectAlong(-along));
            }

            THEN("The vectors can be projected")
            {
                REQUIRE(zero * reflection ==  zero);

                APPROX_EQUAL(i * reflection, j);
                APPROX_EQUAL(j * reflection, i);

                APPROX_EQUAL(a * reflection, (Vec<2>{a.y(), a.x()}));
                APPROX_EQUAL(b * reflection, (Vec<2>{b.y(), b.x()}));
            }
        }
    }
}


SCENARIO("2D shearing")
{
    GIVEN("A few 2D vectors")
    {
        Vec<2> zero{0., 0.};
        Vec<2> i{1., 0.};
        Vec<2> j{0., 1.};

        Vec<2> a{ 0.5, -2.0};
        Vec<2> b{-0.5,  2.0};

        GIVEN("A matrix shearing the X coordinate")
        {
            Matrix<2, 2> shearing = trans2d::shearX(2.);

            THEN("The vectors can be skewed")
            {
                REQUIRE(zero * shearing ==  zero);
                APPROX_EQUAL(i * shearing,  i);
                APPROX_EQUAL(j * shearing,  j + 2*i);

                APPROX_EQUAL(a * shearing, (Vec<2>{-3.5, -2.0}));
                APPROX_EQUAL(b * shearing, (Vec<2>{ 3.5,  2.0}));
            }
        }

        GIVEN("A matrix shearing the vertical axis")
        {
            Matrix<2, 2> shearing = trans2d::shearVertical(Degree<double>(45));

            THEN("The vectors can be skewed")
            {
                REQUIRE(zero * shearing ==  zero);
                APPROX_EQUAL(i * shearing,  i);
                APPROX_EQUAL(j * shearing,  j - i);

                APPROX_EQUAL(a * shearing, (Vec<2>{ 2.5, -2.0}));
                APPROX_EQUAL(b * shearing, (Vec<2>{-2.5,  2.0}));
            }
        }

        GIVEN("A matrix shearing the Y coordinate")
        {
            Matrix<2, 2> shearing = trans2d::shearY(-0.2);

            THEN("The vectors can be skewed")
            {
                REQUIRE(zero * shearing ==  zero);
                APPROX_EQUAL(i * shearing,  i - 0.2*j);
                APPROX_EQUAL(j * shearing,  j);

                APPROX_EQUAL(a * shearing, (Vec<2>{ 0.5, -2.1}));
                APPROX_EQUAL(b * shearing, (Vec<2>{-0.5,  2.1}));
            }
        }

        GIVEN("A matrix shearing the horizontal axis")
        {
            Matrix<2, 2> shearing = trans2d::shearHorizontal(Radian<double>(pi<double>/4.));

            THEN("The vectors can be skewed")
            {
                REQUIRE(zero * shearing ==  zero);
                APPROX_EQUAL(i * shearing,  i + j);
                APPROX_EQUAL(j * shearing,  j);

                APPROX_EQUAL(a * shearing, (Vec<2>{ 0.5, -1.5}));
                APPROX_EQUAL(b * shearing, (Vec<2>{-0.5,  1.5}));
            }
        }
    }
}


SCENARIO("2D translations")
{
    GIVEN("A few homogeneous 2D position")
    {
        using namespace homogeneous;
        Position<3> z = makePosition<3>(0., 0.);
        Position<3> a = makePosition<3>(0.5, -2.0);

        GIVEN("A translation")
        {
            Vec<2> displacement{5., -5.};
            AffineMatrix<3> translation = trans2d::translate(displacement);

            THEN("The homogeneous positions can be translated.")
            {
                Vec<3> homoDisplacement = makeVec<3>(5., -5.);
                REQUIRE(z * translation == makePosition<3>(5., -5.));
                REQUIRE(a * translation == a + homoDisplacement);
            }
        }
    }

    GIVEN("An homogeneous 2D displacement")
    {
        using namespace homogeneous;
        Vec<3> a = makeVec<3>(0.5, -2.0);

        GIVEN("A translation")
        {
            Vec<2> displacement{5., -5.};
            AffineMatrix<3> translation = trans2d::translate(displacement);

            THEN("The homogeneous displacement is not affected by translation.")
            {
                REQUIRE(a * translation == a);
            }
        }
    }
}


SCENARIO("2D windowing transformation.")
{
    GIVEN("A (source) rectangle, and its 4 corners as homogeneous positions.")
    {
        Rectangle<double> source{{-5., 2.}, {10., 8.}};

        using namespace homogeneous;
        Position<3> bottomLeft  = makePosition<3>(source.bottomLeft());
        Position<3> bottomRight = makePosition<3>(source.bottomRight());
        Position<3> topLeft     = makePosition<3>(source.topLeft());
        Position<3> topRight    = makePosition<3>(source.topRight());

        GIVEN("A windowing transformation to a destination rectangle.")
        {
            Rectangle<double> destination{{50., 50.}, {1., 1.}};
            AffineMatrix<3> windowing = trans2d::window(source, destination);

            THEN("The 4 source corners are moved to the 4 destination corners by the transformation.")
            {
                REQUIRE(bottomLeft * windowing == makePosition<3>(destination.bottomLeft()));
                REQUIRE(bottomRight * windowing == makePosition<3>(destination.bottomRight()));
                REQUIRE(topLeft * windowing == makePosition<3>(destination.topLeft()));
                REQUIRE(topRight * windowing == makePosition<3>(destination.topRight()));
            }
        }
    }
}


SCENARIO("2D coordinate frame transformation.")
{
    using namespace homogeneous;

    GIVEN("A 2D coordinate frame")
    {
        Frame<2> localFrame{
            {10., -10.},
            OrthonormalBase<2>::MakeFromSingleVector({0., 1.})
        };

        GIVEN("A few vectors in both spaces")
        {
            Position<3> canon_canonicalOrigin = makePosition(Position<2>{0., 0.});
            Position<3> canon_localOrigin = makePosition(Position<2>{10., -10.});
            Position<3> canon_a = makePosition(Position<2>{5., 5.});
            Position<3> canon_b = makePosition(Position<2>{-0.5, 5.});

            Position<3> local_canonicalOrigin = makePosition(Position<2>{10., 10.});
            Position<3> local_localOrigin = makePosition(Position<2>{0., 0.});
            Position<3> local_a = makePosition(Position<2>{15., 5.});
            Position<3> local_b = makePosition(Position<2>{15., 10.5});

            THEN("Canonical coordinates can be expressed in the local frame")
            {
                AffineMatrix<3> canonicalToFrame = trans2d::canonicalToFrame(localFrame);

                REQUIRE(canon_canonicalOrigin * canonicalToFrame == local_canonicalOrigin);
                REQUIRE(canon_localOrigin * canonicalToFrame == local_localOrigin);
                REQUIRE(canon_a * canonicalToFrame == local_a);
                REQUIRE(canon_b * canonicalToFrame == local_b);
            }

            THEN("Local coordinates can be expressed in the canonical frame")
            {
                AffineMatrix<3> frameToCanonical = trans2d::frameToCanonical(localFrame);

                REQUIRE(local_canonicalOrigin * frameToCanonical == canon_canonicalOrigin);
                REQUIRE(local_localOrigin * frameToCanonical == canon_localOrigin);
                REQUIRE(local_a * frameToCanonical == canon_a);
                REQUIRE(local_b * frameToCanonical == canon_b);
            }
        }
    }
}


SCENARIO("3D rotations")
{
    GIVEN("A few 3D vectors")
    {
        Vec<3> zero {0., 0., 0.};
        Vec<3> i{1., 0., 0.};
        Vec<3> j{0., 1., 0.};
        Vec<3> k{0., 0., 1.};

        GIVEN("A -90° rotation matrix about X")
        {
            Matrix<3, 3> rotation = trans3d::rotateX(-Degree<double>{90});

            THEN("The rotation matrix is equivalent to a -Pi/2 radians rotation matrix")
            {
                REQUIRE(rotation == trans3d::rotateX(-Radian<double>{pi<double>/2}));
            }

            THEN("The vectors can be rotated")
            {
                REQUIRE(zero * rotation ==  zero);
                REQUIRE(i * rotation ==  i);
                APPROX_EQUAL(j * rotation, -k);
                APPROX_EQUAL(k * rotation,  j);
            }
        }

        GIVEN("A +45° rotation matrix about Y")
        {
            Matrix<3, 3> rotation = trans3d::rotateY(Degree<double>{45});

            THEN("The rotation matrix is equivalent to a +Pi/4 radians rotation matrix")
            {
                REQUIRE(rotation == trans3d::rotateY(Radian<double>{pi<double>/4}));
            }

            THEN("The vectors can be rotated")
            {
                static const double v = std::cos(pi<double>/4.);

                REQUIRE(zero * rotation ==  zero);
                APPROX_EQUAL(i * rotation, (Vec<3>{v, 0., -v}));
                REQUIRE(j * rotation ==  j);
                APPROX_EQUAL(k * rotation, (Vec<3>{v, 0.,  v}));
            }
        }

        GIVEN("A +90° rotation matrix about Z")
        {
            Matrix<3, 3> rotation = trans3d::rotateZ(Degree<double>{90});

            THEN("The rotation matrix is equivalent to a +Pi/2 radians rotation matrix")
            {
                REQUIRE(rotation == trans3d::rotateZ(Radian<double>{pi<double>/2}));
            }

            THEN("The vectors can be rotated")
            {
                REQUIRE(zero * rotation ==  zero);
                APPROX_EQUAL(i * rotation,  j);
                APPROX_EQUAL(j * rotation, -i);
                REQUIRE(k * rotation ==  k);
            }
        }

        GIVEN("A third of a revolution rotation matrix about diagonal vector")
        {
            Matrix<3, 3> rotation = trans3d::rotate(2*pi<Radian<double>>/3, UnitVec<3>{{-1., 1., -1.}});

            THEN("The rotation matrix is equivalent to a negative 2/3 of a revolution rotation matrix about the same diagonal")
            {
                APPROX_EQUAL(rotation,
                             trans3d::rotate(-2*2*pi<Radian<double>>/3,
                                             UnitVec<3>{{-1., 1., -1.}}));
            }

            THEN("The rotation matrix is equivalent to a positive 2/3 of a revolution rotation matrix about the diagonal in opposite direction")
            {
                APPROX_EQUAL(rotation,
                             trans3d::rotate(2*2*pi<Radian<double>>/3,
                                             UnitVec<3>{{1., -1., 1.}}));
            }

            THEN("The vectors can be rotated")
            {
                REQUIRE(zero * rotation ==  zero);
                APPROX_EQUAL(i * rotation, -j);
                APPROX_EQUAL(j * rotation, -k);
                APPROX_EQUAL(k * rotation,  i);
            }
        }
    }
}


SCENARIO("3D scaling")
{
    GIVEN("A few 3D vectors")
    {
        Vec<3> zero {0., 0., 0.};
        Vec<3> i{1., 0., 0.};
        Vec<3> j{0., 1., 0.};
        Vec<3> k{0., 0., 1.};

        Vec<3> a{0.5, -10., 28.};
        Vec<3> b{ 0.,  28.,  8.};

        GIVEN("A scaling matrix along the cardinal axes, with factors 3, 2, -0.5")
        {
            Matrix<3, 3> scaling = trans3d::scale(3., 2., -0.5);

            THEN("The vectors can be scaled")
            {
                REQUIRE(zero * scaling ==  zero);
                APPROX_EQUAL(i * scaling,    3*i);
                APPROX_EQUAL(j * scaling,    2*j);
                APPROX_EQUAL(k * scaling, -0.5*k);

                APPROX_EQUAL(a * scaling, (Vec<3>{a.x()*3, a.y()*2, a.z()*-0.5}));
                APPROX_EQUAL(b * scaling, (Vec<3>{b.x()*3, b.y()*2, b.z()*-0.5}));
            }
        }
    }

    GIVEN("Diagonal vectors")
    {
        Vec<3> zero{0., 0., 0.};

        Vec<3> ppp{ 0.5,  0.5,  0.5};
        //Vec<3> ppn{ 2.0,  2.0, -2.0};
        //Vec<3> pnp{ 3.0, -3.0,  3.0};
        //Vec<3> pnn{ 1.5, -1.5, -1.5};
        //Vec<3> npp{-0.5,  0.5,  0.5};
        //Vec<3> npn{-2.0,  2.0, -2.0};
        //Vec<3> nnp{-3.0, -3.0,  3.0};
        Vec<3> nnn{-1.5, -1.5, -1.5};

        Vec<3> a{0.5, -10., 28.};
        Vec<3> b{ 0.,  28.,  8.};


        GIVEN("A scaling matrix along the negative diagonal, with factor 5")
        {
            UnitVec<3> along{nnn};
            Matrix<3, 3> scaling = trans3d::scale(5., along);

            THEN("The vectors can be scaled")
            {
                REQUIRE(zero * scaling ==  zero);

                APPROX_EQUAL(ppp * scaling, 5*ppp);
                APPROX_EQUAL(nnn * scaling, 5*nnn);

                Vec<3> a_onto_nnn = a.dot(along) * along;
                APPROX_EQUAL(a * scaling, (a_onto_nnn*5) + (a-a_onto_nnn));

                Vec<3> b_onto_nnn = b.dot(along) * along;
                APPROX_EQUAL(b * scaling, (b_onto_nnn*5) + (b-b_onto_nnn));
            }
        }
    }
}


SCENARIO("3D orthographic projection and reflection")
{
    GIVEN("A few 3D vectors")
    {
        Vec<3> zero {0., 0., 0.};
        Vec<3> i{1., 0., 0.};
        Vec<3> j{0., 1., 0.};
        Vec<3> k{0., 0., 1.};

        Vec<3> a{0.5, -10., 28.};
        Vec<3> b{ 0.,  28.,  8.};

        GIVEN("An orthographic projection matrix onto XY")
        {
            constexpr Matrix<3, 3> projection = trans3d::projectOrthographicOntoXY();

            THEN("The vectors can be projected")
            {
                REQUIRE(zero * projection ==  zero);
                APPROX_EQUAL(i * projection, i);
                APPROX_EQUAL(j * projection, j);
                APPROX_EQUAL(k * projection, zero);

                APPROX_EQUAL(a * projection, (Vec<3>{a.x(), a.y(), 0.}));
                APPROX_EQUAL(b * projection, (Vec<3>{b.x(), b.y(), 0.}));
            }
        }

        GIVEN("An orthographic projection matrix onto XZ")
        {
            constexpr Matrix<3, 3> projection = trans3d::projectOrthographicOntoXZ();

            THEN("The vectors can be projected")
            {
                REQUIRE(zero * projection ==  zero);
                APPROX_EQUAL(i * projection, i);
                APPROX_EQUAL(j * projection, zero);
                APPROX_EQUAL(k * projection, k);

                APPROX_EQUAL(a * projection, (Vec<3>{a.x(), 0., a.z()}));
                APPROX_EQUAL(b * projection, (Vec<3>{b.x(), 0., b.z()}));
            }
        }

        GIVEN("An orthographic projection matrix onto YZ")
        {
            constexpr Matrix<3, 3> projection = trans3d::projectOrthographicOntoYZ();

            THEN("The vectors can be projected")
            {
                REQUIRE(zero * projection ==  zero);
                APPROX_EQUAL(i * projection, zero);
                APPROX_EQUAL(j * projection, j);
                APPROX_EQUAL(k * projection, k);

                APPROX_EQUAL(a * projection, (Vec<3>{0., a.y(), a.z()}));
                APPROX_EQUAL(b * projection, (Vec<3>{0., b.y(), b.z()}));
            }
        }

        GIVEN("An orthographic projection matrix along an arbitrary axis")
        {
            UnitVec<3> along{ {1., -3., -5.} };

            Matrix<3, 3> projection = trans3d::projectOrthographicAlong(along);

            THEN("The vectors can be projected")
            {
                REQUIRE(zero * projection ==  zero);

                APPROX_EQUAL(i * projection, i-(i.dot(along) * along));
                APPROX_EQUAL(j * projection, j-(j.dot(along) * along));

                APPROX_EQUAL(a * projection, a-(a.dot(along) * along));
                APPROX_EQUAL(b * projection, b-(b.dot(along) * along));
            }
        }

        GIVEN("A reflection matrix along X")
        {
            constexpr Matrix<3, 3> reflection = trans3d::reflectAlongX();

            THEN("The vectors can be reflected")
            {
                REQUIRE(zero * reflection ==  zero);
                APPROX_EQUAL(i * reflection, -i);
                APPROX_EQUAL(j * reflection, j);
                APPROX_EQUAL(k * reflection, k);

                APPROX_EQUAL(a * reflection, (Vec<3>{-a.x(), a.y(), a.z()}));
                APPROX_EQUAL(b * reflection, (Vec<3>{-b.x(), b.y(), b.z()}));
            }
        }

        GIVEN("A reflection matrix along Y")
        {
            constexpr Matrix<3, 3> reflection = trans3d::reflectAlongY();

            THEN("The vectors can be reflected")
            {
                REQUIRE(zero * reflection ==  zero);
                APPROX_EQUAL(i * reflection, i);
                APPROX_EQUAL(j * reflection, -j);
                APPROX_EQUAL(k * reflection, k);

                APPROX_EQUAL(a * reflection, (Vec<3>{a.x(), -a.y(), a.z()}));
                APPROX_EQUAL(b * reflection, (Vec<3>{b.x(), -b.y(), b.z()}));
            }
        }

        GIVEN("A reflection matrix along Z")
        {
            constexpr Matrix<3, 3> reflection = trans3d::reflectAlongZ();

            THEN("The vectors can be reflected")
            {
                REQUIRE(zero * reflection ==  zero);
                APPROX_EQUAL(i * reflection, i);
                APPROX_EQUAL(j * reflection, j);
                APPROX_EQUAL(k * reflection, -k);

                APPROX_EQUAL(a * reflection, (Vec<3>{a.x(), a.y(), -a.z()}));
                APPROX_EQUAL(b * reflection, (Vec<3>{b.x(), b.y(), -b.z()}));
            }
        }

        GIVEN("A reflection matrix along an arbitrary axis")
        {
            UnitVec<3> along{ {1., -1., 0.} };

            Matrix<3, 3> reflection = trans3d::reflectAlong(along);

            THEN("The reflection matrix is equivalent ot a reflection matrix along the axis in opposite direction")
            {
                APPROX_EQUAL(reflection, trans3d::reflectAlong(-along));
            }

            THEN("The vectors can be projected")
            {
                REQUIRE(zero * reflection ==  zero);

                APPROX_EQUAL(i * reflection, j);
                APPROX_EQUAL(j * reflection, i);
                APPROX_EQUAL(k * reflection, k);

                APPROX_EQUAL(a * reflection, (Vec<3>{a.y(), a.x(), a.z()}));
                APPROX_EQUAL(b * reflection, (Vec<3>{b.y(), b.x(), b.z()}));
            }
        }
    }
}


SCENARIO("3D shearing")
{
    GIVEN("A few 2D vectors")
    {
        Vec<3> zero {0., 0., 0.};
        Vec<3> i{1., 0., 0.};
        Vec<3> j{0., 1., 0.};
        Vec<3> k{0., 0., 1.};

        Vec<3> a{ 0.5, -2.0, 5.0};
        Vec<3> b{-0.5,  0.0, 5.0};

        GIVEN("A matrix shearing the X and Y coordinates")
        {
            Matrix<3, 3> shearing = trans3d::shearXY(1., -1.);

            THEN("The vectors can be skewed")
            {
                REQUIRE(zero * shearing ==  zero);
                APPROX_EQUAL(i * shearing,  i);
                APPROX_EQUAL(j * shearing,  j);
                APPROX_EQUAL(k * shearing,  k + i - j);

                APPROX_EQUAL(a * shearing, (Vec<3>{ 5.5, -7.0, 5.0}));
                APPROX_EQUAL(b * shearing, (Vec<3>{ 4.5, -5.0, 5.0}));
            }
        }

        GIVEN("A matrix shearing the X and Z coordinates")
        {
            Matrix<3, 3> shearing = trans3d::shearXZ(2., 0.);

            THEN("The vectors can be skewed")
            {
                REQUIRE(zero * shearing ==  zero);
                APPROX_EQUAL(i * shearing,  i);
                APPROX_EQUAL(j * shearing,  j + 2*i);
                APPROX_EQUAL(k * shearing,  k);

                APPROX_EQUAL(a * shearing, (Vec<3>{-3.5, -2.0, 5.0}));
                APPROX_EQUAL(b * shearing, (Vec<3>{-0.5,  0.0, 5.0}));
            }
        }

        GIVEN("A matrix shearing the Y and Z coordinates")
        {
            Matrix<3, 3> shearing = trans3d::shearYZ(0., -0.2);

            THEN("The vectors can be skewed")
            {
                REQUIRE(zero * shearing ==  zero);
                APPROX_EQUAL(i * shearing,  i - 0.2*k);
                APPROX_EQUAL(j * shearing,  j);
                APPROX_EQUAL(k * shearing,  k);

                APPROX_EQUAL(a * shearing, (Vec<3>{ 0.5, -2.0, 4.9}));
                APPROX_EQUAL(b * shearing, (Vec<3>{-0.5,  0.0, 5.1}));
            }
        }
    }
}


SCENARIO("3D translations")
{
    GIVEN("A few homogeneous 3D position")
    {
        using namespace homogeneous;
        Position<4> z = makePosition<4>(0., 0., 0.);
        Position<4> a = makePosition<4>(0.5, -10., 28.);

        GIVEN("A translation")
        {
            Vec<3> displacement{5., -5., 0.5};
            AffineMatrix<4> translation = trans3d::translate(displacement);

            THEN("The homogeneous positions can be translated.")
            {
                Vec<4> homoDisplacement = makeVec<4>(5., -5., 0.5);
                REQUIRE(z * translation == makePosition<4>(5., -5., 0.5));
                REQUIRE(a * translation == a + homoDisplacement);
            }
        }
    }

    GIVEN("An homogeneous 3D displacement")
    {
        using namespace homogeneous;
        Vec<4> a = makeVec<4>(0.5, -10., 28.);

        GIVEN("A translation")
        {
            Vec<3> displacement{5., -5., 0.5};
            AffineMatrix<4> translation = trans3d::translate(displacement);

            THEN("The homogeneous displacement is not affected by translation.")
            {
                REQUIRE(a * translation == a);
            }
        }
    }
}


SCENARIO("3D windowing transformation.")
{
    GIVEN("A (source) box, and its 8 corners as homogeneous positions.")
    {
        Box<double> source{{-5., 2., 0.5}, {10., 8., 0.5}};

        using namespace homogeneous;
        Position<4> bottomLeftFront  = makePosition<4>(source.bottomLeftFront());
        Position<4> bottomLeftBack   = makePosition<4>(source.bottomLeftBack());
        Position<4> bottomRightFront = makePosition<4>(source.bottomRightFront());
        Position<4> bottomRightBack  = makePosition<4>(source.bottomRightBack());
        Position<4> topLeftFront     = makePosition<4>(source.topLeftFront());
        Position<4> topLeftBack      = makePosition<4>(source.topLeftBack());
        Position<4> topRightFront    = makePosition<4>(source.topRightFront());
        Position<4> topRightBack     = makePosition<4>(source.topRightBack());

        GIVEN("A windowing transformation to a destination box.")
        {
            Box<double> destination{{50., 50., 50.}, {1., 1., 1.}};
            AffineMatrix<4> windowing = trans3d::window(source, destination);

            THEN("The 8 source corners are moved to the 8 destination corners by the transformation.")
            {
                REQUIRE(bottomLeftFront * windowing  == makePosition<4>(destination.bottomLeftFront()));
                REQUIRE(bottomLeftBack * windowing   == makePosition<4>(destination.bottomLeftBack()));
                REQUIRE(bottomRightFront * windowing == makePosition<4>(destination.bottomRightFront()));
                REQUIRE(bottomRightBack * windowing  == makePosition<4>(destination.bottomRightBack()));
                REQUIRE(topLeftFront * windowing     == makePosition<4>(destination.topLeftFront()));
                REQUIRE(topLeftBack * windowing      == makePosition<4>(destination.topLeftBack()));
                REQUIRE(topRightFront * windowing    == makePosition<4>(destination.topRightFront()));
                REQUIRE(topRightBack * windowing     == makePosition<4>(destination.topRightBack()));
            }
        }
    }
}


SCENARIO("3D coordinate frame transformation.")
{
    using namespace homogeneous;

    GIVEN("A 3D coordinate frame")
    {
        Frame<3> localFrame{
            {10., 10., 1.5},
            OrthonormalBase<3>::MakeFromTwoVectors(/*w*/{0., 0., 1.}, /*up*/{-1., -1., 0.})
        };

        GIVEN("A few vectors in both spaces")
        {
            Position<4> canon_canonicalOrigin = makePosition(Position<3>{0., 0., 0.});
            Position<4> canon_localOrigin = makePosition(Position<3>{10., 10., 1.5});
            Position<4> canon_a = makePosition(Position<3>{0., 10., 5.});

            Position<4> local_canonicalOrigin = makePosition(Position<3>{0., 10*std::sqrt(2), -1.5});
            Position<4> local_localOrigin = makePosition(Position<3>{0., 0., 0.});
            Position<4> local_a = makePosition(Position<3>{10*std::sin(pi<double>/4), 10.*std::sin(pi<double>/4), 3.5});

            THEN("Canonical coordinates can be expressed in the local frame")
            {
                AffineMatrix<4> canonicalToFrame = trans3d::canonicalToFrame(localFrame);

                REQUIRE((canon_canonicalOrigin * canonicalToFrame).equalsWithinTolerance(local_canonicalOrigin, 0.00001));
                REQUIRE(canon_localOrigin * canonicalToFrame == local_localOrigin);
                REQUIRE((canon_a * canonicalToFrame).equalsWithinTolerance(local_a, 0.00001));
            }

            THEN("Local coordinates can be expressed in the canonical frame")
            {
                AffineMatrix<4> frameToCanonical = trans3d::frameToCanonical(localFrame);

                REQUIRE(local_canonicalOrigin * frameToCanonical == canon_canonicalOrigin);
                REQUIRE(local_localOrigin * frameToCanonical == canon_localOrigin);
                REQUIRE(local_a * frameToCanonical == canon_a);
            }
        }
    }
}


SCENARIO("3D perspective transformation.")
{
    using namespace homogeneous;

    GIVEN("A few homogeneous positions.")
    {
        Position<4> nearPlane_a = makePosition(Position<3>{0.,  0.,  -2.});
        Position<4> nearPlane_b = makePosition(Position<3>{10., -5., -2.});

        Position<4> farPlane_a = makePosition(Position<3>{0.,  0.,  -10.});
        Position<4> farPlane_b = makePosition(Position<3>{5., -25., -10.});
        
        Position<4> mid_a = makePosition(Position<3>{0.,   0.,  -4.});
        Position<4> mid_b = makePosition(Position<3>{-5.5, 25., -4.});

        GIVEN("A pespective transformation.")
        {
            Matrix<4, 4> perspective = trans3d::perspective(-2., -10.);

            auto nearPlane_a_transformed = normalize(nearPlane_a * perspective);
            auto nearPlane_b_transformed = normalize(nearPlane_b * perspective);
            auto mid_a_transformed = normalize(mid_a * perspective);
            auto mid_b_transformed = normalize(mid_b * perspective);
            auto farPlane_a_transformed = normalize(farPlane_a * perspective);
            auto farPlane_b_transformed = normalize(farPlane_b * perspective);

            THEN("The near plane positions are not changed by the transformation.")
            {
                REQUIRE(nearPlane_a_transformed == nearPlane_a);
                REQUIRE(nearPlane_b_transformed  == nearPlane_b);
            }

            THEN("The other positions are applying perspective as expected (divided by Z/Near).")
            {
                //Mid
                REQUIRE(mid_a_transformed.x() == mid_a.x()/2.);
                REQUIRE(mid_a_transformed.y() == mid_a.y()/2.);

                REQUIRE(mid_b_transformed.x() == mid_b.x()/2.);
                REQUIRE(mid_b_transformed.y() == mid_b.y()/2.);

                THEN("The mid points Z value is NOT identical after transformation")
                {
                    REQUIRE_FALSE(mid_a_transformed.z() == mid_a.z());
                    REQUIRE_FALSE(mid_b_transformed.z() == mid_b.z());
                }

                //Far plane
                REQUIRE(farPlane_a_transformed.x() == farPlane_a.x()/(farPlane_a.z() / -2.));
                REQUIRE(farPlane_a_transformed.y() == farPlane_a.y()/(farPlane_a.z() / -2.));

                REQUIRE(farPlane_b_transformed.x() == farPlane_b.x()/(farPlane_b.z() / -2.));
                REQUIRE(farPlane_b_transformed.y() == farPlane_b.y()/(farPlane_b.z() / -2.));

                THEN("The far plane points Z value is identical after transformation")
                {
                    REQUIRE(farPlane_a_transformed.z() == farPlane_a.z());
                    REQUIRE(farPlane_b_transformed.z() == farPlane_b.z());
                }

                THEN("The relative Z ordering is maintained by the transformation")
                {
                    REQUIRE(-2. > mid_a_transformed.z());
                    REQUIRE(mid_a_transformed.z() > farPlane_a_transformed.z());

                    REQUIRE(-2. > mid_b_transformed.z());
                    REQUIRE(mid_b_transformed.z() > farPlane_b_transformed.z());
                }
            }

            GIVEN("An inverse pespective transformation.")
            {
                Matrix<4, 4> inverse = trans3d::perspectiveInverse(-2., -10.);

                THEN("It allows to find the original positions back.")
                {
                    REQUIRE(normalize(nearPlane_a_transformed * inverse) == nearPlane_a);
                    REQUIRE(normalize(nearPlane_b_transformed * inverse) == nearPlane_b);
                    REQUIRE(normalize(mid_a_transformed * inverse) == mid_a);
                    REQUIRE(normalize(mid_b_transformed * inverse) == mid_b);
                    REQUIRE(normalize(farPlane_a_transformed * inverse) == farPlane_a);
                    REQUIRE(normalize(farPlane_b_transformed * inverse) == farPlane_b);
                }
            }
        }
    }
}
