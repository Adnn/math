string(TOLOWER ${PROJECT_NAME} _lower_project_name)
set(TARGET_NAME ${_lower_project_name}_tests)

set(${TARGET_NAME}_HEADERS
    catch.hpp
    detection.h
    operation_detectors.h
)

set(${TARGET_NAME}_SOURCES
    Angle.cpp
    Barycentric.cpp
    Base.cpp
    Bezier_tests.cpp
    Box.cpp
    Canonical_tests.cpp
    CardinalCubic_tests.cpp
    Color_tests.cpp
    Constexpr_tests.cpp
    Homogeneous_tests.cpp
    Interpolation_tests.cpp
    Matrix.cpp
    Noexcept_tests.cpp
    Polynomial.cpp
    Range.cpp
    Rectangle.cpp
    Traits.cpp
    Transformations_tests.cpp
    TransformNormal_tests.cpp
    Utilities_tests.cpp
    Vector.cpp
    VectorOfAngle.cpp
)

add_executable(${TARGET_NAME}
    main.cpp
    ${${TARGET_NAME}_SOURCES}
    ${${TARGET_NAME}_HEADERS}
)


target_link_libraries(${TARGET_NAME} PRIVATE
    ad::math
)

include(cmc-cpp)
cmc_cpp_all_warnings_as_errors(${TARGET_NAME})


##
## Install
##
install(TARGETS ${TARGET_NAME} RUNTIME)
