import re
import os
from conan import ConanFile
from conan.errors import ConanInvalidConfiguration
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps
from conan.tools.files import load, copy


class ScopeguardConan(ConanFile):
    name = "scope-guard"
    license = "MIT"
    author = "offa <offa@github>"
    url = "https://github.com/offa/scope-guard"
    description = (
        "Implementation of Scoped Guards and Unique Resource as proposed in P0052."
    )
    homepage = "https://github.com/offa/scope-guard"
    topics = (
        "cpp",
        "cpp17",
        "p0052",
        "scope-guard",
        "scope-exit",
        "scope-fail",
        "scope-success",
        "unique-resource",
    )
    settings = "os", "arch", "compiler", "build_type"
    exports = ["LICENSE"]
    exports_sources = ("CMakeLists.txt", "include/*", "test/*", "cmake/*")
    package_type = "header-library"
    options = {"unittest": [True, False], "enable_compat_header": [True, False]}
    default_options = {"unittest": False, "enable_compat_header": False}

    def set_version(self):
        cmake_lists_content = load(
            self, os.path.join(self.recipe_folder, "CMakeLists.txt")
        )
        project_match = re.search(
            r"project\s*\((.+?)\)", cmake_lists_content, re.DOTALL
        )

        if not project_match:
            raise ConanInvalidConfiguration(
                "No valid project() statement found in CMakeLists.txt"
            )

        project_params = project_match.group(1).split()
        version_string = project_params[project_params.index("VERSION") + 1]

        if not re.search(r"\d+\.\d+\.\d+(?:\.\d)?", version_string):
            raise ConanInvalidConfiguration("No valid version found in CMakeLists.txt")

        self.version = version_string
        self.output.info(f"Project version from CMakeLists.txt: '{self.version}'")

    def requirements(self):
        if self.options.unittest:
            self.requires("catch2/3.13.0")
            self.requires("trompeloeil/49")

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["SCOPEGUARD_UNITTEST"] = self.options.unittest
        tc.variables["SCOPEGUARD_ENABLE_COMPAT_HEADER"] = (
            self.options.enable_compat_header
        )
        tc.generate()

        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = self._configure_cmake()
        cmake.build()

    def package(self):
        copy(
            self,
            pattern="LICENSE",
            dst=os.path.join(self.package_folder, "licenses"),
            src=self.source_folder,
        )
        cmake = self._configure_cmake()
        cmake.install()

    def package_info(self):
        self.cpp_info.set_property("cmake_file_name", "ScopeGuard")
        self.cpp_info.set_property("cmake_target_name", "ScopeGuard::ScopeGuard")
        self.cpp_info.bindirs = []
        self.cpp_info.libdirs = []

    def package_id(self):
        self.info.clear()

    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.configure()
        return cmake
