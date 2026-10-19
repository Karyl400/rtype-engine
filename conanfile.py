from conan import ConanFile
from conan.tools.cmake import CMakeDeps, CMakeToolchain


class RTypeEngineConan(ConanFile):
    name = "rtype-engine"
    version = "0.1.0"
    settings = "os", "compiler", "build_type", "arch"
    options = {"sfml": [True, False]}
    default_options = {"sfml": False}

    def requirements(self):
        self.requires("asio/1.32.0")
        if self.options.sfml:
            self.requires("sfml/3.0.2")

    def generate(self):
        CMakeDeps(self).generate()
        toolchain = CMakeToolchain(self)
        toolchain.user_presets_path = False
        toolchain.generate()
