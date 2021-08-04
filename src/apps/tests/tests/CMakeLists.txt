string(TOLOWER ${PROJECT_NAME} _lower_project_name)
project(${_lower_project_name}_tests)

set(${PROJECT_NAME}_HEADERS
    catch.hpp
    detection.h
    operation_detectors.h
)

set(${PROJECT_NAME}_SOURCES
    Angle.cpp
    Barycentric.cpp
    Base.cpp
    Canonical_tests.cpp
    Color_tests.cpp
    Constexpr_tests.cpp
    Homogeneous_tests.cpp
    Matrix.cpp
    Noexcept_tests.cpp
    Polynomial.cpp
    Range.cpp
    Rectangle.cpp
    Traits.cpp
    Transformations_tests.cpp
    TransformNormal_tests.cpp
    Vector.cpp
    VectorOfAngle.cpp
)

add_executable(${PROJECT_NAME}
    main.cpp
    ${${PROJECT_NAME}_SOURCES}
    ${${PROJECT_NAME}_HEADERS}
)


target_link_libraries(${PROJECT_NAME} PRIVATE
    ad::math
)

include(cmc-cpp)
cmc_cpp_all_warnings_as_errors(${PROJECT_NAME})


##
## Install
##
install(TARGETS ${PROJECT_NAME} RUNTIME)
