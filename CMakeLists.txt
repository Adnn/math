cmake_minimum_required(VERSION 3.18)

project(Math)

# Include cmake_helper submodule
include("cmake/include.cmake")

cmc_include_conan_configuration()

include(cmc-install)
cmc_install_root_component_config(${PROJECT_NAME})
cmc_register_source_package(${PROJECT_NAME})

add_subdirectory(src)
