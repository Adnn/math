#include "catch.hpp"
#include "detection.h"
#include "operation_detectors.h"

#include <math/Vector.h>

#include <algorithm>


using namespace ad;
using namespace ad::math;


SCENARIO("Vector types properties")
{
    // Trivially copyable types have great properties for low level manipulation
    // (e.g. file serialization)
    // see: https://en.cppreference.com/w/cpp/types/is_trivially_copyable#Notes
    REQUIRE(std::is_trivially_copyable<Vec<3>>::value);
    REQUIRE(std::is_trivially_copyable<Size<2>>::value);
    REQUIRE(std::is_trivially_copyable<Position<8>>::value);
}


SCENARIO("Vectors can be constructed and manipulated")
{
    WHEN("Using the Zero factory")
    {
        Vec<2> vec2 = Vec<2>::Zero();

        THEN("All elements are 0")
        {
            REQUIRE(std::all_of(vec2.begin(), vec2.end(), [](double aValue){ return aValue == 0; }));
        }

        THEN("Elements are assignable via accessors")
        {
            vec2.x() = 1;
            vec2.y() = 2;
            REQUIRE(vec2[0] == 1);
            REQUIRE(vec2[1] == 2);
        }

    }

    WHEN("Using explicit constructor")
    {
        Vec<3> vec3{1.0, 2.0, 3.0};

        THEN("Stored values can be accessed as a pointer to contiguous data")
        {
            const double * data = vec3.data();
            REQUIRE(data[0] == 1.0);
            REQUIRE(data[1] == 2.0);
            REQUIRE(data[2] == 3.0);
        }

        THEN("Vector can be reset to zero")
        {
            vec3.setZero();
            REQUIRE(std::all_of(vec3.begin(), vec3.end(), [](double aValue){ return aValue == 0; }));
        }
    }

    GIVEN("A vector")
    {
        Vec<3, int> vec3{4, 4, 2};
        THEN("It can be compared and assigned")
        {
            Vec<3, int> other{5, 4, 2};
            REQUIRE(vec3 != other);
            vec3 = other;
            REQUIRE(vec3 == other);
        }
    }
}


SCENARIO("There is a separate UnitVec type to model unit vectors")
{
    REQUIRE_FALSE(std::is_aggregate_v<UnitVec<4>>);
    // Note: disables aggregate initialization, such as:
    //UnitVec<4> unit4{vec4};

    REQUIRE_FALSE(std::is_default_constructible_v<UnitVec<4>>);

    REQUIRE_FALSE(std::is_trivially_constructible_v<UnitVec<4>, Vec<4>>);
    // Note: Constructing a UnitVec from a Vec is not using a trivial (~defaulted) operation

    GIVEN("A vector of dimension 4")
    {
        Vec<4> vec4{10., 20., 40., 80.};

        WHEN("A unit vector of matching dimension is explicitly constructed from the vector")
        {
            UnitVec<4> unit4{vec4};

            THEN("The unit vector norm is 1")
            {
                REQUIRE(unit4.getNorm() == 1.);
            }
        }
    }
}


SCENARIO("There are separate vector template classes")
{
    WHEN("Using different vector types of matching dimension and value type")
    {
        THEN("They are not implicitly convertible")
        {
            REQUIRE_FALSE(std::is_convertible<Size<2>, Vec<2>>::value);
        }

        THEN("They can be explicitly converted")
        {
            Size<2> size{5., 10.};
            Vec<2> vec = static_cast<Vec<2>>(size);
            REQUIRE(std::equal(vec.begin(), vec.end(), size.begin()));

            Vec<2> vec_as = size.as<Vec>();
            REQUIRE(std::equal(vec_as.begin(), vec_as.end(), size.begin()));
        }
    }

    WHEN("Using same vector class, but with different value_type")
    {
        THEN("They are not implicitly convertible")
        {
            REQUIRE_FALSE(std::is_convertible<Vec<2>, Vec<2, int>>::value);
        }

        THEN("They can be explicitly converted")
        {
            Vec<2> source{5., 10.};
            {
                Vec<2, int> converted = static_cast<Vec<2, int>>(source);
                REQUIRE(converted.at(0) == 5);
                REQUIRE(converted.at(1) == 10);
            }

            {
                Vec<2, int> converted = source.as<Vec, int>();
                REQUIRE(converted.at(0) == 5);
                REQUIRE(converted.at(1) == 10);
            }
        }
    }

    WHEN("Using different vector types, with different value_type")
    {
        THEN("They are not implicitly convertible")
        {
            REQUIRE_FALSE(std::is_convertible<Vec<2, float>, Position<2, int>>::value);
        }

        THEN("They can be explicitly converted")
        {
            Vec<2, float> source{5.f, 10.f};
            {
                Position<2, int> converted = static_cast<Position<2, int>>(source);
                REQUIRE(converted.at(0) == 5);
                REQUIRE(converted.at(1) == 10);
            }

            {
                Position<2, int> converted = source.as<Position, int>();
                REQUIRE(converted.at(0) == 5);
                REQUIRE(converted.at(1) == 10);
            }
        }
    }

    WHEN("Vector types are not of matching dimension")
    {
        THEN("They cannot be converted at all")
        {
            REQUIRE_FALSE(std::is_convertible<Vec<2, float>, Vec<3, float>>::value);
            REQUIRE_FALSE(std::is_convertible<Position<2, float>, Vec<3, float>>::value);
            REQUIRE_FALSE(std::is_convertible<Position<2, float>, Vec<3, int>>::value);
        }
    }
}


