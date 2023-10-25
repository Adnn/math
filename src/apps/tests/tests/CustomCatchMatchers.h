#include "catch.hpp"

#include <sstream>


template <class T_math>
requires requires(T_math lhs, T_math rhs, typename T_math::value_type v)
    {
        {lhs.equalsWithinTolerance(rhs, v)} -> std::same_as<bool>;
    }
class MathApprox : public Catch::MatcherBase<T_math>
{
    using tolerance_type = typename T_math::value_type;

public:
    MathApprox(T_math aExpected, tolerance_type aTolerance) :
        mExpected{std::move(aExpected)},
        mTolerance{std::move(aTolerance)}
    {}

    // Performs the test for this matcher
    bool match(const T_math & aMatrix) const override
    {
        return aMatrix.equalsWithinTolerance(mExpected, mTolerance);
    }

    // Produces a string describing what this matcher does. It should
    std::string describe() const override
    {
        std::ostringstream ss;
        ss << "equals " << mExpected << " within a tolerance of " << mTolerance;
        return ss.str();
    }

private:
    T_math mExpected;
    tolerance_type mTolerance;
};


// The builder function
template <class T_math>
MathApprox<std::remove_cvref_t<T_math>> Approximates(
        T_math && aExpected,
        typename std::remove_cvref_t<T_math>::value_type aTolerance = 
            std::numeric_limits<typename std::remove_cvref_t<T_math>::value_type>::epsilon())
{
    return MathApprox<std::remove_cvref_t<T_math>>{
        std::forward<T_math>(aExpected), aTolerance};
}
