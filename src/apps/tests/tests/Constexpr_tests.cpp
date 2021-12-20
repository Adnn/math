#include "catch.hpp"

#include <math/Matrix.h>
#include <math/Vector.h>


using namespace ad::math;


template <class T_left, class T_right>
constexpr T_left assign(T_left aDestination, T_right && aSource)
{
    aDestination = std::forward<T_right>(aSource);
    return aDestination;
}


#define COMPOUND(name, operation) \
    /*templat* <class T_matrix>*/ \
    constexpr Matrix<2, 2, int> compound##name(const Matrix<2, 2, int> & aRhs) \
    { \
        Matrix<2, 2, int> cMatrix{0, 0, 0, 0}; \
        cMatrix.operation(aRhs); \
        return cMatrix; \
    }

COMPOUND(Addition, operator+=)
COMPOUND(Substraction, operator-=)
COMPOUND(ComponentWiseMultiplication, cwMulAssign)
COMPOUND(ComponentWiseDivision, cwDivAssign)
COMPOUND(MatrixMultiplication, operator*=)

#undef COMPOUND


#define COMPOUNDSCALAR(name, operation) \
    template <class T_matrix>                                                       \
    constexpr T_matrix compoundScalar##name(T_matrix aMatrix,                       \
                                            typename T_matrix::value_type aScalar)  \
    {                                                                               \
        aMatrix.operation(aScalar);                                                 \
        return aMatrix;                                                             \
    }

COMPOUNDSCALAR(Multiplication, operator*=)
COMPOUNDSCALAR(Division, operator/=)

#undef COMPOUNDSCALAR


