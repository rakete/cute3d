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
            print "WARNING: using windows copy command!"
            print "if you want a reliable and sane build experience you should get gnu cp and"
            print "put it in your PATH, I am writing this as I am about to give up to try and make things"
            print "work with windows builtin copy and xcopy.exe, this script will use the cmd builtin"
            print "copy for copying, but I know that this has several issues and is slow, if you"
            print "happen to know a better way, please file an issue, thank you"
            w.rule(name="copy", command="cmd /c \"copy $in $out >nul\"")
    else:
        w.rule(name="copy", command="cp $in $out")
        w.newline()

def mkdir(w, build_platform):
    # - most complex part of this script deals with validating and copying the glsl shaders
    # - first we make a generic mkdir rule so that we can create the shader directory, since shaders
    # are like an asset the gets loaded at runtime, we just recreate the same shader_directory in the
    # build_directory so that any hardcoded relative paths still work
    if build_platform == "windows":
        if command_exists("mkdir.exe"):
            w.rule(name="mkdir", command="mkdir -p $out")
        else:
            w.rule(name="mkdir", command="cmd /c \"if not exist $out mkdir $out\"")
    else:
        w.rule(name="mkdir", command="mkdir -p $out")
        w.newline()

    w.build("shader", "mkdir")
    w.newline()

def glsl_validate(w):
        # - when the glsl-validate.py script is found in path, create validate_glsl rule
    # - we use prefix shaders for glsl version compatibilty, and need to prepend those when validating, this only
    # works with my own glsl-validator.py fork for now
    # - the --write parameter is important, it outputs the full shader as flat.full.vert (for example), so that we
    # can rely on those files as dependencies in ninja, they serve us no other function
    prefix_shader = []
    glsl_validate = command_exists("glsl-validate.py")
    if glsl_validate:
        prefix_shader = [os.path.join("shader", "prefix.vert"), os.path.join("shader", "prefix.frag")]
        prefix_shader_string = " ".join(prefix_shader)

        w.rule(name="validate_glsl", command="python " + glsl_validate + " --no-color " + prefix_shader_string + " $in --write shader")
        w.newline()


def build_shaders(w, build_platform, source_directory, build_directory, script_directory, shader_subdir):
    glsl_validate = command_exists("glsl-validate.py")

    # - for every shader in the source+shader directory, we need to create several build statements:
    # 1. when the build is out of source, we need to copy the shader
    # 2. when there is a glsl-validate.py command, we want to use it to validate the shader
    # 3. we create an additional phony build statement so that we can depend on something simple like flat.vert
    # - making things difficult is that ninja does not use a shell but creates a process for every build command,
    # and the workaround for that on windows, using cmd /c, does not really work that well for my emacs setup, so
    # I won't create build rules with multiple commands, but only build rules which only run one command
    source_shader_directory = os.path.relpath(os.path.join(source_directory, shader_subdir), build_directory)
    shaders = []

    # - first things first, I just list all potential shader files here, but without extensions, thats why I am using
    # these scary looking list comprehensions instead of just a simple glob
    current_directory = os.getcwd()
    os.chdir(source_shader_directory)
    shader_filenames = []
    [shader_filenames.append(os.path.splitext(sf)[0]) for sf in glob.glob("*.vert")]
    [shader_filenames.append(os.path.splitext(sf)[0]) for sf in glob.glob("*.frag") if os.path.splitext(sf)[0] not in shader_filenames]
    os.chdir(current_directory)

    # - iterate unique filenames without extension, then vert_shader is the name (like flat.vert, frag_shader for .frag),
    # source_vert_shader is the source path (like ../shader/flat.vert), full_vert_shader is the full file that glsl_validate.py
    # writes if given the --write parameter (like shader/flat.full.vert)
    # - notice that the full_vert_shader path is relative to the current build directory
    for shader_filename in shader_filenames:
        if re.search("\.full$", shader_filename):
            continue

        vert_shader = shader_filename + ".vert"
        frag_shader = shader_filename + ".frag"
        source_vert_shader = os.path.join(source_shader_directory, vert_shader)
        source_frag_shader = os.path.join(source_shader_directory, frag_shader)
        full_vert_shader = os.path.join(shader_subdir, shader_filename + ".full.vert")
        full_frag_shader = os.path.join(shader_subdir, shader_filename + ".full.frag")

        # - prefix_deps we create validate and copy rules for the prefix shaders just like for all the other shaders,
        # but all the other shaders will then have the prefix shaders as order_only deps, so that the prefix shaders get
        # copied first, and can then be used in the validate commands for the other shaders
        prefix_deps = []
        if shader_filename != "prefix":
            prefix_deps = [os.path.join(shader_subdir, "prefix.vert"), os.path.join(shader_subdir, "prefix.frag")]

        # - these following two if statements create all the build commands, check if the source actually exist because
        # it may be possible that there is a vert shader without frag equivalent for example
        # - dest_vert_shader is just the destination path for the shader (like shader/flat.vert), just like full_vert_shader
        # it is a path relative to the current build directory
        if os.path.isfile(source_vert_shader):
            dest_vert_shader = os.path.join(shader_subdir, vert_shader)

            # - only create a copy build command if this is an out of source build
            if os.path.relpath(build_directory, script_directory) != ".":
                w.build(dest_vert_shader, "copy", source_vert_shader, order_only=["shader"])

            # - if glsl_validate exists, create validate command and make phony rule depend on full_vert_shader so that
            # the validation gets triggered if something depends on the vert_shader name
            # - else just create the phony rule depend on dest_vert_shader, so that only the copy gets triggered, again
            # only if this is an out of source build, if not then nothing gets created
            # - append filename to shaders list, which are later used as dependencies for executables, so that shaders
            # get copied and validated when I build test-solid.exe for example, append is in both if branches and not
            # outside so that nothing gets append if this is _not_ an out of source build and glsl_validate.py does _not_
            # exist
            if glsl_validate and shader_filename != "prefix":
                w.build(full_vert_shader, "validate_glsl", source_vert_shader, order_only=prefix_deps)
                #w.build(vert_shader, "phony", full_vert_shader, dest_vert_shader)
                shaders.append(dest_vert_shader)
            elif os.path.relpath(build_directory, script_directory) != ".":
                #w.build(vert_shader, "phony", dest_vert_shader)
                shaders.append(dest_vert_shader)
                w.newline()

        # - same thing as above but for .frag extension instead of .vert
        if os.path.isfile(source_frag_shader):
            dest_frag_shader = os.path.join(shader_subdir, frag_shader)

            if os.path.relpath(build_directory, script_directory) != ".":
                w.build(dest_frag_shader, "copy", source_frag_shader, order_only=["shader"])

            if glsl_validate and shader_filename != "prefix":
                w.build(full_frag_shader, "validate_glsl", source_frag_shader, order_only=prefix_deps)
                #w.build(frag_shader, "phony", full_frag_shader, dest_frag_shader)
                shaders.append(dest_frag_shader)
            else:
                #w.build(frag_shader, "phony", dest_frag_shader)
                shaders.append(dest_frag_shader)
                w.newline()
                w.newline()

    return shaders
