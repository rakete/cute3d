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

import os
import re
import glob

def command_exists(cmd):
    for path in os.environ["PATH"].split(os.pathsep):
        if os.access(os.path.join(path, cmd), os.X_OK):
            return os.path.join(path, cmd)

        if os.access(os.path.join(path, cmd + ".exe"), os.X_OK):
            return os.path.join(path, cmd + ".exe")

    return None

def pairwise(it):
    it = iter(it)
    while True:
        yield next(it), next(it)

def copy(w, build_platform):
    # - create a generic copy rule, warning about not having gnu cp on windows because the builtin
    # copy is a pita, xcopy did not work either
    if build_platform == "windows":
        if command_exists("cp"):
            w.rule(name="copy", command="cp $in $out")
        else:
            print("WARNING: using windows copy command!")
            print("if you want a reliable and sane build experience you should get gnu cp and")
            print("put it in your PATH, I am writing this as I am about to give up to try and make things")
            print("work with windows builtin copy and xcopy.exe, this script will use the cmd builtin")
            print("copy for copying, but I know that this has several issues and is slow, if you")
            print("happen to know a better way, please file an issue, thank you")
            w.rule(name="copy", command="cmd /c \"copy $in $out >nul\"")
    else:
        w.rule(name="copy", command="cp $in $out")
        w.newline()

def mkdir(w, build_platform):
    if build_platform == "windows":
        if command_exists("mkdir.exe"):
            w.rule(name="mkdir", command="mkdir -p $out")
        else:
            w.rule(name="mkdir", command="cmd /c \"if not exist $out mkdir $out\"")
    else:
        w.rule(name="mkdir", command="mkdir -p $out")
        w.newline()

def glsl_validate(w):
    # - we use prefix shaders for glsl version compatibilty, and need to prepend those when validating, that is done
    # via the $prefix variable, we can't hardcode the prefix shaders here because their path may be variable
    # - the --write parameter is important, it outputs the full shader as flat.full.vert (for example), so that we
    # can rely on those files as dependencies in ninja, they serve us no other function
    module_directory = os.path.dirname(os.path.realpath(__file__))
    glsl_validate_path = os.path.join(module_directory, "scripts", "glsl_validate.py")

    if os.path.exists(glsl_validate_path):
        w.rule(name="validate_glsl", command="python " + glsl_validate_path + " --no-color $prefix $in $write")
        w.newline()

def xxd(w, source_directory):
    module_directory = os.path.dirname(os.path.realpath(__file__))
    xxd_path = os.path.join(module_directory, "scripts", "xxd.py")

    if os.path.exists(xxd_path):
        w.rule(name="xxd", command="python " + xxd_path + " $in $out $name_prefix")
        w.newline()

