namespace trans2d {


    template <class T_number, class T_angleUnitTag>
    constexpr Matrix<2, 2, T_number> rotate(const Angle<T_number, T_angleUnitTag> aAngle)
    {
        return {
             cos(aAngle), sin(aAngle),
            -sin(aAngle), cos(aAngle),
        };
    }


    template <class T_number>
    constexpr Matrix<2, 2, T_number> scale(const T_number aFactorX, const T_number aFactorY)
    {
        return {
            aFactorX,         0.,
                  0.,   aFactorY,
        };
    }


    template <class T_number>
    constexpr Matrix<2, 2, T_number> scale(const T_number aFactor, const UnitVec<2, T_number> aAxis)
    {
        // Some compact aliases for parameters
        const T_number & k = aFactor;
        const UnitVec<2, T_number> & n = aAxis;

        return {
            1 + (k-1) * std::pow(n.x(), 2),                (k-1) * n.x()*n.y(),
                       (k-1) * n.x()*n.y(),     1 + (k-1) * std::pow(n.y(), 2),
        };
    }


    template <class T_number, class T_angleRepresentation, class T_angleUnitTag>
    constexpr Matrix<2, 2, T_number> 
    scale(const T_number aFactorHorizontal, const T_number aFactorVertical, 
          const Angle<T_angleRepresentation, T_angleUnitTag> aCounterClockwise)
    {
        // Compact aliases
        auto l1 = aFactorHorizontal;
        auto l2 = aFactorVertical;

        auto cs = cos(-aCounterClockwise);
        auto sn = sin(-aCounterClockwise);
        
        auto antidiagonal = (l2 - l1) * cs * sn;

        return {
            l1 * std::pow(cs, 2) + l2 * std::pow(sn, 2),    antidiagonal,
            antidiagonal,                                   l2 * std::pow(cs, 2) + l1 * std::pow(sn, 2),
        };
    }


    template <class T_number>
    constexpr Matrix<2, 2, T_number> projectOrthographicOntoX()
    {
        return scale(T_number{1}, T_number{0});
    };


    template <class T_number>
    constexpr Matrix<2, 2, T_number> projectOrthographicOntoY()
    {
        return scale(T_number{0}, T_number{1});
    };


    template <class T_number>
    constexpr Matrix<2, 2, T_number> projectOrthographicAlong(const UnitVec<2, T_number> aAxis)
    {
        return scale(T_number{0}, aAxis);
    }


    template <class T_number>
    constexpr Matrix<2, 2, T_number> reflectAlongX()
    {
        return scale(T_number{-1}, T_number{1});
    }


    template <class T_number>
    constexpr Matrix<2, 2, T_number> reflectAlongY()
    {
        return scale(T_number{1}, T_number{-1});
    }


    template <class T_number>
    constexpr Matrix<2, 2, T_number> reflectAlong(const UnitVec<2, T_number> aAxis)
    {
        return scale(T_number{-1}, aAxis);
    }


    template <class T_number>
    constexpr Matrix<2, 2, T_number> shearX(const T_number aWeightY)
    {
        return {
            T_number{1},    T_number{0},
               aWeightY,    T_number{1},
        };
    }


    template <class T_number>
    constexpr Matrix<2, 2, T_number> shearY(const T_number aWeightX)
    {
        return {
            T_number{1},       aWeightX,
            T_number{0},    T_number{1},
        };
    }


    template <class T_number, class T_angleRepresentation, class T_angleUnitTag>
    constexpr Matrix<2, 2, T_number> 
    shearVertical(const Angle<T_angleRepresentation, T_angleUnitTag> aCounterClockwise)
    {
        return shearX(-tan(aCounterClockwise));
    }


    template <class T_number, class T_angleRepresentation, class T_angleUnitTag>
    constexpr Matrix<2, 2, T_number>
    shearHorizontal(const Angle<T_angleRepresentation, T_angleUnitTag> aCounterClockwise)
    {
        return shearY(tan(aCounterClockwise));
    }


} // namespace trans2d


namespace trans3d {


    template <class T_number, class T_angleUnitTag>
    constexpr Matrix<3, 3, T_number> rotateX(const Angle<T_number, T_angleUnitTag> aAngle)
    {
        return {
            1.,           0.,            0.,
            0.,  cos(aAngle),   sin(aAngle),
            0., -sin(aAngle),   cos(aAngle),
        };
    }


    template <class T_number, class T_angleUnitTag>
    constexpr Matrix<3, 3, T_number> rotateY(const Angle<T_number, T_angleUnitTag> aAngle)
    {
        return {
            cos(aAngle),    0., -sin(aAngle),
                     0.,    1.,           0.,
            sin(aAngle),    0.,  cos(aAngle),
        };
    }


    template <class T_number, class T_angleUnitTag>
    constexpr Matrix<3, 3, T_number> rotateZ(const Angle<T_number, T_angleUnitTag> aAngle)
    {
        return {
             cos(aAngle),   sin(aAngle),    0.,
            -sin(aAngle),   cos(aAngle),    0.,
                      0.,            0.,    1.,
        };
    }


