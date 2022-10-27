set(TARGET_NAME math)

set(${TARGET_NAME}_HEADERS
    Angle.h
    Barycentric.h
    Base.h
    Box.h
    Canonical.h
    Clamped.h
    Color.h
    commons.h
    Constants.h
    Homogeneous.h
    Homogeneous-impl.h
    LinearMatrix.h
    Matrix.h
    Matrix-impl.h
    MatrixBase.h
    MatrixBase-impl.h
    MatrixTraits.h
    Quaternion.h
    Quaternion-impl.h
    Range.h
    Rectangle.h
    StructuredBindings.h
    Transformations.h
    Transformations-impl.h
    Utilities.h
    Vector.h
    Vector-impl.h
    VectorUtilities.h

    Curves/Bezier.h
    Curves/CardinalCubic.h
    Curves/CurveBase.h

    Interpolation/Interpolation.h
    Interpolation/QuaternionInterpolation.h
    Interpolation/ParameterAnimation.h
)

set(${TARGET_NAME}_DOC
    Interpolation/ParameterAnimation.md
)

source_group(TREE ${CMAKE_CURRENT_LIST_DIR}
             FILES ${${TARGET_NAME}_HEADERS}
                   ${${TARGET_NAME}_DOC}
)

add_library(${TARGET_NAME} INTERFACE)

# Custom target to have the files show up in the IDE
add_custom_target(${TARGET_NAME}_IDE
    SOURCES
        ${${TARGET_NAME}_HEADERS}
        ${${TARGET_NAME}_DOC}
)

add_library(ad::${TARGET_NAME} ALIAS ${TARGET_NAME})

cmc_target_current_include_directory(${TARGET_NAME})

# Some constexpr functions rely on the current CPP version to select their implementation
# and MSVC requires a compiler option to define _cplusplus macro.
if(MSVC)
    target_compile_options(${TARGET_NAME} INTERFACE "/Zc:__cplusplus")
endif()

# Cannot enable warning as error on interface targets (as they do not compile).

cmc_cpp_sanitizer(${TARGET_NAME} ${BUILD_CONF_Sanitizer})


##
## Install
##
install(TARGETS ${TARGET_NAME} EXPORT ${TARGET_NAME}Targets)

include(cmc-install)
# Install the header files
cmc_install_with_folders(DESTINATION include/${TARGET_NAME}/${TARGET_NAME}
                         FILES ${${TARGET_NAME}_HEADERS})
# Setup CMake package in both build and install trees
cmc_install_packageconfig(${TARGET_NAME} ${TARGET_NAME}Targets ${PROJECT_NAME}
                          NAMESPACE ad::)
