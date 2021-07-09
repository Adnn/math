from conans import ConanFile, CMake, tools

from os import path


class MathConan(ConanFile):
    """ Conan recipe for Math """
    """ Currently the library is header only, but there are compiled tests """
    """ so we keep the usual settings """

    name = "math"
    license = "MIT"
    url = "https://github.com/Adnn/math"
    description = "A mathematic library implementation, first concentrating on linear algrebra"
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

    build_requires = ("cmake/3.20.4",)

    build_policy = "missing"
    generators = "cmake_paths", "cmake"

    scm = {
        "type": "git",
        "url": "auto",
        "revision": "auto",
        "submodule": "recursive",
    }


    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.definitions["CMAKE_PROJECT_Math_INCLUDE"] = \
            path.join(self.source_folder, "cmake", "conan", "customconan.cmake")
        cmake.definitions["BUILD_tests"] = self.options.build_tests
        cmake.configure()
        return cmake


    def configure(self):
        tools.check_min_cppstd(self, "17")


    def build(self):
        cmake = self._configure_cmake()
        cmake.build()


    def package(self):
        cmake = self._configure_cmake()
        cmake.install()


    def package_info(self):
        pass
