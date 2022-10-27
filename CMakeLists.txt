cmake_minimum_required(VERSION 3.23)

project(Math VERSION "${BUILD_VERSION}")

# Include cmake_helper submodule
include("cmake/include.cmake")

cmc_include_conan_configuration()

# Enable the grouping target in folders, when available in IDE.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Note: also set by conanuser_config.cmake
option (BUILD_CONF_WarningAsError "Enable warning as error compiler option." False)

include(cmc-install)
cmc_install_root_component_config(${PROJECT_NAME})
cmc_register_source_package(${PROJECT_NAME})

include(cmc-cpp)
cmc_cpp_define_sanitizer_enum()
add_subdirectory(src)
