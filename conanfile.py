import os

from conans import ConanFile, CMake, tools


class ScopeguardConan(ConanFile):
    name = "scope-guard"
    version = "0.3.5"
    license = "MIT"
    author = "offa <offa@github>"
    url = "https://github.com.offa/scope-guard"
    description = "Implementation of Scoped Guards and Unique Resource as proposed in P0052."
    homepage = "https://github.com/mrtazz/restclient-cpp"
    topics = ("cpp", "cpp17", "p0052", "scope-guard",
              "scope-exit", "scope-fail", "scope-success", "unique-resource", "cmake")
    no_copy_source = True
    _source_dir = "{}-{}".format(name, version)
    scm = {
        "type": "git",
        "subfolder": _source_dir,
        "url": "{}.git".format(homepage),
        "revision": version
    }
    requires = (
        "Catch2/2.9.2@catchorg/stable",
        "trompeloeil/v34@rollbear/stable"
    )


    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.configure(source_folder=self._source_dir, build_folder="build")
        return cmake

    def package(self):
        cmake = self._configure_cmake()
        cmake.install()