def build_shaders(w, build_platform, source_directory, build_directory, script_directory, shader_subdir):
    # - most complex part of this script deals with validating and copying the glsl shaders
    # - first we create the shader directory, since shaders are like an asset the gets loaded
    # at runtime, we just recreate the same shader_directory in the build_directory so that
    # any hardcoded relative paths still work
    w.build(shader_subdir, "mkdir")
    w.newline()

    # - for every shader in the source+shader directory, we need to create several build statements:
    # 1. when the build is out of source, we need to copy the shader, if it is not we create a phony rule instead
    # 2. we want to validate the shader with glsl_validate.py
    # 3. we need to generate a .h file with xxd.py from the shader file so that it can be included as string literal
    # - making things difficult is that ninja does not use a shell but creates a process for every build command,
    # and the workaround for that on windows, using cmd /c, does not really work that well for my emacs setup, so
    # I won't create build rules with multiple commands, but only build rules which only run one command
    source_shader_directory = os.path.relpath(os.path.join(source_directory, shader_subdir), build_directory)
    shaders = []

    # - first things first, I just list all potential shader files here, but without extensions, thats why I am using
    # these scary looking list comprehensions instead of just a simple glob
    # - I only list .vert files because these must come in .vert/.frag pairs anyways or they would not work
    current_directory = os.getcwd()
    os.chdir(source_shader_directory)
    shader_filenames_with_extensions = []
    [shader_filenames_with_extensions.append(sf) for sf in glob.glob("*.vert")]
    os.chdir(current_directory)

    # - iterate unique filenames without extension,
    # - then vert_shader is the name (like flat.vert, frag_shader for .frag),
    # - source_vert_shader is the source path (like ../shader/flat.vert),
    # - full_vert_shader is the full file that glsl_validate.py writes if given the --write parameter (like shader/flat.full.vert),
    # - dest_vert_shader is just the destination path for the shader (like shader/flat.vert), just like full_vert_shader
    # it is a path relative to the current build directory
    for shader_filename_with_extension in shader_filenames_with_extensions:
        # - when the filename looks like this: flat.vert_with_prefix, then it is one of the files that has
        # been output by the glsl_validate.py script when it validates a shader, and we can ignore it
        if re.search("_with_prefix$", shader_filename_with_extension):
            continue

        shader_filename = os.path.splitext(shader_filename_with_extension)[0]

        vert_shader = shader_filename + ".vert"
        frag_shader = shader_filename + ".frag"
        source_vert_shader = os.path.join(source_shader_directory, vert_shader)
        source_frag_shader = os.path.join(source_shader_directory, frag_shader)
        full_vert_shader = os.path.join(shader_subdir, shader_filename + ".vert_with_prefix")
        full_frag_shader = os.path.join(shader_subdir, shader_filename + ".frag_with_prefix")
        dest_vert_shader = os.path.join(shader_subdir, vert_shader)
        dest_frag_shader = os.path.join(shader_subdir, frag_shader)

        prefix_deps = []
        if shader_filename != "prefix":
            prefix_deps = [os.path.join(shader_subdir, "prefix.vert"), os.path.join(shader_subdir, "prefix.frag")]

        out_of_source = False
        if os.path.relpath(build_directory, script_directory) != ".":
            out_of_source = True

        # - these following two if statements create all the build commands
        if os.path.isfile(source_vert_shader):
            if shader_filename != "prefix":
                # - if we are NOT looking at a prefix shader create a build rule that validates the shader, with two
                # variables that specifiy the prefix shaders and the shader_subdir that glsl_validate.py should write to
                # - make the validate_glsl rule depend on the prefix_deps, so that those get copied first before running
                # any validation
                w.build(full_vert_shader, "validate_glsl", source_vert_shader, order_only=prefix_deps)
                w.variable("prefix", prefix_deps, 1)
                w.variable("write", "--write " + shader_subdir, 1)
                # - if the current build is out of source, we need to copy the shader into build directory, if not then we
                # just create a phony rule, in both cases we need to depend on the full_vert_shader so that the validation
                # above gets triggered
                if out_of_source:
                    w.build(dest_vert_shader, "copy", source_vert_shader, order_only=full_vert_shader)
                else:
                    w.build(dest_vert_shader, "phony", full_vert_shader)
                # - append dest_vert_shader to shaders, we'll return those later so that other rules can depend on them
                shaders.append(dest_vert_shader)
            elif out_of_source:
                w.build(dest_vert_shader, "copy", source_vert_shader)

        # - same thing as above but for .frag extension instead of .vert
        if os.path.isfile(source_frag_shader):
            if shader_filename != "prefix":
                w.build(full_frag_shader, "validate_glsl", source_frag_shader, order_only=prefix_deps)
                w.variable("prefix", prefix_deps, 1)
                w.variable("write", "--write " + shader_subdir, 1)
                if out_of_source:
                    w.build(dest_frag_shader, "copy", source_frag_shader, order_only=full_frag_shader)
                else:
                    w.build(dest_frag_shader, "phony", full_frag_shader)
                shaders.append(dest_frag_shader)
            elif out_of_source:
                w.build(dest_frag_shader, "copy", source_frag_shader)

        w.newline()
    return shaders
