namespace ad {
namespace math {


namespace trans2d {


    template <class T_number, class T_angleUnitTag>
    constexpr LinearMatrix<2, 2, T_number> rotate(const Angle<T_number, T_angleUnitTag> aAngle)
    {
        return {
             cos(aAngle), sin(aAngle),
            -sin(aAngle), cos(aAngle),
        };
    }


    template <class T_number, class T_angleUnitTag>
    constexpr AffineMatrix<3, T_number> rotateAbout(const Angle<T_number, T_angleUnitTag> aAngle,
                                                    const Position<2, T_number> aRotationCenter)
    {
        return translate(-aRotationCenter.template as<Vec>()) 
            * rotate(aAngle) 
            * translate(aRotationCenter.template as<Vec>());
    }


    template <class T_number>
    constexpr LinearMatrix<2, 2, T_number> scale(const T_number aFactorX, const T_number aFactorY)
    {
        return {
            aFactorX,       T_number{0},
            T_number{0},    aFactorY,
        };
    }


    template <class T_number>
    constexpr LinearMatrix<2, 2, T_number> scale(Size<2, T_number> aFactors)
    {
        return scale(aFactors[0], aFactors[1]);
    }


    template <class T_number>
    constexpr LinearMatrix<2, 2, T_number> scale(const T_number aFactor, const UnitVec<2, T_number> aAxis)
    {
        // Some compact aliases for parameters
        const T_number & k = aFactor;
        const UnitVec<2, T_number> & n = aAxis;

        return {
            1 + (k-1) * std::pow(n.x(), 2),     (k-1) * n.x()*n.y(),
            (k-1) * n.x()*n.y(),                1 + (k-1) * std::pow(n.y(), 2),
        };
    }


    template <class T_number, class T_angleRepresentation, class T_angleUnitTag>
    constexpr LinearMatrix<2, 2, T_number>
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
    constexpr LinearMatrix<2, 2, T_number> scaleUniform(const T_number aFactor)
    {
        return scale(aFactor, aFactor);
    }

    template <class T_number>
    constexpr LinearMatrix<2, 2, T_number> projectOrthographicOntoX()
    {
        return scale(T_number{1}, T_number{0});
    };


    template <class T_number>
    constexpr LinearMatrix<2, 2, T_number> projectOrthographicOntoY()
    {
        return scale(T_number{0}, T_number{1});
    };


    template <class T_number>
    constexpr LinearMatrix<2, 2, T_number> projectOrthographicAlong(const UnitVec<2, T_number> aAxis)
    {
        return scale(T_number{0}, aAxis);
    }


    template <class T_number>
    constexpr LinearMatrix<2, 2, T_number> reflectAlongX()
    {
        return scale(T_number{-1}, T_number{1});
    }


    template <class T_number>
    constexpr LinearMatrix<2, 2, T_number> reflectAlongY()
    {
        return scale(T_number{1}, T_number{-1});
    }


    template <class T_number>
    constexpr LinearMatrix<2, 2, T_number> reflectAlong(const UnitVec<2, T_number> aAxis)
    {
        return scale(T_number{-1}, aAxis);
    }


    template <class T_number>
    constexpr LinearMatrix<2, 2, T_number> shearX(const T_number aWeightY)
    {
        return {
            T_number{1},    T_number{0},
               aWeightY,    T_number{1},
        };
    }


    template <class T_number>
    constexpr LinearMatrix<2, 2, T_number> shearY(const T_number aWeightX)
    {
        return {
            T_number{1},       aWeightX,
            T_number{0},    T_number{1},
        };
    }


    template <class T_number, class T_angleRepresentation, class T_angleUnitTag>
    constexpr LinearMatrix<2, 2, T_number>
    shearVertical(const Angle<T_angleRepresentation, T_angleUnitTag> aCounterClockwise)
    {
        return shearX(-tan(aCounterClockwise));
    }


    template <class T_number, class T_angleRepresentation, class T_angleUnitTag>
    constexpr LinearMatrix<2, 2, T_number>
    shearHorizontal(const Angle<T_angleRepresentation, T_angleUnitTag> aCounterClockwise)
    {
        return shearY(tan(aCounterClockwise));
    }


    template <class T_number>
    constexpr AffineMatrix<3, T_number>
    translate(const Vec<2, T_number> aDisplacement)
    {
        // Note: because there is no AffineMatrix ctor directly taking the list of element
        // the construction has to use the constructor taking a Matrix.
        return AffineMatrix<3, T_number>{{
            T_number{1},      T_number{0},
            T_number{0},      T_number{1},
            aDisplacement[0], aDisplacement[1],
        }};
    }