#define HAS(mem_fun) template <class T> using has_##mem_fun##_t = decltype(std::declval<T&>().mem_fun());

HAS(x)
HAS(y)
HAS(z)
HAS(w)


SCENARIO("Vec class operations")
{
    THEN("A zero factory is available")
    {
        Vec<3> zero = Vec<3>::Zero();
        REQUIRE(zero.x() == 0.);
        REQUIRE(zero.y() == 0.);
        REQUIRE(zero.z() == 0.);
    }

    THEN("Two vectors of matching dimension and type can be added together")
    {
        REQUIRE(is_detected_v<is_additive_t, Vec<5>, Vec<5>>);
        REQUIRE(is_detected_v<is_additivecompound_t, Vec<2>, Vec<2>>);
    }

    THEN("Two vectors of different dimension cannot be added together")
    {
        REQUIRE_FALSE(is_detected_v<is_additive_t, Vec<4>, Vec<5>>);
        REQUIRE_FALSE(is_detected_v<is_additivecompound_t, Vec<3>, Vec<2>>);
    }

    THEN("Two vectors of different value type cannot be added together")
    {
        REQUIRE_FALSE(is_detected_v<is_additive_t, Vec<4>, Vec<4, int>>);
        REQUIRE_FALSE(is_detected_v<is_additivecompound_t, Vec<3, int>, Vec<3>>);
    }

    GIVEN("Two 3 elements vectors")
    {
        Vec<3> a = {3., 4., 5.};
        Vec<3> b = {10., 20., 30.};

        THEN("They can be added and substracted")
        {
            Vec<3> add = a+b;
            REQUIRE(add.x() == a.x() + b.x());
            REQUIRE(add.y() == a.y() + b.y());
            REQUIRE(add.z() == a.z() + b.z());

            Vec<3> sub = b-a;
            REQUIRE(sub.x() == b.x() - a.x());
            REQUIRE(sub.y() == b.y() - a.y());
            REQUIRE(sub.z() == b.z() - a.z());
        }
    }

    GIVEN("Two vector of dimension 3")
    {
        Vec<3> a{ 3., 2., -4.};
        Vec<3> b{ 5., -1., 2.};

        THEN("dot product can be computed")
        {
            REQUIRE(a.dot(b) == 5);
            REQUIRE(a.dot(b) == b.dot(a));
        }

        THEN("Hadamard product can be computed")
        {
            Vec<3> expected{15., -2., -8.};
            REQUIRE(a.cwMul(b) == expected);

            b.cwMulAssign(a);
            REQUIRE(b == expected);
        }

        THEN("Cross product can be computed")
        {
            Vec<3> expected{0., -26., -13.};
            REQUIRE(a.cross(b) == expected);

            a.crossAssign(b);
            REQUIRE(a == expected);
        }
    }

    GIVEN("A vector 3")
    {
        Vec<3> vector{ 5., 6., 15.};

        THEN("The vector can be multiplied by a scalar")
        {
            Vec<3> expected = {3.*5., 3.*6, 3.*15.};
            REQUIRE(vector*3. == expected);
        }

        THEN("The norm of the vector can be computed")
        {
            double expected = std::sqrt(5.*5. + 6.*6. + 15.*15.);
            REQUIRE(vector.getNorm() == expected);
        }

        THEN("The vector can be normalized")
        {
            REQUIRE(vector.normalize().getNorm() == Approx(1.));
        }

        GIVEN("A 3x3 matrix")
        {
            Matrix<3, 3> matrix {
                4., 12., 33.,
                1., 0.5, 1.,
                84., 0.5, 4.,
            };

            Vec<3> expected { 1286., 70.5, 231. };

            THEN("The vector can me multiplied by the matrix")
            {
                REQUIRE((vector*matrix) == expected);
            }

            THEN("The vector can me multiplied by the matrix")
            {
                vector *= matrix;
                REQUIRE(vector == expected);
            }
        }
    }

    THEN("Accessors are available depending on dimension")
    {
        REQUIRE(is_detected_v<has_x_t, Vec<2>>);
        REQUIRE(is_detected_v<has_y_t, Vec<2>>);
        REQUIRE_FALSE(is_detected_v<has_z_t, Vec<2>>);
        REQUIRE_FALSE(is_detected_v<has_w_t, Vec<2>>);

        REQUIRE(is_detected_v<has_x_t, Vec<3>>);
        REQUIRE(is_detected_v<has_y_t, Vec<3>>);
        REQUIRE(is_detected_v<has_z_t, Vec<3>>);
        REQUIRE_FALSE(is_detected_v<has_w_t, Vec<3>>);

        REQUIRE(is_detected_v<has_x_t, Vec<4>>);
        REQUIRE(is_detected_v<has_y_t, Vec<4>>);
        REQUIRE(is_detected_v<has_z_t, Vec<4>>);
        REQUIRE(is_detected_v<has_w_t, Vec<4>>);
    }
}


