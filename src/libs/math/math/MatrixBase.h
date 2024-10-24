#pragma once

#include "MatrixTraits.h"

#include <algorithm>
#include <array>
#include <iostream>

#include <reflexion/NameValuePair.h>


namespace ad {
namespace math {


#define TMP class T_derived, int N_rows, int N_cols, class T_number
#define TMT class, int, int, class
#define TMA T_derived, N_rows, N_cols, T_number

#define TMA_RIGHT T_derivedRight, N_rows, N_cols, T_number


// Forward declaration
template <class, int, class> class Vector;


namespace detail {


class CastTag
{
    template <TMP>
    friend class MatrixBase;
};


// Required to be able to deduce Row template parameters in the Vector ctor taking a Row(Base).
// see: https://stackoverflow.com/a/12640996/1027706
template <int N_cols, class T_data>
class RowBase
{
    template <class, int, class> 
    friend class ::ad::math::Vector;

protected:
    constexpr RowBase(T_data * aData) noexcept :
        mRow{aData}
    {}

    // The const member function return pointer to non-const, since it is modeled after iterators.
    constexpr T_data * data() const noexcept
    { return mRow; }

    T_data * mRow;
};


} // namespace detail


template <TMP>
class MatrixBase
{
public:
    static constexpr int size_value = N_rows*N_cols;

private:
    typedef std::array<T_number, size_value> store_type;

public:
    typedef typename store_type::value_type value_type; // i.e. T_number
    typedef typename store_type::iterator iterator;
    typedef typename store_type::const_iterator const_iterator;

    static constexpr std::size_t Rows{N_rows};
    static constexpr std::size_t Cols{N_cols};

public:
    class Row : public detail::RowBase<Cols, T_number>
    {
        friend class MatrixBase;

        using BaseType = detail::RowBase<Cols, T_number>;

        constexpr explicit Row(T_number *aRow) noexcept:
            BaseType{aRow}
        {}

        // Only allow move-construction, required to return from Matrix::operator[]
        constexpr Row(Row &&) noexcept = default;
        Row & operator=(Row &&) = delete;
        Row(const Row &) = delete;
        Row & operator=(const Row &) = delete;

    public:
        constexpr T_number & operator[](std::size_t aColumn)
        { return BaseType::data()[aColumn]; }
    };

    class const_Row : public detail::RowBase<Cols, const T_number>
    {
        friend class MatrixBase;

        using BaseType = detail::RowBase<Cols, const T_number>;

        constexpr explicit const_Row(const T_number *aRow) noexcept:
            BaseType{aRow}
        {}

        // Only allow move-construction, required to return from Matrix::operator[]
        constexpr const_Row(const_Row &&) noexcept = default;
        const_Row & operator=(const_Row &&) = delete;
        const_Row(const const_Row &) = delete;
        const_Row & operator=(const const_Row &) = delete;

    public:
        constexpr T_number operator[](std::size_t aColumn)
        { return BaseType::data()[aColumn]; }
    };

protected:
    // Implementer note:
    // This is definitely too conservative, but I don't feel confident in my understanding of
    // noexcept at the moment. (And this class is intended for arithmetic types)
    // Also, this should probably be different depending on the member function.
    static constexpr bool should_noexcept = std::is_arithmetic<value_type>::value;

public:
    /// \brief Default constructor initializes to zero.
    /// \note Initially there was no default constructor.
    /// Yet having default construction behave like zero init of built-in type simplify generic programming.
    /// \note The defaulted default-ctor would return unitialized memory.
    constexpr MatrixBase() noexcept(should_noexcept)
    { setZero(); }

    // Implementer note:
    // I don't feel confident enough with my understanding of aggregate initialization
    // (see: https://en.cppreference.com/w/cpp/language/aggregate_initialization)
    // to know when it can and cannot throw exceptions. So be conservative and noexcept(false).
    //
    // Must be made explicit, otherwise it would be available for implicit conversions
    // in any context expecting a 1x1 matrix.
    // E.g., the ctor of an AffineMatrix<2> takes a Matrix<1, 1> as its first argument,
    // we don't want this MatrixBase ctor to try and convert anything to a Matrix<1, 1>.
    //
    // Yet that causes complication where we construct derived types by list initialization
    // (forcing to type out the type, because it is explicit even with several elements)
    // So it is split between the single element (explicit case),
    // and the several elements case (not explicit)