    template <class T_number>
    constexpr AffineMatrix<3, T_number>
    window(const Rectangle<T_number> aSource, const Rectangle<T_number> aDestination)
    {
        const T_number xsl = aSource.xMin();
        const T_number xsh = aSource.xMax();
        const T_number ysl = aSource.yMin();
        const T_number ysh = aSource.yMax();

        const T_number xdl = aDestination.xMin();
        const T_number xdh = aDestination.xMax();
        const T_number ydl = aDestination.yMin();
        const T_number ydh = aDestination.yMax();

        const T_number swidth = aSource.width();
        const T_number sheight = aSource.height();

        const T_number dwidth = aDestination.width();
        const T_number dheight = aDestination.height();

        return AffineMatrix<3, T_number>{{
            dwidth/swidth,                  T_number{0},
            T_number{0},                    dheight/sheight,
            (xdl*xsh - xdh*xsl) / swidth,   (ydl*ysh - ydh*ysl) / sheight
        }};
    }


    template <class T_number>
    constexpr AffineMatrix<3, T_number>
    frameToCanonical(const Frame<2, T_number> aFrame)
    {
        /// FoCG 3rd p137
        const auto & u = aFrame.base.u();
        const auto & v = aFrame.base.v();
        const auto & e = aFrame.origin;

        return AffineMatrix<3, T_number>{{
                u.x(),  u.y(),
                v.x(),  v.y(),
                e.x(),  e.y()
        }};
    }


    template <class T_number>
    constexpr AffineMatrix<3, T_number>
    canonicalToFrame(const Frame<2, T_number> aFrame)
    {
        /// FoCG 3rd p138
        const auto & u = aFrame.base.u();
        const auto & v = aFrame.base.v();
        const auto & e = aFrame.origin;

        return 
            AffineMatrix<3, T_number>{{
                T_number{1},    T_number{0},
                T_number{0},    T_number{1},
                -e.x(),         -e.y()
            }}
            *
            AffineMatrix<3, T_number>{{
                u.x(),          v.x(),
                u.y(),          v.y(),
                T_number{0},    T_number{0}
            }}
        ;
    }


    template <class T_number>
    constexpr AffineMatrix<3, T_number>
    orthographicProjection(const Rectangle<T_number> aProjected)
    {
        return window(aProjected,
                      { { T_number{-1}, T_number{-1}},
                        { T_number{2},  T_number{2} } });
    }


} // namespace trans2d


namespace trans3d {


    template <class T_number, class T_angleUnitTag>
    constexpr LinearMatrix<3, 3, T_number> rotateX(const Angle<T_number, T_angleUnitTag> aAngle)
    {
        return {
            T_number{1.}, T_number{0.},  T_number{0.},
            T_number{0.},  cos(aAngle),   sin(aAngle),
            T_number{0.}, -sin(aAngle),   cos(aAngle),
        };
    }


    template <class T_number, class T_angleUnitTag>
    constexpr LinearMatrix<3, 3, T_number> rotateY(const Angle<T_number, T_angleUnitTag> aAngle)
    {
        return {
            cos(aAngle),    T_number{0.}, -sin(aAngle),
            T_number{0.},   T_number{1.}, T_number{0.},
            sin(aAngle),    T_number{0.},  cos(aAngle),
        };
    }


    template <class T_number, class T_angleUnitTag>
    constexpr LinearMatrix<3, 3, T_number> rotateZ(const Angle<T_number, T_angleUnitTag> aAngle)
    {
        return {
             cos(aAngle),   sin(aAngle),    T_number{0.},
            -sin(aAngle),   cos(aAngle),    T_number{0.},
            T_number{0.},  T_number{0.},    T_number{1.},
        };
    }


    template <class T_number, class T_angleUnitTag>
    constexpr LinearMatrix<3, 3, T_number> rotate(const UnitVec<3, T_number> aAxis,
                                                  const Angle<T_number, T_angleUnitTag> aAngle)
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
    constexpr LinearMatrix<3, 3, T_number> scale(const T_number aFactorX,
                                                 const T_number aFactorY,
                                                 const T_number aFactorZ)
    {
        return {
            aFactorX,       T_number{0},    T_number{0},
            T_number{0},    aFactorY,       T_number{0},
            T_number{0},    T_number{0},    aFactorZ,
        };

    }


    template <class T_number>
    constexpr LinearMatrix<3, 3, T_number> scale(Size<3, T_number> aFactors)
    {
        return scale(aFactors[0], aFactors[1], aFactors[2]);
    }