    template <class T_number, class T_angleUnitTag>
    constexpr Matrix<3, 3, T_number> rotate(const Angle<T_number, T_angleUnitTag> aAngle,
                                            const UnitVec<3, T_number> aAxis)
    {
        // Some compact aliases for parameters
        const Angle<T_number, T_angleUnitTag> & O = aAngle;
        const UnitVec<3, T_number> & n = aAxis;

        return {
            std::pow(n.x(), 2) * (1-cos(O)) + cos(O),    n.x()*n.y() * (1-cos(O)) + n.z()*sin(O),    n.x()*n.z() * (1-cos(O)) - n.y()*sin(O),
             n.x()*n.y() * (1-cos(O)) - n.z()*sin(O),   std::pow(n.y(), 2) * (1-cos(O)) + cos(O),    n.y()*n.z() * (1-cos(O)) + n.x()*sin(O),
             n.x()*n.z() * (1-cos(O)) + n.y()*sin(O),    n.y()*n.z() * (1-cos(O)) - n.x()*sin(O),   std::pow(n.z(), 2) * (1-cos(O)) + cos(O),
        };
    }


    template <class T_number>
    constexpr Matrix<3, 3, T_number> scale(const T_number aFactorX,
                                           const T_number aFactorY,
                                           const T_number aFactorZ)
    {
        return {
            aFactorX,         0.,         0.,
                  0.,   aFactorY,         0.,
                  0.,         0.,   aFactorZ,
        };

    }


    template <class T_number>
    constexpr Matrix<3, 3, T_number> scale(const T_number aFactor, const UnitVec<3, T_number> aAxis)
    {
        // Some compact aliases for parameters
        const T_number & k = aFactor;
        const UnitVec<3, T_number> & n = aAxis;

        return {
            1 + (k-1) * std::pow(n.x(), 2),                (k-1) * n.x()*n.y(),                (k-1) * n.x()*n.z(),
                       (k-1) * n.x()*n.y(),     1 + (k-1) * std::pow(n.y(), 2),                (k-1) * n.y()*n.z(),
                       (k-1) * n.x()*n.z(),                (k-1) * n.y()*n.z(),     1 + (k-1) * std::pow(n.z(), 2),
        };
    }


    template <class T_number>
    constexpr Matrix<3, 3, T_number> projectOrthographicOntoXY()
    {
        return scale(T_number{1}, T_number{1}, T_number{0});
    }


    template <class T_number>
    constexpr Matrix<3, 3, T_number> projectOrthographicOntoXZ()
    {
        return scale(T_number{1}, T_number{0}, T_number{1});
    }


    template <class T_number>
    constexpr Matrix<3, 3, T_number> projectOrthographicOntoYZ()
    {
        return scale(T_number{0}, T_number{1}, T_number{1});
    }


    template <class T_number>
    constexpr Matrix<3, 3, T_number> projectOrthographicAlong(const UnitVec<3, T_number> aAxis)
    {
        return scale(T_number{0}, aAxis);
    }


    template <class T_number>
    constexpr Matrix<3, 3, T_number> reflectAlongX()
    {
        return scale(T_number{-1}, T_number{1}, T_number{1});
    }


    template <class T_number>
    constexpr Matrix<3, 3, T_number> reflectAlongY()
    {
        return scale(T_number{1}, T_number{-1}, T_number{1});
    }


    template <class T_number>
    constexpr Matrix<3, 3, T_number> reflectAlongZ()
    {
        return scale(T_number{1}, T_number{1}, T_number{-1});
    }


    template <class T_number>
    constexpr Matrix<3, 3, T_number> reflectAlong(const UnitVec<3, T_number> aAxis)
    {
        return scale(T_number{-1}, aAxis);
    }


    template <class T_number>
    constexpr Matrix<3, 3, T_number> shearXY(const T_number aWeightZonX,
                                             const T_number aWeightZonY)
    {
        return {
            T_number{1},    T_number{0},    T_number{0},
            T_number{0},    T_number{1},    T_number{0},
            aWeightZonX,    aWeightZonY,    T_number{1},
        };
    }


    template <class T_number>
    constexpr Matrix<3, 3, T_number> shearXZ(const T_number aWeightYonX,
                                             const T_number aWeightYonZ)
    {
        return {
            T_number{1},    T_number{0},    T_number{0},
            aWeightYonX,    T_number{1},    aWeightYonZ,
            T_number{0},    T_number{0},    T_number{1},
        };
    }


    template <class T_number>
    constexpr Matrix<3, 3, T_number> shearYZ(const T_number aWeightXonY,
                                             const T_number aWeightXonZ)
    {
        return {
            T_number{1},    aWeightXonY,    aWeightXonZ,
            T_number{0},    T_number{1},    T_number{0},
            T_number{0},    T_number{0},    T_number{1},
        };
    }


} // namespace trans3d