SCENARIO("Matrices can be constant expressions")
{
    GIVEN("A constexpr Matrix")
    {
        constexpr Matrix<2, 2, int> cMatrix{
            11, 12,
            21, 22
        };

        THEN("It can be copied to a constant expression matrix of same type")
        {
            constexpr Matrix<2, 2, int> cCopyMatrix{cMatrix};
            REQUIRE(std::integral_constant<bool, cCopyMatrix[0][0] == 11>::value);

            constexpr Matrix<2, 2, int> cAssignedMatrix{5, 5, 5, 5};
            REQUIRE(std::integral_constant<bool, cAssignedMatrix[0][0] != 11>::value);
            REQUIRE(std::integral_constant<bool, assign(cAssignedMatrix, cMatrix)[0][0] == 11>::value);
        }

        THEN("It can be moved to a constant expression matrix of same type")
        {
            constexpr Matrix<2, 2, int> cCopyMatrix{std::move(cMatrix)};
            REQUIRE(std::integral_constant<bool, cCopyMatrix[0][0] == 11>::value);

            constexpr Matrix<2, 2, int> cAssignedMatrix{5, 5, 5, 5};
            REQUIRE(std::integral_constant<bool, cAssignedMatrix[0][0] != 11>::value);
            REQUIRE(std::integral_constant<bool,
                    assign(cAssignedMatrix, std::move(cMatrix))[0][0] == 11>::value);
        }

#if __cplusplus > 201703L
        THEN("It can be casted to a constant expression matrix of differing element type")
        {
            constexpr Matrix<2, 2, double> cDoubleMatrix{cMatrix};
            REQUIRE(std::integral_constant<bool, cDoubleMatrix[0][0] == 11.0>::value);
        }
#endif

        THEN("Its elements are accessible as constant expressions")
        {
            REQUIRE(std::integral_constant<bool, cMatrix[0][0] == 11>::value);
            REQUIRE(std::integral_constant<bool, cMatrix[1][0] == 21>::value);

            REQUIRE(std::integral_constant<bool, cMatrix.at(1) == 12>::value);
            REQUIRE(std::integral_constant<bool, cMatrix.at(3) == 22>::value);

            REQUIRE(std::integral_constant<bool, *(cMatrix.data()) == 11>::value);
        }

        THEN("It can be negated")
        {
            REQUIRE(std::integral_constant<bool, (-cMatrix)[0][0] == -11>::value);
            REQUIRE(std::integral_constant<bool, (-cMatrix)[1][0] == -21>::value);
        }

        THEN("Iterator access as constant expressions")
        {
            REQUIRE(std::integral_constant<bool, *(cMatrix.begin() + 1) == 12>::value);
            REQUIRE(std::integral_constant<bool, std::distance(cMatrix.begin(), cMatrix.end()) == 4>::value);
        }

        GIVEN("A second constexpr Matrix")
        {
            constexpr Matrix<2, 2, int> cMatrixTwo{
                111, 112,
                121, 122
            };

#if __cplusplus > 201703L
            // std::array::operator== is constexpr only since C++20
            THEN("Equality comparison results are constant expressions")
            {
                REQUIRE_FALSE(std::bool_constant<cMatrix == cMatrixTwo>::value);
                REQUIRE(std::bool_constant<cMatrix != cMatrixTwo>::value);
                REQUIRE(std::bool_constant<cMatrix == cMatrix>::value);
                REQUIRE(std::bool_constant<cMatrixTwo == cMatrixTwo>::value);
            }
#endif

            THEN("Additions are constant expressions")
            {
                constexpr Matrix<2, 2, int> three = cMatrix + cMatrixTwo;
                REQUIRE(std::bool_constant<three[0][0] == 122>::value);

                constexpr Matrix<2, 2, int> compound = compoundAddition(cMatrixTwo);
                REQUIRE(std::bool_constant<compound[0][0] == 111>::value);
            }

            THEN("Substractions are constant expressions")
            {
                constexpr Matrix<2, 2, int> three = cMatrix - cMatrixTwo;
                REQUIRE(std::bool_constant<three[0][0] == -100>::value);

                constexpr Matrix<2, 2, int> compound = compoundSubstraction(cMatrixTwo);
                REQUIRE(std::bool_constant<compound[0][0] == -111>::value);
            }

            THEN("Component-wise mulitplications are constant expressions")
            {
                constexpr Matrix<2, 2, int> three = cMatrix.cwMul(cMatrixTwo);
                REQUIRE(std::bool_constant<three[0][0] == 1221>::value);

                constexpr Matrix<2, 2, int> compound =
                    compoundComponentWiseMultiplication(cMatrixTwo);
                REQUIRE(std::bool_constant<compound[0][0] == 0>::value);
            }

            THEN("Component-wise divisions are constant expressions")
            {
                constexpr Matrix<2, 2, int> three = cMatrixTwo.cwDiv(cMatrix);
                REQUIRE(std::bool_constant<three[0][0] == 10>::value);

                constexpr Matrix<2, 2, int> compound =
                    compoundComponentWiseDivision(cMatrixTwo);
                REQUIRE(std::bool_constant<compound[0][0] == 0>::value);
            }

            THEN("Matrix mulitplications are constant expressions")
            {
                constexpr Matrix<2, 2, int> three = cMatrix * cMatrixTwo;
                REQUIRE(std::bool_constant<three[0][0] == 2673>::value);

                constexpr Matrix<2, 2, int> compound = compoundMatrixMultiplication(cMatrixTwo);
                REQUIRE(std::bool_constant<compound[0][0] == 0>::value);
            }
        }

        GIVEN("A constexpr scalar value of matching type")
        {
            constexpr int scalar{10};

            THEN("The matrix can be multiplied by the scalar")
            {
                constexpr Matrix<2, 2, int> post = cMatrix * scalar;
                REQUIRE(std::bool_constant<post[0][0] == 110>::value);
                REQUIRE(std::bool_constant<post[1][0] == 210>::value);

                constexpr Matrix<2, 2, int> pre = scalar * cMatrix;
                REQUIRE(std::bool_constant<post.at(0) == pre.at(0)>::value);
                REQUIRE(std::bool_constant<post.at(1) == pre.at(1)>::value);
                REQUIRE(std::bool_constant<post.at(2) == pre.at(2)>::value);
                REQUIRE(std::bool_constant<post.at(3) == pre.at(3)>::value);

                constexpr Matrix<2, 2, int> compound =
                    compoundScalarMultiplication(cMatrix, scalar);
                REQUIRE(std::bool_constant<post.at(0) == compound.at(0)>::value);
                REQUIRE(std::bool_constant<post.at(1) == compound.at(1)>::value);
                REQUIRE(std::bool_constant<post.at(2) == compound.at(2)>::value);
                REQUIRE(std::bool_constant<post.at(3) == compound.at(3)>::value);
            }

            THEN("The matrix can be divided by the scalar")
            {
                constexpr Matrix<2, 2, int> post = cMatrix / scalar;
                REQUIRE(std::bool_constant<post[0][0] == 1>::value);
                REQUIRE(std::bool_constant<post[1][0] == 2>::value);

                // No scalar pre-division

                constexpr Matrix<2, 2, int> compound =
                    compoundScalarDivision(cMatrix, scalar);
                REQUIRE(std::bool_constant<post.at(0) == compound.at(0)>::value);
                REQUIRE(std::bool_constant<post.at(1) == compound.at(1)>::value);
                REQUIRE(std::bool_constant<post.at(2) == compound.at(2)>::value);
                REQUIRE(std::bool_constant<post.at(3) == compound.at(3)>::value);
            }
        }
    }

    GIVEN("A constexpr zero Matrix")
    {
        constexpr auto cMatrix = Matrix<4, 6, float>::Zero();

        THEN("Its elements are accessible as constant expressions")
        {
            REQUIRE(std::integral_constant<bool, cMatrix[0][0] == 0>::value);
            REQUIRE(std::integral_constant<bool, cMatrix[1][0] == 0>::value);
        }
    }

    GIVEN("A constexpr indentity Matrix")
    {
        constexpr auto cMatrix = Matrix<4, 4, std::uint8_t>::Identity();

        THEN("Its elements are accessible as constant expressions")
        {
            REQUIRE(std::integral_constant<bool, cMatrix[0][0] == 1>::value);
            REQUIRE(std::integral_constant<bool, cMatrix[1][1] == 1>::value);
            REQUIRE(std::integral_constant<bool, cMatrix[2][2] == 1>::value);
            REQUIRE(std::integral_constant<bool, cMatrix[3][3] == 1>::value);

            REQUIRE(std::integral_constant<bool, cMatrix[0][1] == 0>::value);
            REQUIRE(std::integral_constant<bool, cMatrix[1][0] == 0>::value);
        }
    }
}


