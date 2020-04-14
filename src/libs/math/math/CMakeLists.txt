project(math)

set(${PROJECT_NAME}_HEADERS
    Angle.h
    Barycentric.h
    Color.h
    commons.h
    Constants.h
    Matrix.h
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
)

add_library(${PROJECT_NAME} INTERFACE)

# Custom target to have the files show up in the IDE
add_custom_target(${PROJECT_NAME}_IDE
    SOURCES
        ${${PROJECT_NAME}_HEADERS}
)

add_library(ad::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
        $<INSTALL_INTERFACE:include/>
)


##
## Install
##
install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Targets)

include(cmc-install)
# Install the header files
cmc_install_with_folders(DESTINATION include/${PROJECT_NAME}
                         FILES ${${PROJECT_NAME}_HEADERS})
# Setup CMake package in both build and install trees
cmc_install_packageconfig(${PROJECT_NAME} ${PROJECT_NAME}Targets
                          NAMESPACE ad::)
