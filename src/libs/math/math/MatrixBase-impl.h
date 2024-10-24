#include "Utilities.h"

#include <cmath>


namespace ad {
namespace math {


template<TMP>
template <class... T_element,
          std::enable_if_t<sizeof...(T_element) == N_rows*N_cols && (N_rows*N_cols > 1), int>>
constexpr MatrixBase<TMA>::MatrixBase(T_element... vaElements) :
        mStore{ {vaElements...} }
{}


template<TMP>
constexpr MatrixBase<TMA>::MatrixBase(T_number aSingleElement) requires (N_rows*N_cols == 1) :
    mStore{ {aSingleElement} }
{}


template<TMP>
constexpr MatrixBase<TMA>::MatrixBase(UninitializedTag) noexcept(should_noexcept) :
    mStore{}
{}


template<TMP>
constexpr MatrixBase<TMA>::MatrixBase(detail::CastTag, store_type aData)
noexcept(std::is_nothrow_move_constructible<value_type>::value) :
    mStore{std::move(aData)}
{}


template<TMP>
template <class T_otherDerived,
          class /* default template argument used to enable_if */>
constexpr MatrixBase<TMA>::operator T_otherDerived () const noexcept(should_noexcept)
{
    T_otherDerived result(detail::CastTag{}, mStore);
    return result;
}


template<TMP>
template <class T_otherDerived, class T_otherNumber,
          class /* default template argument used to enable_if */>
constexpr MatrixBase<TMA>::MatrixBase(const MatrixBase<T_otherDerived,
                                                       N_rows,
                                                       N_cols,
                                                       T_otherNumber> & aOther)
noexcept(should_noexcept) :
    mStore{}
{
    // Disable the "possible loss of data" warning because this is an *explicit* ctor
    // only available for explicit conversions
#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4244)
#endif
    std::copy(aOther.begin(), aOther.end(), mStore.begin());
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
}


template<TMP>
constexpr T_derived & MatrixBase<TMA>::setZero() noexcept(should_noexcept)
{
#if __cplusplus > 201703L
    // std::array::fill() is constexpr since C++20
    // see: https://en.cppreference.com/w/cpp/container/array/fill
    mStore.fill(value_type{0});
#else
    // Implementer's note: the API only offers const_iterators
    for(std::size_t id = 0; id != size_value; ++id)
    {
        at(id) = value_type{0};
    }
#endif
    return *derivedThis();
}


template<TMP>
constexpr T_derived MatrixBase<TMA>::Zero() noexcept(should_noexcept)
{
    T_derived result(UninitializedTag{});
    return result.setZero();
}


template <TMP>
constexpr typename MatrixBase<TMA>::Row MatrixBase<TMA>::operator[](std::size_t aRow)
{
    return Row{&mStore[aRow*N_cols]};
}

template <TMP>
constexpr typename MatrixBase<TMA>::const_Row MatrixBase<TMA>::operator[](std::size_t aRow) const
{
    return const_Row{&mStore[aRow*N_cols]};
}


template <TMP>
constexpr typename MatrixBase<TMA>::const_iterator MatrixBase<TMA>::cbegin() const noexcept
{
    return mStore.cbegin();
}

template <TMP>
constexpr typename MatrixBase<TMA>::const_iterator MatrixBase<TMA>::cend() const noexcept
{
    return mStore.cend();
}

template <TMP>
constexpr typename MatrixBase<TMA>::iterator MatrixBase<TMA>::begin() noexcept
{
    return mStore.begin();
}

template <TMP>
constexpr typename MatrixBase<TMA>::iterator MatrixBase<TMA>::end() noexcept
{
    return mStore.end();
}


template <TMP>
constexpr typename MatrixBase<TMA>::const_iterator MatrixBase<TMA>::begin() const noexcept
{
    return cbegin();
}

template <TMP>
constexpr typename MatrixBase<TMA>::const_iterator MatrixBase<TMA>::end() const noexcept
{
    return cend();
}


#define GET_EXTREME_ELEMENT_IMPL(algorithm)             \
    return algorithm(begin(), end(),                    \
            [](const T_number & a, const T_number & b)  \
            {                                           \
                return std::abs(a) < std::abs(b);       \
            });


template <TMP>
constexpr typename MatrixBase<TMA>::iterator
MatrixBase<TMA>::getMinMagnitudeElement() noexcept(should_noexcept)
{
    GET_EXTREME_ELEMENT_IMPL(std::min_element);
}


template <TMP>
constexpr typename MatrixBase<TMA>::const_iterator
MatrixBase<TMA>::getMinMagnitudeElement() const noexcept(should_noexcept)
{
    GET_EXTREME_ELEMENT_IMPL(std::min_element);
}


template <TMP>
constexpr typename MatrixBase<TMA>::iterator
MatrixBase<TMA>::getMaxMagnitudeElement() noexcept(should_noexcept)
{
    GET_EXTREME_ELEMENT_IMPL(std::max_element);
}


template <TMP>
constexpr typename MatrixBase<TMA>::const_iterator
MatrixBase<TMA>::getMaxMagnitudeElement() const noexcept(should_noexcept)
{
    GET_EXTREME_ELEMENT_IMPL(std::max_element);
}


# undef GET_EXTREME_ELEMENT_IMPL


template <TMP>
constexpr T_derived * MatrixBase<TMA>::derivedThis() noexcept
{
    return static_cast<T_derived*>(this);
}


template <TMP>
constexpr const T_derived * MatrixBase<TMA>::derivedThis() const noexcept
{
    return static_cast<const T_derived*>(this);
}


template <TMP>
constexpr T_number & MatrixBase<TMA>::at(std::size_t aIndex)
{
    return mStore[aIndex];
}


template <TMP>
constexpr T_number & MatrixBase<TMA>::at(std::size_t aRow, std::size_t aColumn)
{
    return mStore[aRow*N_cols + aColumn];
}


template <TMP>
constexpr T_number MatrixBase<TMA>::at(std::size_t aIndex) const
{
    return mStore[aIndex];
}


template <TMP>
constexpr T_number MatrixBase<TMA>::at(std::size_t aRow, std::size_t aColumn) const
{
    return mStore[aRow*N_cols + aColumn];
}


template <TMP>
constexpr T_number * MatrixBase<TMA>::data() noexcept
{
    return mStore.data();
}


template <TMP>
constexpr const T_number * MatrixBase<TMA>::data() const noexcept
{
    return mStore.data();
}


template <TMP>
template <class T_derivedRight>
constexpr additive_t<T_derived, T_derivedRight> &
MatrixBase<TMA>::operator+=(const MatrixBase<TMA_RIGHT> &aRhs) noexcept(should_noexcept)
{
    for(std::size_t elementId = 0; elementId != N_rows*N_cols; ++elementId)
    {
        mStore[elementId] += aRhs.at(elementId);
    }

    return *derivedThis();
}


template <TMP>
template <class T_derivedRight>
constexpr additive_t<T_derived, T_derivedRight> &
MatrixBase<TMA>::operator-=(const MatrixBase<TMA_RIGHT> &aRhs) noexcept(should_noexcept)
{
    for(std::size_t elementId = 0; elementId != N_rows*N_cols; ++elementId)
    {
        mStore[elementId] -= aRhs.at(elementId);
    }

    return *derivedThis();
}


template <TMP>
template <class T_scalar>
constexpr std::enable_if_t<! from_matrix_v<T_scalar>, T_derived &>
MatrixBase<TMA>::operator*=(T_scalar aScalar) noexcept(should_noexcept)
{
    for(std::size_t elementId = 0; elementId != N_rows*N_cols; ++elementId)
    {
        // TODO Ad 2022/11/15: Should it be transformed to (T_value) element * aScalar?
        // Currently, this prevents applying a floating point factor to an integral type (with warning as error).
        // Yet, I am also worried that it could silence legitimate errors (e.g. multiplying a sdr color by a non-normalized float).
        mStore[elementId] *= aScalar;
    }
    return *derivedThis();
}


template <TMP, class T_scalar>
constexpr std::enable_if_t<! from_matrix_v<T_scalar>, T_derived>
operator*(T_scalar aScalar, const MatrixBase<TMA> &aRhs)
{
    T_derived copy(static_cast<const T_derived &>(aRhs));
    copy *= aScalar;
    return copy;
}


template <TMP, class T_scalar>
constexpr std::enable_if_t<! from_matrix_v<T_scalar>, T_derived>
operator*(const MatrixBase<TMA> &aLhs, T_scalar aScalar)
{
    return aScalar * aLhs;
}


template <TMP>
template <class T_scalar>
constexpr std::enable_if_t<! from_matrix_v<T_scalar>, T_derived &>
MatrixBase<TMA>::operator/=(T_scalar aScalar) noexcept(should_noexcept)
{
    for(std::size_t elementId = 0; elementId != N_rows*N_cols; ++elementId)
    {
        mStore[elementId] /= aScalar;
    }
    return *derivedThis();
}


template <TMP, class T_scalar>
constexpr std::enable_if_t<! from_matrix_v<T_scalar>, T_derived>
operator/(const MatrixBase<TMA> &aLhs, T_scalar aScalar)
{
    T_derived copy(static_cast<const T_derived &>(aLhs));
    copy /= aScalar;
    return copy;
}


template <TMP>
constexpr T_derived MatrixBase<TMA>::operator-() const noexcept(should_noexcept)
{
    T_derived copy(*derivedThis());
    copy *= -1;
    return copy;
}


template <TMP>
constexpr T_derived & MatrixBase<TMA>::cwMulAssign(const MatrixBase &aRhs) noexcept(should_noexcept)
{
    for(std::size_t elementId = 0; elementId != N_rows*N_cols; ++elementId)
    {
        mStore[elementId] *= aRhs.mStore[elementId];
    }
    return *this->derivedThis();
}


template <TMP>
constexpr T_derived MatrixBase<TMA>::cwMul(T_derived aRhs) const noexcept(should_noexcept)
{
    return aRhs.cwMulAssign(*this);
}


template <TMP>
constexpr T_derived & MatrixBase<TMA>::cwDivAssign(const MatrixBase &aRhs) noexcept(should_noexcept)
{
    for(std::size_t elementId = 0; elementId != N_rows*N_cols; ++elementId)
    {
        mStore[elementId] /= aRhs.mStore[elementId];
    }
    return *this->derivedThis();
}


template <TMP>
constexpr T_derived MatrixBase<TMA>::cwDiv(const T_derived &aRhs) const noexcept(should_noexcept)
{
    T_derived left(*derivedThis()); // Avoid conversion op by using *derivedThis instead of *this
    return left.cwDivAssign(aRhs);
}


template <TMP>
constexpr T_derived & MatrixBase<TMA>::cwModAssign(const MatrixBase &aRhs) noexcept(should_noexcept)
{
    for(std::size_t elementId = 0; elementId != N_rows*N_cols; ++elementId)
    {
        mStore[elementId] %= aRhs.mStore[elementId];
    }
    return *this->derivedThis();
}


template <TMP>
constexpr T_derived MatrixBase<TMA>::cwMod(const T_derived &aRhs) const noexcept(should_noexcept)
{
    T_derived left(*derivedThis()); // Avoid conversion op by using *derivedThis instead of *this
    return left.cwModAssign(aRhs);
}


template<TMP>
constexpr bool MatrixBase<TMA>::equalsWithinTolerance(const MatrixBase & aRhs, T_number aEpsilon) const noexcept(should_noexcept)
{
    return std::equal(begin(), end(),
                      aRhs.begin(), aRhs.end(),
                      [aEpsilon](const T_number & a, const T_number & b)
                      {
                           return absoluteTolerance(a, b, aEpsilon);
                      });
}


template<TMP>
template <class T_extracted, std::size_t... VN_indices>
T_extracted MatrixBase<TMA>::extract(std::index_sequence<VN_indices...> aIndices) const 
noexcept(should_noexcept)
{
    return T_extracted{at(VN_indices)...};
}


/*
 * Component-wise operations
 */
namespace {

