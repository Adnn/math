set(TARGET_NAME math)

set(${TARGET_NAME}_HEADERS
    Angle.h
    Barycentric.h
    Base.h
    Box.h
    Canonical.h
    Color.h
    commons.h
    Constants.h
    Homogeneous.h
    Homogeneous-impl.h
    Interpolation.h
    Matrix.h
    Matrix-impl.h
    MatrixBase.h
    MatrixBase-impl.h
    MatrixTraits.h
    Range.h
    Rectangle.h
    Transformations.h
    Transformations-impl.h
    Utilities.h
    Vector.h
    Vector-impl.h
    VectorUtilities.h

    Curves/Bezier.h
    Curves/CardinalCubic.h
    Curves/CurveBase.h
)

source_group(TREE ${CMAKE_CURRENT_LIST_DIR}
             FILES ${${TARGET_NAME}_HEADERS}
)

add_library(${TARGET_NAME} INTERFACE)

# Custom target to have the files show up in the IDE
add_custom_target(${TARGET_NAME}_IDE
    SOURCES
        ${${TARGET_NAME}_HEADERS}
)

add_library(ad::${TARGET_NAME} ALIAS ${TARGET_NAME})

cmc_target_current_include_directory(${TARGET_NAME})

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