    /// \brief Constructor from explicit element values.
    /// \note std::array does not have a std::initializer_list constructor, but aggregate initialization.
    /// \note enable_if to only allow this ctor when the right number of arguments is provided
    ///       (notably prevents this constructor from being selected as the default ctor)
    /// \attention the provided values' type is not allowed potential loss of precision.
    template <class... T_element,
              std::enable_if_t<sizeof...(T_element) == N_rows*N_cols && (N_rows*N_cols > 1), int> = 0>
    constexpr MatrixBase(T_element... vaElements) /*noexcept (see note)*/;

    // Note: split because of a complication with explicit handling of the ctor above (see comments).
    // (C++ 20 introduce conditional explicit, which could be used instead)
    // This constructor is restricted to the case where the matrix is 1 element exactly.
    // Note: Without `requires` clause, one could rely on SFINAE, by templating the method with
    //       <typename = std::enable_if_t<(N_rows*N_cols == 1)>>
    constexpr explicit MatrixBase(T_number aSingleElement) requires (N_rows*N_cols == 1) /*noexcept (see note)*/;

    /// \brief Explicit cast to another derived type of same dimensions and scalar type
    template <class T_otherDerived,
              class = std::enable_if_t<std::is_base_of<MatrixBase<T_otherDerived,
                                                                  N_rows,
                                                                  N_cols,
                                                                  T_number>,
                                                       T_otherDerived>::value,
                                       T_otherDerived>>
    constexpr explicit operator T_otherDerived () const noexcept(should_noexcept);

    /// Not possible, that leads to a duplicate conversion operator error
    ///// \brief Explicit cast to the same derived type, except with a different value_type
    //template <class T_otherDerived,
    //          class = std::enable_if_t<std::is_same<typename T_otherDerived::template derived_type<MatrixBase::value_type>,
    //                                                 T_derived>::value>>
    //explicit operator T_otherDerived () const;

    // NOTE: This is not a copy constructor because it is templated
    // see: https://en.cppreference.com/w/cpp/language/copy_constructor
    // NOTE: Disabled if the value_type is the same, in which case the explicit conversion
    // operator could have better performance
    // NOTE: std::is_convertible is not the exact requirement here, but were are not aware
    // of a better alternative in the type support library
    // (std::is_assignable not working for int and double, for example)
    /// \brief Explicit cast to a different value_type, with matching dimensions,
    ///        wether or not the derived type is the same
    /// \attention Can only be used in a constexpr context since C++20
    template <class T_otherDerived, class T_otherNumber,
              class = std::enable_if_t<   (! std::is_same<T_number, T_otherNumber>::value)
                                       && (std::is_convertible<T_otherNumber, T_number>::value)>>
    constexpr explicit MatrixBase(const MatrixBase<T_otherDerived,
                                                   N_rows,
                                                   N_cols,
                                                   T_otherNumber> & aOther)
    noexcept(should_noexcept);

    /// \brief Sets all elements to zero.
    constexpr T_derived & setZero() noexcept(should_noexcept);

    /// \brief Returns a Matrix with all elements set to 0.
    static constexpr T_derived Zero() noexcept(should_noexcept);


    /// \brief Access a row of this matrix by the row's index.
    ///
    /// It is essentially intended to be immediately followed by a second access,
    /// to the column of the matrix.
    constexpr Row operator[](std::size_t aRow);
    constexpr const_Row operator[](std::size_t aRow) const;


    /// \brief Iterate one line at a time, going through each column in the line
    /// before descending to the next line.
    constexpr const_iterator cbegin() const noexcept;
    constexpr const_iterator cend() const noexcept;
    // Also implemented to enable range for loop
    constexpr iterator begin() noexcept;
    constexpr iterator end() noexcept;
    constexpr const_iterator begin() const noexcept;
    constexpr const_iterator end() const noexcept;

    /// \brief Returns an iterator to the matrix element with the lowest magnitude.
    constexpr iterator getMinMagnitudeElement() noexcept(should_noexcept);
    constexpr const_iterator getMinMagnitudeElement() const noexcept(should_noexcept);
    /// \brief Returns an iterator to the matrix element with the biggest magnitude.
    constexpr iterator getMaxMagnitudeElement() noexcept(should_noexcept);
    constexpr const_iterator getMaxMagnitudeElement() const noexcept(should_noexcept);

