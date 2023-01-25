import re
import os
from conans import ConanFile, CMake, tools


class ScopeguardConan(ConanFile):
    name = "scope-guard"
    license = "MIT"
    author = "offa <offa@github>"
    url = "https://github.com.offa/scope-guard"
    description = "Implementation of Scoped Guards and Unique Resource as proposed in P0052."
    homepage = "https://github.com/offa/scope-guard"
    topics = ("cpp", "cpp17", "p0052", "scope-guard",
              "scope-exit", "scope-fail", "scope-success", "unique-resource", "cmake")
    generators = ("cmake_find_package", "cmake_paths")
    exports = ["LICENSE"]
    exports_sources = ("CMakeLists.txt", "include/*", "test/*", "cmake/*")
    options = {
        "unittest": ["ON", "OFF"],
        "enable_compat_header": ["ON", "OFF"]
    }
    default_options = (
        "unittest=ON",
        "enable_compat_header=OFF"
    )
    __requirements = [
        "catch2/3.3.0",
        "trompeloeil/43"
    ]


    def set_version(self):
        cmake_lists_content = tools.load(os.path.join(self.recipe_folder, "CMakeLists.txt"))
        project_match = re.search(r'project\s*\((.+?)\)', cmake_lists_content, re.DOTALL)

        if not project_match:
            raise ConanInvalidConfiguration("No valid project() statement found in CMakeLists.txt")

        project_params = project_match.group(1).split()
        version_string = project_params[project_params.index("VERSION") + 1]

        if not re.search(r'\d+\.\d+\.\d+(?:\.\d)?', version_string):
            raise ConanInvalidConfiguration("No valid version found in CMakeLists.txt")

        self.version = version_string
        self.output.info(f"Project version from CMakeLists.txt: '{self.version}'")

    def requirements(self):
        if self.options.unittest:
            for req in self.__requirements:
                self.requires(req)

    def package(self):
        self.copy("LICENSE", dst="license")
        cmake = self._configure_cmake()
        cmake.install()

    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.definitions["UNITTEST"] = self.options.unittest
        cmake.definitions["ENABLE_COMPAT_HEADER"] = self.options.enable_compat_header
        cmake.configure(build_folder="build")
        return cmake
