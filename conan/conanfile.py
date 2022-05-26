from conans import ConanFile, tools
from conan.tools.cmake import CMake


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

    build_policy = "missing"
    generators = "CMakeDeps", "CMakeToolchain"

    scm = {
        "type": "git",
        "url": "auto",
        "revision": "auto",
        "submodule": "recursive",
    }

    python_requires="shred_conan_base/0.0.1@adnn/develop"
    python_requires_extend="shred_conan_base.ShredBaseConanFile"


    def _generate_cmake_configfile(self):
        """ Generates a conanuser_config.cmake file which includes the file generated by """
        """ cmake_paths generator, and forward the remaining options to CMake. """
        with open("conanuser_config.cmake", "w") as config:
            config.write("message(STATUS \"Including user generated conan config.\")\n")
            # avoid path.join, on Windows it outputs '\', which is a string escape sequence.
            #config.write("include(\"{}\")\n".format("${CMAKE_CURRENT_LIST_DIR}/conan_paths.cmake"))
            config.write("set({} {})\n".format("BUILD_tests", self.options.build_tests))
            config.write("set(CMAKE_EXPORT_COMPILE_COMMANDS 1)\n")


    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.configure()
        return cmake


    def configure(self):
        tools.check_min_cppstd(self, "17")


    def generate(self):
        self._generate_cmake_configfile()


    def build(self):
        cmake = self._configure_cmake()
        cmake.build()


    def package(self):
        cmake = self._configure_cmake()
        cmake.install()

    def package_info(self):
        self.cpp_info.builddirs = [self.folders.build_folder]
