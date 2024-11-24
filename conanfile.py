import os

from conan import ConanFile
from conan.tools.cmake import cmake_layout
from conan.tools.files import copy

class Z80Emu(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        self.requires("sdl/2.28.3")
        self.requires("imgui/[>1.91 <1.92]", headers=True, transitive_headers=True)

    def build_requirements(self):
        self.tool_requires("cmake/[>3.30]")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        imgui_backends_src = os.path.join(self.dependencies["imgui"].package_folder, "res", "bindings")
        imgui_backends_dest = os.path.join(self.source_folder, "libs", "imgui")

        copy(self, "*impl_sdlrenderer2*", imgui_backends_src, imgui_backends_dest)
        copy(self, "*impl_sdl2*", imgui_backends_src, imgui_backends_dest)
