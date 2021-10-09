#include "catch.hpp"

#include <sstream>


template <class T_matrix>
class MatrixApprox : public Catch::MatcherBase<T_matrix>
{
    using tolerance_type = typename T_matrix::value_type;

public:
    MatrixApprox(
            T_matrix aExpected,
            tolerance_type aTolerance = ) :
        mExpected{std::move(aExpected)},
        mTolerance{std::move(aTolerance)}
    {}

    // Performs the test for this matcher
    bool match(const T_matrix & aMatrix) const override
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
    T_matrix mExpected;
    typename T_matrix::value_type mTolerance;
};


// The builder function
template <class T_matrix>
MatrixApprox<T_matrix> Approximates(
        T_matrix && aExpected,
        typename T_matrix::value_type aTolerance = std::numeric_limits<T_matrix::value_type>::epsilon())
{
    return MatrixApprox(std::forward<T_matrix>(aExpected),
                        aTolerance);
}
