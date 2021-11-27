#include "catch.hpp"

#include <math/Vector.h>


using namespace ad::math;


/// This illustrate a concern related to equality comparison in inheritance hierachies (C87)
/// see: https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c87-beware-of--on-base-classes 
/// Of course, those classes should not be inherited, maybe they should simply be marked final.
SCENARIO("Equality and inheritance", "[.]")
{
    GIVEN("Instances from two classes inheriting separately from the same vector type.")
    {
        struct Foo : public Vec<2, double>
        {
            Foo() :
                Vec<2, double>{0., 0.}
            {}

            int a;
        };

        struct Bar : public Vec<2, double>
        {
            Bar() :
                Vec<2, double>{0., 0.}
            {}

            double b;
        };

        Foo a;
        Bar b;

        THEN("They cannot be compared")
        {
            // This should not compile !
            REQUIRE(a != b);
        }
    }
}