SCENARIO("Specialized vector types can be constant expressions")
{
    GIVEN("A constexpr Position of doubles")
    {
        constexpr Position<3, double> cPosition{ 0.0, 1.0, 2.0 };

        THEN("Its elements can be accessed via a \"1D\" subscript operator")
        {
            REQUIRE(std::bool_constant<cPosition[0] == 0.0>::value);
            REQUIRE(std::bool_constant<cPosition[1] == 1.0>::value);
            REQUIRE(std::bool_constant<cPosition[2] == 2.0>::value);
        }

        THEN("It can be casted to a constexpr Vec of doubles")
        {
            constexpr Vec<3, double> cOffset{cPosition};
            REQUIRE(std::bool_constant<cPosition[0] == cOffset[0]>::value);
            REQUIRE(std::bool_constant<cPosition[1] == cOffset[1]>::value);
            REQUIRE(std::bool_constant<cPosition[2] == cOffset[2]>::value);
        }

        THEN("It can be explicitly obtained ad() a constexpr Vec of doubles")
        {
            constexpr Vec<3, double> cOffset = cPosition.as<Vec>();
            REQUIRE(std::bool_constant<cPosition[0] == cOffset[0]>::value);
            REQUIRE(std::bool_constant<cPosition[1] == cOffset[1]>::value);
            REQUIRE(std::bool_constant<cPosition[2] == cOffset[2]>::value);
        }

#if __cplusplus > 201703L
        //std::copy used by conversion constructor is constexpr sinc C++20
        THEN("It can be casted to a constexpr Vec of ints")
        {
            constexpr Vec<3, int> cOffset{cPosition};
            REQUIRE(std::bool_constant<cOffset[0] == cPosition[0]>::value);
            REQUIRE(std::bool_constant<cOffset[1] == cPosition[1]>::value);
            REQUIRE(std::bool_constant<cOffset[2] == cPosition[2]>::value);
        }
#endif

        THEN("The specialized accessors are constant expressions")
        {
            REQUIRE(std::bool_constant<cPosition.x() == 0.0>::value);
            REQUIRE(std::bool_constant<cPosition.y() == 1.0>::value);
            REQUIRE(std::bool_constant<cPosition.z() == 2.0>::value);
        }
    }

    GIVEN("A constexpr Vec of int")
    {
        constexpr Vec<2, int> cOffset{2, 2};

        THEN("It can be multplied by a constexp Matrix")
        {
            constexpr Matrix<2, 2, int> cMatrix{1, 0, 0, 2};

            constexpr Vec<2, int> cMultiplied = cOffset * cMatrix;
            REQUIRE(std::bool_constant<cMultiplied[0] == 2>::value);
            REQUIRE(std::bool_constant<cMultiplied[1] == 4>::value);
        }

        THEN("Vec special operations are constant expressions")
        {
            constexpr int norm = cOffset.getNormSquared();
            constexpr int dot = cOffset.dot(cOffset);
            REQUIRE(std::bool_constant<norm == dot>::value);
        }
    }
}
