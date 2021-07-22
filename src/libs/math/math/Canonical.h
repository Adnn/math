#pragma once

#include "Angle.h"


namespace ad {
namespace math{


///  \brief Wrapper allowing to use the system type to mark that a value is in its canonical form.
/// 
/// The canonical form is notably useful when rotations are involved (angles, polar coordinates, Euler angles).
template <class T>
class Canonical;


/// \brief Partial specialization for Angle.
template <class T_representation, class T_unitTag>
class Canonical<Angle<T_representation, T_unitTag>>
{
	using wrapped_type = Angle<T_representation, T_unitTag>;

public:
	explicit constexpr Canonical(wrapped_type aAngle);

	/*implicit*/ constexpr operator wrapped_type() const;

private:
	wrapped_type mAngle;
};


template <class T_representation, class T_unitTag>
constexpr Canonical<Angle<T_representation, T_unitTag>>::Canonical(wrapped_type aAngle) :
	mAngle{reduce(aAngle)}
{}


template <class T_representation, class T_unitTag>
constexpr Canonical<Angle<T_representation, T_unitTag>>::operator wrapped_type() const
{
	return mAngle;
}


}} // namespace ad::math