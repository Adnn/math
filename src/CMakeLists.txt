add_subdirectory(libs/math/math)

option(BUILD_tests "Build the test applications" ON)
if(BUILD_tests)
    add_subdirectory(apps/tests/tests)
endif()