    // TODO Ad 2022/04/07: Should at change to the usual meaning of **checked** access?
    // In which case, a non-checked alternative should still be provided, and existing
    // code in this lib should be changed to use it.
    /// \brief Unchecked access to an element by its linear index, i.e. `row*N_rows + column`
    constexpr T_number & at(std::size_t aIndex);
    /// \brief Unchecked access to an element by its row and column index.
    /// \note The order of parameters (row then column) matches usual M_ij mathematical notation.
    constexpr T_number & at(std::size_t aRow, std::size_t aColumn);
    constexpr T_number at(std::size_t aIndex) const;
    constexpr T_number at(std::size_t aRow, std::size_t aColumn) const;

    /// \brief Returns a pointer to the matrix storage, a contiguous sequence of elements.
    constexpr T_number * data() noexcept;
    constexpr const T_number * data() const noexcept;


    // Allows for compound addition of other derived types, depending on the derived traits
    template <class T_derivedRight>
    constexpr additive_t<T_derived, T_derivedRight> &
    operator+=(const MatrixBase<TMA_RIGHT> &aRhs) noexcept(should_noexcept);
    template <class T_derivedRight>
    constexpr additive_t<T_derived, T_derivedRight> &
    operator-=(const MatrixBase<TMA_RIGHT> &aRhs) noexcept(should_noexcept);

    template <class T_scalar>
    constexpr std::enable_if_t<! from_matrix_v<T_scalar>, T_derived &>
    operator*=(T_scalar aScalar) noexcept(should_noexcept);
    template <class T_scalar>
    constexpr std::enable_if_t<! from_matrix_v<T_scalar>, T_derived &>
    operator/=(T_scalar aScalar) noexcept(should_noexcept);

    constexpr T_derived operator-() const noexcept(should_noexcept);

    /// \brief The compound componentwise multiplication
    constexpr T_derived & cwMulAssign(const MatrixBase &aRhs) noexcept(should_noexcept);
    /// \brief The componentwise multiplication
    /// \note Made a member function for the syntax with the 'operator name' in between operands
    constexpr T_derived cwMul(T_derived aRhs) const noexcept(should_noexcept);

    /// \brief The compound componentwise division
    constexpr T_derived & cwDivAssign(const MatrixBase &aRhs) noexcept(should_noexcept);
    /// \brief The componentwise division
    constexpr T_derived cwDiv(const T_derived &aRhs) const noexcept(should_noexcept);

    /// \brief The compound componentwise modulo
    constexpr T_derived & cwModAssign(const MatrixBase &aRhs) noexcept(should_noexcept);
    /// \brief The componentwise modulo
    constexpr T_derived cwMod(const T_derived &aRhs) const noexcept(should_noexcept);

    // Implementer note:
    // Those two operations were initially member functions (single explicit parameter).
    // Yet, it prevented to use implicit conversions on arguments.
    // Attention:
    // I am a bit worried by C87, in case client code would inherit somme of the T_derived types.
    // see: https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c87-beware-of--on-base-classes
    // see: EqualityDanger_tests.cpp
    /// \brief Equality comparison.
    /// \attention Can be used in a constexpr context only since C++20
    friend constexpr bool operator==(const T_derived & aLhs, const T_derived &aRhs) noexcept(should_noexcept)
    { return aLhs.mStore == aRhs.mStore; }
    /// \brief Non-equality comparison.
    /// \attention Can be used in a constexpr context only since C++20
    friend constexpr bool operator!=(const T_derived & aLhs, const T_derived &aRhs) noexcept(should_noexcept)
    { return !(aLhs == aRhs); }

    /// \brief Return true if elements of `aRhs` are all within the tolerance `aEpsilon` of corresponding
    /// elements in this matrix.
    ///
    /// \note This can be used to test if two instances are almost equal.
    constexpr bool equalsWithinTolerance(const MatrixBase & aRhs, T_number aEpsilon) const noexcept(should_noexcept);


    // Implementer note:
    // Those two operations were initially free functions, notably templated on T_derived.
    // Yet, it prevented them from being available for types derived from T_derived.
    // see: https://stackoverflow.com/a/9789036/1027706
    template <class T_derivedRight>
    friend constexpr additive_t<T_derived, T_derivedRight>
    operator+(T_derived aLhs, const MatrixBase<TMA_RIGHT>& aRhs)
    { return aLhs += aRhs; }

    template <class T_derivedRight>
    friend constexpr additive_t<T_derived, T_derivedRight>
    operator-(T_derived aLhs, const MatrixBase<TMA_RIGHT>& aRhs)
    { return aLhs -= aRhs; }


protected:
    constexpr T_derived * derivedThis() noexcept;
    constexpr const T_derived * derivedThis() const noexcept;

