#include "catch.hpp"

#include <array>
#include <string>


SCENARIO("Prototyping around what is noexcept")
{
    WHEN("Using types able to throw excpetions (such as std::string)")
    {
        std::string a("alpha");
        std::string o("omega");
        REQUIRE_FALSE(noexcept(std::array<std::string, 2>{ "alpha", "omega" }));
        REQUIRE_FALSE(noexcept(std::array<std::string, 2>{ std::string("alpha"), std::string("omega") }));
        REQUIRE_FALSE(noexcept(std::array<std::string, 2>{ a, o}));

        {
            std::string a("alpha");
            std::string o("omega");
            REQUIRE(noexcept(std::array<std::string, 2>{ std::move(a), std::move(o)}));
        }
    }
}
