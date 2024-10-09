from conans import ConanFile, tools
from conan.tools.cmake import CMake, CMakeToolchain, cmake_layout


from os import path


class MathConan(ConanFile):
    """ Conan recipe for Math """
    """ Currently the library is header only, but there are compiled tests """
    """ so we keep the usual settings """

    name = "math"
    license = "MIT"
    url = "https://github.com/Adnn/math"
    description = "A mathematic library implementation, first concentrating on linear algrebra."
    topics = ("math", "linear algebra", "geometry", "graphics",)
    settings = ("os", "compiler", "build_type", "arch")
    options = {
        "shared": [True, False],
        "build_tests": [True, False],
    }
    default_options = {
        "shared": False,
        "build_tests": False,
    }

    requires = (
        ("reflexion/b2aa059210@adnn/develop"),
    )

    build_policy = "missing"
    generators = "CMakeDeps", "CMakeToolchain"


    python_requires="shred_conan_base/0.0.5@adnn/stable"
    python_requires_extend="shred_conan_base.ShredBaseConanFile"