    template <class T_number>
    constexpr LinearMatrix<3, 3, T_number> scale(const T_number aFactor, const UnitVec<3, T_number> aAxis)
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
    constexpr LinearMatrix<3, 3, T_number> scaleUniform(const T_number aFactor)
    {
        return scale(aFactor, aFactor, aFactor);
    }


    template <class T_number>
    constexpr LinearMatrix<3, 3, T_number> projectOrthographicOntoXY()
    {
        return scale(T_number{1}, T_number{1}, T_number{0});
    }


    template <class T_number>
    constexpr LinearMatrix<3, 3, T_number> projectOrthographicOntoXZ()
    {
        return scale(T_number{1}, T_number{0}, T_number{1});
    }


    template <class T_number>
    constexpr LinearMatrix<3, 3, T_number> projectOrthographicOntoYZ()
    {
        return scale(T_number{0}, T_number{1}, T_number{1});
    }


    template <class T_number>
    constexpr LinearMatrix<3, 3, T_number> projectOrthographicAlong(const UnitVec<3, T_number> aAxis)
    {
        return scale(T_number{0}, aAxis);
    }


    template <class T_number>
    constexpr LinearMatrix<3, 3, T_number> reflectAlongX()
    {
        return scale(T_number{-1}, T_number{1}, T_number{1});
    }


    template <class T_number>
    constexpr LinearMatrix<3, 3, T_number> reflectAlongY()
    {
        return scale(T_number{1}, T_number{-1}, T_number{1});
    }


    template <class T_number>
    constexpr LinearMatrix<3, 3, T_number> reflectAlongZ()
    {
        return scale(T_number{1}, T_number{1}, T_number{-1});
    }


    template <class T_number>
    constexpr LinearMatrix<3, 3, T_number> reflectAlong(const UnitVec<3, T_number> aAxis)
    {
        return scale(T_number{-1}, aAxis);
    }


    template <class T_number>
    constexpr LinearMatrix<3, 3, T_number> shearXY(const T_number aWeightZonX,
                                             const T_number aWeightZonY)
    {
        return {
            T_number{1},    T_number{0},    T_number{0},
            T_number{0},    T_number{1},    T_number{0},
            aWeightZonX,    aWeightZonY,    T_number{1},
        };
    }


    template <class T_number>
    constexpr LinearMatrix<3, 3, T_number> shearXZ(const T_number aWeightYonX,
                                             const T_number aWeightYonZ)
    {
        return {
            T_number{1},    T_number{0},    T_number{0},
            aWeightYonX,    T_number{1},    aWeightYonZ,
            T_number{0},    T_number{0},    T_number{1},
        };
    }


    template <class T_number>
    constexpr LinearMatrix<3, 3, T_number> shearYZ(const T_number aWeightXonY,
                                             const T_number aWeightXonZ)
    {
        return {
            T_number{1},    aWeightXonY,    aWeightXonZ,
            T_number{0},    T_number{1},    T_number{0},
            T_number{0},    T_number{0},    T_number{1},
        };
    }


    template <class T_number>
    constexpr AffineMatrix<4, T_number>
    translate(const Vec<3, T_number> aDisplacement)
    {
        // Note: because there is no AffineMatrix ctor directly taking the list of element
        // the construction has to use the constructor taking a Matrix.
        return AffineMatrix<4, T_number>{{
            T_number{1},       T_number{0},       T_number{0},      
            T_number{0},       T_number{1},       T_number{0},
            T_number{0},       T_number{0},       T_number{1}, 
            aDisplacement[0],  aDisplacement[1],  aDisplacement[2],
        }};
    }


    template <class T_number>
    constexpr AffineMatrix<4, T_number>
    window(const Box<T_number> aSource, const Box<T_number> aDestination)
    {
        const T_number xsl = aSource.xMin();
        const T_number xsh = aSource.xMax();
        const T_number ysl = aSource.yMin();
        const T_number ysh = aSource.yMax();
        const T_number zsl = aSource.zMin();
        const T_number zsh = aSource.zMax();

        const T_number xdl = aDestination.xMin();
        const T_number xdh = aDestination.xMax();
        const T_number ydl = aDestination.yMin();
        const T_number ydh = aDestination.yMax();
        const T_number zdl = aDestination.zMin();
        const T_number zdh = aDestination.zMax();

        const T_number swidth = aSource.width();
        const T_number sheight = aSource.height();
        const T_number sdepth = aSource.depth();

        const T_number dwidth = aDestination.width();
        const T_number dheight = aDestination.height();
        const T_number ddepth = aDestination.depth();

        return AffineMatrix<4, T_number>{{
            dwidth/swidth,                  T_number{0},                    T_number{0},
            T_number{0},                    dheight/sheight,                T_number{0},
            T_number{0},                    T_number{0},                    ddepth/sdepth,
            (xdl*xsh - xdh*xsl) / swidth,   (ydl*ysh - ydh*ysl) / sheight,  (zdl*zsh - zdh*zsl) / sdepth,
        }};
    }