SCENARIO("Position class operations")
{
    THEN("Positions cannot be added together")
    {
        REQUIRE_FALSE(is_detected_v<is_additive_t, Position<3>, Position<3>>);
        REQUIRE_FALSE(is_detected_v<is_additivecompound_t, Position<3>, Position<3>>);
    }

    THEN("Positions cannot be compound substracted together")
    {
        REQUIRE_FALSE(is_detected_v<is_substractivecompound_t, Position<3>, Position<3>>);
    }

    THEN("Positions can be substracted together")
    {
        REQUIRE(is_detected_v<is_substractive_t, Position<3>, Position<3>>);
    }

    THEN("A displacement can be added to a position of same dimension and value type")
    {
        REQUIRE(is_detected_v<is_additive_t, Position<3>, Vec<3>>);
        REQUIRE(is_detected_v<is_additivecompound_t, Position<3>, Vec<3>>);
    }

    THEN("A position cannot be added to a displacement even with matching dimension and value type")
    {
        REQUIRE_FALSE(is_detected_v<is_additive_t, Vec<3>, Position<3>>);
        REQUIRE_FALSE(is_detected_v<is_additivecompound_t, Vec<3>, Position<3>>);
    }

    GIVEN("A position instance of dimension 3")
    {
        Position<3, int> pos3{5, 10, 15};

        GIVEN("A 3x3 Matrix")
        {
            Matrix<3, 3, int> matrix{
                2, 0, 2,
                1, 1, 1,
                0, 1, 0
            };

            THEN("The position can be multiplied by the matrix")
            {
                Position<3, int> expected = { 10+10, 10+15, 10+10 };
                REQUIRE( (pos3*matrix) == expected);

                pos3*=matrix;
                REQUIRE( pos3 == expected);
            }

        }

        GIVEN("A displacement of dimension 3")
        {
            Vec<3, int> vec3{0, 20, -5};

            THEN("The displacement can be added to the position")
            {
                Position<3, int> expected{5, 30, 10};
                REQUIRE(pos3+vec3 == expected);
            }

            THEN("The displacement can me substracted from the position")
            {
                Position<3, int> expected{5, -10, 20};
                REQUIRE(pos3-vec3 == expected);
            }
        }

        GIVEN("Another position of dimension 3")
        {
            Position<3, int> pos3b{5, 30, 5};

            THEN("Substraction the first position from the second gives the displacement")
            {
                Vec<3, int> expected{0, 20, -10};
                REQUIRE(pos3b-pos3 == expected);
                REQUIRE(pos3b == pos3+expected);
            }
        }
    }
}


template<class T>
class has_area
{
    template <size_t> struct exists;

    template <class C> static std::true_type  test(exists<sizeof(decltype(std::declval<C>().area()))>*);
    template <class C> static std::false_type test(...);

public:
    //static bool const value = std::is_same<decltype(test<T>(0)), std::true_type>::value;
    static bool const value = decltype(test<T>(nullptr))::value;
};

HAS(area)
HAS(volume)

SCENARIO("Size class operations")
{
    GIVEN("A size instance of dimension 2")
    {
        Size<2> size2{2., 5.};

        THEN("Values are accessible via subscript operator")
        {
            REQUIRE(size2[0]  == 2.);
            REQUIRE(size2[1]  == 5.);
        }

        THEN("Values are accessible via accessors")
        {
            REQUIRE(size2.width()  == 2.);
            REQUIRE(size2.height() == 5.);
        }

        THEN("Its area can be computed")
        {
            REQUIRE(size2.area() == 2.*5.);
        }

        THEN("Volume member function is disabled")
        {
            REQUIRE_FALSE(is_detected_v<has_volume_t, Size<2>>);
        }
    }

    GIVEN("A size instance of dimension 3")
    {
        Size<3> box{2., 4., 3.};

        THEN("Elements are accessible via accessors")
        {
            REQUIRE(box.width()  == 2.);
            REQUIRE(box.height() == 4.);
            REQUIRE(box.depth()  == 3.);
        }

        THEN("Its volume can be computed")
        {
            REQUIRE(box.volume() == 2.*4.*3.);
        }

        THEN("Area member function is disabled")
        {
            // Custom check
            REQUIRE_FALSE(has_area<Size<3>>::value);

            // Equivalent, but using detection experimental feature
            REQUIRE_FALSE(is_detected<has_area_t, Size<3>>::value);
        }
    }
}
