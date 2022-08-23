cmake_minimum_required(VERSION 3.20)

project(Math)

# Include cmake_helper submodule
include("cmake/include.cmake")

cmc_include_conan_configuration()

# Enable the grouping target in folders, when available in IDE.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(cmc-install)
cmc_install_root_component_config(${PROJECT_NAME})
cmc_register_source_package(${PROJECT_NAME})

# Export compile commands by default
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

add_subdirectory(src)