    // Implementer note: defaulted operations seems to deduce constexpr (and hopefully noexcept)
    MatrixBase(const MatrixBase &aRhs) = default;
    MatrixBase & operator=(const MatrixBase &aRhs) = default;

    // NOTE: std::array is movable if its elements are, i.e. it will move each of them
    // see: https://stackoverflow.com/a/14370753/1027706
    MatrixBase(MatrixBase && aRhs) = default;
    MatrixBase & operator=(MatrixBase && aRhs) = default;

    template <class T_extracted, std::size_t... VN_indices>
    T_extracted extract(std::index_sequence<VN_indices...> aIndices) const noexcept(should_noexcept);

public:

    // Implementer note: At first, this was a protected struct of Matrix template
    // yet, in some situation implementations need an un-initialized matrix of different dimensions
    // e.g. transpose operation
    // 2021/12/20 Ad Note: At second, the constructor was private, with a list of friend class templates.
    // Yet it is also used in a few highly templated free-functions, which make the friend list hard to maintain.
    // Make the tag usable publicly, hopefully it will not appear in user code.
    /// \attention Users, please refrain from using this internal tag.
    struct UninitializedTag
    {
        constexpr UninitializedTag() = default;
    };

    /// \brief Like a default constructor, but inaccessible to the client code thanks to the protected tag.
    constexpr explicit MatrixBase(UninitializedTag) noexcept(should_noexcept);

    constexpr MatrixBase(detail::CastTag, store_type aData)
            noexcept(std::is_nothrow_move_constructible<value_type>::value);

    template<class T_witness>
    void describeTo(T_witness && w)
    {
        w.witness(NVP(mStore));
    }

private:
    store_type mStore;
};

/*
 * Free function arithmetic operators
 */

// Implementer note:
//   Was simple and elegant, matching the T_derived in the first argument (by value to be modified)
//   with the T_derived in the MatrixBase
//   Yet it only allowed addition for matching derived types (and could not disable it)
//template <TMP>
//T_derived operator+(T_derived aLhs, const MatrixBase<TMA> &aRhs);

// Implementer note:
//   should_noexcept should not be part of the API, so it cannot be easily used here

template <TMP, class T_scalar>
constexpr std::enable_if_t<! from_matrix_v<T_scalar>, T_derived>
operator*(const MatrixBase<TMA> &aLhs, T_scalar aScalar)
/*noexcept(T_derived::should_noexcept)*/;

template <TMP, class T_scalar>
constexpr std::enable_if_t<! from_matrix_v<T_scalar>, T_derived>
operator*(T_scalar aScalar, const MatrixBase<TMA> &aRhs)
/*noexcept(T_derived::should_noexcept)*/;

template <TMP, class T_scalar>
constexpr std::enable_if_t<! from_matrix_v<T_scalar>, T_derived>
operator/(const MatrixBase<TMA> &aLhs, T_scalar aScalar)
/*noexcept(T_derived::should_noexcept)*/;


/*
 * Component-wise operations
 */
template <TMP>
T_derived min(const MatrixBase<TMA> & aLhs, const MatrixBase<TMA> & aRhs)
noexcept(T_derived::should_noexcept);


template <TMP>
T_derived max(const MatrixBase<TMA> & aLhs, const MatrixBase<TMA> & aRhs)
noexcept(T_derived::should_noexcept);


template <TMP>
T_derived abs(const MatrixBase<TMA> & aMatrix)
noexcept(T_derived::should_noexcept);


template <TMP>
T_derived floor(const MatrixBase<TMA> & aMatrix)
noexcept(T_derived::should_noexcept);


template <TMP>
T_derived ceil(const MatrixBase<TMA> & aMatrix)
noexcept(T_derived::should_noexcept);


template <TMP>
T_derived round(const MatrixBase<TMA> & aMatrix)
noexcept(T_derived::should_noexcept);


/*
 * Output operator
 */
template<TMP>
std::ostream & operator<<(std::ostream & os, const MatrixBase<TMA> &aMatrix);


namespace detail
{

    std::false_type test_matrix_convertible(...);

    template <TMP>
    std::true_type test_matrix_convertible(const ::ad::math::MatrixBase<TMA> &);

} // namespace detail


template <class T>
class from_matrix : public decltype(detail::test_matrix_convertible(std::declval<T>()))
{};


} // namespace math
} // namespace ad


#include "MatrixBase-impl.h"


#undef TMA_RIGHT
#undef TMA
#undef TMT
#undef TMP
