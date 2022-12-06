from conans import ConanFile
from conan.tools.cmake import CMake

class imageConan(ConanFile):
    name = 'image'
    version = '0.1.0'
    license = ''

    settings = 'os', 'compiler', 'build_type', 'arch'
    generators = 'CMakeDeps', 'CMakeToolchain'

    requires = ['fmt/9.1.0', 'benchmark/1.7.1']

    def build(self):
        cmake = CMake(self)

        if self.should_configure:
            cmake.configure()
        if self.should_build:
            cmake.build()
