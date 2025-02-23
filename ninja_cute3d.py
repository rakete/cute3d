# Cute3D, a simple opengl based framework for writing interactive realtime applications

# Copyright (C) 2013-2017 Andreas Raster

# This file is part of Cute3D.

# Cute3D is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# Cute3D is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with Cute3D.  If not, see <http://www.gnu.org/licenses/>.
from __future__ import print_function

import ninja_syntax
import ninja_build

import glob
import subprocess
import sys
import os
import re

if not ninja_build.command_exists("ninja"):
    print("could not find ninja, is ninja-build package installed?")
    sys.exit(1)

build_settings = ninja_build.detect_settings(sys.argv)
ninja_build.print_settings(build_settings)

# - always write to build.ninja in current directory for now
build_file_handle = open("build.ninja", "w+")
w = ninja_syntax.Writer(build_file_handle, 127)

# - generic rules for tools that I've put into ninja_build to make them reusable elsewhere
ninja_build.copy(w, build_settings.build_platform)
ninja_build.mkdir(w, build_settings.build_platform)
ninja_build.glsl_validate(w)
ninja_build.xxd(w, build_settings.source_directory)

# - I want to be able to use build_shaders in other projects to validate shaders and copy them where they belong
# - there is a lot of complexity hidden behind this one function call
shaders = ninja_build.build_shaders(w, build_settings.build_platform, build_settings.source_directory, build_settings.build_directory, build_settings.script_directory, "shader")

# - all dlls found in source_directory are copied to build_directory when building, but only
# when platform is windows and the build_directory and source_directory are not the same directory
dlls = []
if build_settings.build_platform == "windows" and os.path.relpath(build_settings.build_directory, build_settings.script_directory) != ".":
    os.chdir(build_settings.source_directory)
    dlls = glob.glob("*.dll")
    os.chdir(build_settings.current_directory)

    for d in dlls:
       w.build(d, "copy", os.path.join(build_settings.source_directory, d))
    w.newline()

# - the rest should be pretty straightforward, here we create compile and link rules depending on which toolset
# is selected and use the cflags and ldflags that we configured above
if build_settings.build_toolset == "mingw" or build_settings.build_toolset == "gcc":
    # - use gcc_compiler_color
    gcc_compiler_color_path = os.path.join(build_settings.script_directory, "scripts", "gcc_compiler_color.py")
    compiler_call = "gcc "
    if build_settings.build_platform != "windows":
        compiler_call = "python " + gcc_compiler_color_path + " gcc"
    # -c and /c in gcc and cl.exe mean: compile without linking
    w.rule(name="compile", command=compiler_call + " -MMD -MF $out.d -c $in -o $out " + build_settings.cflags, deps="gcc", depfile="$out.d")
    w.newline()
    w.rule(name="link", command="gcc $in -o $out " + build_settings.ldflags)
    w.newline()
elif build_settings.build_toolset == "msvc":
    # - needs /FS to enable synchronous writes to pdb database
    w.rule(name="compile", command="cl.exe /nologo /showIncludes /FS /c $in /Fo$out " + build_settings.cflags, deps="msvc")
    w.newline()
    w.rule(name="link", command="cl.exe /nologo $in /link " + build_settings.ldflags + " /out:$out")
    w.newline()

# - chdir to source_directory and enumrate all .c files
os.chdir(build_settings.source_directory)
sources = glob.glob("*.c")
os.chdir(build_settings.current_directory)

# - create build command for every .c file creating an .o (or .obj if platform is windows)
objects = []
for c in sources:
    if build_settings.build_toolset == "msvc":
        o = c.replace(".c", ".obj")
    else:
        o = c.replace(".c", ".o")
    w.build(o, "compile", os.path.join(build_settings.source_directory, c))
    objects.append(o)
w.newline()

# - default is to just build all objects
w.default(objects)
w.newline()

# - enumerate all tests and create build rule for all of them, using dlls and shaders as dependencies
# so that building a test triggers shader validation and dll/shader copying
tests_directory = os.path.relpath(os.path.join(build_settings.source_directory, "tests"), build_settings.current_directory)
os.chdir(tests_directory)
test_sources = glob.glob("*.c")
os.chdir(build_settings.current_directory)
for c in test_sources:
    if build_settings.build_toolset == "msvc":
        o = c.replace(".c", ".obj")
    else:
        o = c.replace(".c", ".o")
    w.build(o, "compile", os.path.join(tests_directory, c))

    binary = c.replace(".c", "")
    if build_settings.build_platform == "windows":
        w.build(binary + ".exe", "link", [o] + objects)
        w.build(binary, "phony", binary + ".exe", dlls + shaders)
    else:
        w.build(binary + ".bin", "link", [o] + objects)
        w.build(binary, "phony", binary + ".bin", dlls + shaders)

    w.newline()
