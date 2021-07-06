#pragma once

#include "MatrixTraits.h"

#include <array>
#include <iostream>


namespace ad {
namespace math {


#define TMP class T_derived, int N_rows, int N_cols, class T_number
#define TMA T_derived, N_rows, N_cols, T_number

#define TMA_RIGHT T_derivedRight, N_rows, N_cols, T_number


namespace detail {


class CastTag
{
    template <TMP>
    friend class MatrixBase;
};


} // namespace detail


template <TMP>
class MatrixBase
{
    static constexpr int size_value = N_rows*N_cols;
    typedef std::array<T_number, size_value> store_type;

public:
    typedef typename store_type::value_type value_type; // i.e. T_number
    typedef typename store_type::const_iterator const_iterator;

    static constexpr std::size_t Rows{N_rows};
    static constexpr std::size_t Cols{N_cols};

private:
    class Row
    {
        friend class MatrixBase;

        constexpr explicit Row(T_number *aRow) noexcept:
            mRow(aRow)
        {}

        // Only allow move-construction, required to return from Matrix::operator[]
        constexpr Row(Row &&) noexcept = default;
        Row & operator=(Row &&) = delete;
        Row(const Row &) = delete;
        Row & operator=(const Row &) = delete;

    public:
        constexpr T_number & operator[](std::size_t aColumn)
        { return mRow[aColumn]; }

    private:
        T_number *mRow;
    };

    class const_Row
    {
        friend class MatrixBase;

        constexpr explicit const_Row(const T_number *aRow) noexcept:
            mRow(aRow)
        {}

        // Only allow move-construction, required to return from Matrix::operator[]
        constexpr const_Row(const_Row &&) noexcept = default;
        const_Row & operator=(const_Row &&) = delete;
        const_Row(const const_Row &) = delete;
        const_Row & operator=(const const_Row &) = delete;

    public:
        constexpr T_number operator[](std::size_t aColumn)
        { return mRow[aColumn]; }

    private:
        const T_number *mRow;
    };

protected:
    // Implementer note:
    // This is definitely too conservative, but I don't feel confident in my understanding of
    // noexcept at the moment. (And this class is intended for arithmetic types)
    // Also, this should probably be different depending on the member function.
    static constexpr bool should_noexcept = std::is_arithmetic<value_type>::value;

public:
    /// \note: std::array does not have a std::initializer_list constructor, but aggregate initialization.
    /// \note: enable_if to only allow this ctor when the right number of arguments is provided
    ///        (notably prevents this constructor from being selected as the default ctor)
    // Implementer note:
    // I don't feel confident enought with my understanding of aggregate initialization
    // (see: https://en.cppreference.com/w/cpp/language/aggregate_initialization)
    // to know when it can and cannot throw exceptions. So be conservative and noexcept(false).
    template <class... T_element,
              std::enable_if_t<sizeof...(T_element) == N_rows*N_cols, int> = 0>
    constexpr MatrixBase(T_element... vaElements) /*noexcept (see note)*/;

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

    /// \brief Sets all elements to zero
    constexpr T_derived & setZero() noexcept(should_noexcept);

    /// \brief Returns a Matrix with all elements set to 0.
    static constexpr T_derived Zero() noexcept(should_noexcept);


    constexpr Row operator[](std::size_t aRow);
    constexpr const_Row operator[](std::size_t aRow) const;


    // Iterate one line at a time, going through each column in the line
    // before descending to the next line
    constexpr const_iterator cbegin() const noexcept;
    constexpr const_iterator cend() const noexcept;
    // Also implemented to enable range for loop
    constexpr const_iterator begin() const noexcept;
    constexpr const_iterator end() const noexcept;

    constexpr T_number & at(std::size_t aIndex);
    constexpr T_number & at(std::size_t aRow, std::size_t aColumn);
    constexpr T_number at(std::size_t aIndex) const;
    constexpr T_number at(std::size_t aRow, std::size_t aColumn) const;

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

    /// \attention Can only be used in a constexpr context since C++20
    constexpr bool operator==(const MatrixBase &aRhs) const noexcept(should_noexcept);
    /// \attention Can only be used in a constexpr context since C++20
    constexpr bool operator!=(const MatrixBase &aRhs) const noexcept(should_noexcept);

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

public:
    /// \note The default default-ctor would return unitialized memory.
    /// This is usefull in implementation details, but we make it more explicit with a ctor taking a tag
    MatrixBase() = delete;

    // Implementer note: At first, this was a protected struct of Matrix template
    // yet, in some situation implementations need an un-initialized matrix of different dimensions
    // e.g. transpose operation
    class UninitializedTag
    {
        template <class, int, int, class> friend class MatrixBase;
        UninitializedTag() = default;
    };

    /// \brief Like a default constructor, but inaccessible to the client code thanks to the protected tag.
    constexpr MatrixBase(UninitializedTag) noexcept(should_noexcept);

    constexpr MatrixBase(detail::CastTag, store_type aData)
            noexcept(std::is_nothrow_move_constructible<value_type>::value);

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

template <TMP, class T_derivedRight>
constexpr additive_t<T_derived, T_derivedRight>
operator+(T_derived aLhs, const MatrixBase<TMA_RIGHT> & aRhs)
/*noexcept(T_derived::should_noexcept)*/;

template <TMP, class T_derivedRight>
constexpr additive_t<T_derived, T_derivedRight>
operator-(T_derived aLhs, const MatrixBase<TMA_RIGHT> & aRhs)
/*noexcept(T_derived::should_noexcept)*/;

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
 * Output operator
 */
template<TMP>
std::ostream & operator<<(std::ostream & os, const MatrixBase<TMA> &aMatrix);


#include "MatrixBase-impl.h"


namespace detail {
    std::false_type test_matrix_convertible(...);

    template <TMP>
    std::true_type test_matrix_convertible(const ::ad::math::MatrixBase<TMA> &);
} // namespace detail


template <class T>
class from_matrix : public decltype(detail::test_matrix_convertible(std::declval<T>()))
{};


#undef TMA_RIGHT
#undef TMA
#undef TMP


} // namespace math
} // namespace ad
