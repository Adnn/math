cmake_minimum_required(VERSION 3.16)

project(Math)

# Include cmake_helper submodule
include("cmake/include.cmake")

cmc_include_conan_configuration()

include(cmc-install)
cmc_install_root_component_config()

add_subdirectory(src)