    template <class T_number>
    constexpr AffineMatrix<4, T_number>
    frameToCanonical(const Frame<3, T_number> aFrame)
    {
        /// FoCG 3rd p137
        const auto & u = aFrame.base.u();
        const auto & v = aFrame.base.v();
        const auto & w = aFrame.base.w();
        const auto & e = aFrame.origin;

        return AffineMatrix<4, T_number>{{
                u.x(),  u.y(),  u.z(),
                v.x(),  v.y(),  v.z(),
                w.x(),  w.y(),  w.z(),
                e.x(),  e.y(),  e.z(),
        }};
    }


    template <class T_number>
    constexpr AffineMatrix<4, T_number>
    canonicalToFrame(const Frame<3, T_number> aFrame)
    {
        /// FoCG 3rd p138
        const auto & u = aFrame.base.u();
        const auto & v = aFrame.base.v();
        const auto & w = aFrame.base.w();
        const auto & e = aFrame.origin;

        return 
            AffineMatrix<4, T_number>{{
                T_number{1},    T_number{0},    T_number{0},
                T_number{0},    T_number{1},    T_number{0},
                T_number{0},    T_number{0},    T_number{1},
                -e.x(),         -e.y(),         -e.z()
            }}
            *
            AffineMatrix<4, T_number>{{
                u.x(),          v.x(),          w.x(),
                u.y(),          v.y(),          w.y(),
                u.z(),          v.z(),          w.z(),
                T_number{0},    T_number{0},    T_number{0}
            }}
        ;
    }


    template <class T_number>
    constexpr AffineMatrix<4, T_number>
    orthographicProjection(const Box<T_number> aProjected)
    {
        return window(aProjected,
                      { { T_number{-1}, T_number{-1}, T_number{-1} },
                        { T_number{2},  T_number{2},  T_number{2}  } });
    }

    template <class T_number>
    constexpr Matrix<4, 4, T_number>
    perspective(const T_number aNearPlaneZ, const T_number aFarPlaneZ)
    {
        const T_number n = aNearPlaneZ;
        const T_number f = aFarPlaneZ;
        const T_number v0 = T_number{0};
        const T_number v1 = T_number{1};

        return {
            n,   v0,  v0,    v0,
            v0,  n,   v0,    v0,
            v0,  v0,  n+f,   v1,
            v0,  v0,  -f*n,  v0
        };
    }



    template <class T_number>
    constexpr Matrix<4, 4, T_number>
    perspectiveNegated(const T_number aNearPlaneZ, const T_number aFarPlaneZ)
    {
        // IMPORTANT: Negate the perspective matrix from p152, this way -Z is copied into W.
        // Assuming a right handed frame where camera looks along negative Z axis, 
        // this copies a positive value in W for vertices **in front** of the camera (having negative Z coordinates).
        // Thus vertices between near and far plane can pass the OpenGL clipping test -w < x, y, z < +w.
        // This does **not** change handed-ness, but negates the whole transformed vector:
        // the transformed homogeneous coordinates are strictly equivalent.
        return -1 * perspective(aNearPlaneZ, aFarPlaneZ);
    }



    template <class T_number>
    constexpr Matrix<4, 4, T_number>
    perspectiveInverse(const T_number aNearPlaneZ, const T_number aFarPlaneZ)
    {
        const T_number n = aNearPlaneZ;
        const T_number f = aFarPlaneZ;
        const T_number v0 = T_number{0};
        const T_number v1 = T_number{1};

        return {
            f,   v0,  v0,   v0,
            v0,  f,   v0,   v0,
            v0,  v0,  v0,   -v1,
            v0,  v0,  f*n,  n+f, 
        };
    }


    template <class T_number>
    constexpr AffineMatrix<4, T_number>
    ndcToViewport(const Rectangle<T_number> aViewPort, T_number aNear, T_number aFar)
    {
        assert(aNear > aFar);
        return window(
            Box<T_number>{ 
                { T_number{-1}, T_number{-1}, T_number{1} },
                { T_number{2}, T_number{2}, T_number{2} }
            },
            Box<T_number>{
                { aViewPort.origin(), aNear },
                { aViewPort.dimension(), aNear - aFar },
            });
    }

} // namespace trans3d


} // namespace math
} // namespace ad