    template <std::size_t... VN_indices, TMP>
    T_derived binaryOp(const MatrixBase<TMA> & aLhs,
                       const MatrixBase<TMA> & aRhs, 
                       const T_number &(* aOperation)(const T_number &, const T_number &),
                       std::index_sequence<VN_indices...>)
    {
        return {aOperation(aLhs.at(VN_indices), aRhs.at(VN_indices))...}; 
    }

    template <std::size_t... VN_indices, TMP>
    T_derived unaryOp(const MatrixBase<TMA> & aMatrix,
                      const T_number &(* aOperation)(const T_number &),
                      std::index_sequence<VN_indices...>)
    {
        return {aOperation(aMatrix.at(VN_indices))...}; 
    }

    template <std::size_t... VN_indices, TMP>
    T_derived unaryOp(const MatrixBase<TMA> & aMatrix,
                      T_number(* aOperation)(T_number),
                      std::index_sequence<VN_indices...>)
    {
        return {aOperation(aMatrix.at(VN_indices))...}; 
    }

} // anonymous namespace


template <TMP>
T_derived min(const MatrixBase<TMA> & aLhs, const MatrixBase<TMA> & aRhs) 
noexcept(T_derived::should_noexcept)
{
   return binaryOp(aLhs, aRhs, std::min, std::make_index_sequence<N_rows * N_cols>());
}


template <TMP>
T_derived max(const MatrixBase<TMA> & aLhs, const MatrixBase<TMA> & aRhs) 
noexcept(T_derived::should_noexcept)
{
   return binaryOp(aLhs, aRhs, std::max, std::make_index_sequence<N_rows * N_cols>());
}


template <TMP>
T_derived abs(const MatrixBase<TMA> & aMatrix)
noexcept(T_derived::should_noexcept)
{
    return unaryOp(aMatrix, std::abs, std::make_index_sequence<N_rows * N_cols>());
}


template <TMP>
T_derived floor(const MatrixBase<TMA> & aMatrix)
noexcept(T_derived::should_noexcept)
{
    return unaryOp(aMatrix,
                   static_cast<T_number(*)(T_number)>(std::floor),
                   std::make_index_sequence<N_rows * N_cols>());
}


template <TMP>
T_derived ceil(const MatrixBase<TMA> & aMatrix)
noexcept(T_derived::should_noexcept)
{
    return unaryOp(aMatrix,
                   static_cast<T_number(*)(T_number)>(std::ceil),
                   std::make_index_sequence<N_rows * N_cols>());
}


template <TMP>
T_derived round(const MatrixBase<TMA> & aMatrix)
noexcept(T_derived::should_noexcept)
{
    return unaryOp(aMatrix,
                   static_cast<T_number(*)(T_number)>(std::round),
                   std::make_index_sequence<N_rows * N_cols>());
}


/*
 * Output operator
 */
template<TMP>
std::ostream & operator<<(std::ostream & os, const MatrixBase<TMA> &aMatrix)
{
    auto printRow = [&](std::size_t row)
    {
        os << "| ";
        for(std::size_t col = 0; col != N_cols; ++col)
        {
            os << aMatrix[row][col] << ' ';
        }
        os << '|';
    };

    printRow(0);
    for(std::size_t row = 1; row != N_rows; ++row)
    {
        os << '\n';
        printRow(row);
    }

    return os;
}


}} // namespace ad::math
