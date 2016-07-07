import ninja_syntax
import glob
import platform
import subprocess
import sys
import os

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

# - I support out of tree builds, and so I take the path where this script lies, an absolute path,
# and then use that script_directory to compute a relative path from the current_directory, which is
# also the (potential) out of tree build_directory, to the source_directory
script_directory = os.path.dirname(os.path.realpath(__file__))
current_directory = os.getcwd()
source_directory = os.path.relpath(script_directory, current_directory)
build_directory = current_directory

print "source_directory: " + source_directory
print "build_directory: " + build_directory

# - besides figuring out directories the largest part of the first section of this script is figuring various
# build related configuration settings, most importantly the platform we are building on, which for now
# should only be windows or linux, and the toolset, which can be gcc, mingw or msvc (and clang sooner or
# later)
# - the architecture is not used right now, but eventually I want to be able to specify which archtecture to
# build for
build_platform = platform.system().lower()
build_architecture = platform.machine().lower()

# - default is gcc, on windows its mingw
build_toolset = "gcc"
if build_platform == "windows":
    build_toolset = "mingw"

# - the toolset is so far the only thing that can be specified by the user as parameter, I may add things like
# configuration (release, debug, ...) later
if len(sys.argv) > 1:
    user_toolset = sys.argv[1].lower()
    if user_toolset == "gcc" or user_toolset == "clang" or user_toolset == "mingw":
        build_toolset = user_toolset
    elif user_toolset == "msvc" and build_platform == "windows":
        build_toolset = user_toolset
    else:
        print "can not configure toolset " + user_toolset
        print "use gcc, mingw or msvc"
        sys.exit(1)

print "build_platform: " + build_platform
print "build_toolset: " + build_toolset

# - now that the toolset and platform are known, I set the compiler and linker parameter settings according
# to which toolset/platform combination has been selected
# - I try to organize this settings into different categories:
#  - features: compiler features and debugging
#  - optimization: code optimization settings
#  - linking: parameters that influence linking
#  - libraries: libraries to link against and their paths
#  - includes: include paths
# - these then get concatenated into cflags and ldflags, which I later use when creating the compile and link
# rules
# - sdl2_cflags and sdl2_libs are seperate so that I can set them with the sdl2-config utility
sdl2_cflags = ""
sdl2_libs = ""
features = ""
optimization = ""
warnings = ""
linking = ""
libraries = ""
includes = ""
cflags = ""
ldflags = ""
# - I want this whole section to only set defaults in the future, and then maybe use an ini file or something that the user
# is supposed to create with the actual, mmh, maybe only the things that I can not know, like paths to libraries and stuff
# like that
if build_platform == "linux" or build_toolset == "gcc":
    sdl2_cflags = subprocess.check_output(["bash", "sdl2-config", "--cflags"]).rstrip()
    sdl2_libs = subprocess.check_output(["bash", "sdl2-config", "--libs"]).rstrip()

    features = "-std=c11 -pg -DDEBUG -fsanitize=address -fno-omit-frame-pointer"
    optimization = "-flto=4 -march=native"
    warnings = "-Wall -Wmaybe-uninitialized -Wsign-conversion -Wno-missing-field-initializers -Wno-missing-braces -Wno-pedantic-ms-format -Wno-unknown-pragmas -pedantic"
    linking = "-fPIC"
    libraries = "-lm -lGL " + sdl2_libs
    includes = "-I" + source_directory

    cflags = features + " " + warnings + " " + linking + " " + sdl2_cflags + " " + optimization + " " + includes
    ldflags = linking + " " + libraries
elif build_toolset == "mingw":
    if command_exists("sdl2-config") and command_exists("bash"):
        sdl2_cflags = subprocess.check_output(["bash", "sdl2-config", "--cflags"]).rstrip()
        sdl2_libs = subprocess.check_output(["bash", "sdl2-config", "--libs"]).rstrip()
    else:
        sdl2_cflags = "-Ic:/MinGW/include/SDL2 -Dmain=SDL_main"
        sdl2_libs = "-Lc:/MinGW/lib -lmingw32 -lSDL2main -lSDL2 -mwindows"

    features = "-posix -std=c11 -pg -DDEBUG -fsanitize=address -fno-omit-frame-pointer"
    optimization = "-flto=4 -march=native"
    warnings = "-Wall -Wmaybe-uninitialized -Wsign-conversion -Wno-missing-field-initializers -Wno-missing-braces -Wno-pedantic-ms-format -Wno-unknown-pragmas -pedantic"
    linking = "-fPIC"
    libraries = "-lole32 -loleaut32 -limm32 -lwinmm -lversion -lm -lopengl32 " + sdl2_libs
    includes = "-I" + source_directory

    cflags = features + " " + warnings + " " + linking + " " + sdl2_cflags + " " + optimization + " " + includes
    ldflags = linking + " " + libraries
elif build_toolset == "msvc":
    sdl2_cflags = "/Ic:\\VC\\SDL2-2.0.4\\include"
    sdl2_libs = "/LIBPATH:c:\\VC\\SDL2-2.0.4\\lib\\x64 SDL2.lib SDL2main.lib"

    # - MD is for dynamic linking
    features = " /DDEBUG /DCUTE_BUILD_MSVC /MD"
    # - Oi and Zi are inlining related, don't remember really, without them I had unresolved references iirc
    optimization = "/Oi /Zi /Od"
    # warning C4204: nonstandard extension used: non-constant aggregate initializer
    # warning C4996: 'strncat': This function or variable may be unsafe. Consider using strncat_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
    # warning C4068: unknown pragma
    warnings = "/W4 /wd4204 /wd4996 /wd4068"
    linking = ""
    # - chkstk.obj for _alloca
    # - msvcurt.lib: Static library for the pure managed CRT.
    # - /ENTRY:main needs libucrt.lib
    # - /ENTRY:main made exe hang on exit
    libraries = "msvcrt.lib opengl32.lib chkstk.obj " + sdl2_libs
    includes = "/I" + source_directory

    cflags = features + " " + warnings + " " + linking + " " + sdl2_cflags + " " + optimization + " " + includes
    ldflags = "/SUBSYSTEM:CONSOLE " + linking + " " + libraries
else:
    print "building with " + build_toolset + " on " + build_platform + " is not supported yet."
    sys.exit(1)

print "sdl2_cflags: " + sdl2_cflags
print "sdl2_libs: " + sdl2_libs
print "features: " + features
print "optimization: " + optimization
print "warnings: " + warnings
print "linking: " + linking
print "libraries: " + libraries
print "cflags: " + cflags
print "ldflags: " + ldflags

# - always write to build.ninja in current directory for now
build_file_handle = open("build.ninja", "w+")
w = ninja_syntax.Writer(build_file_handle, 127)

# - create a generic copy rule, warning about not having gnu cp on windows because the builtin
# copy is a pita, xcopy did not work either
if build_platform == "windows":
    if command_exists("cp"):
        w.rule(name="copy", command="cp $in $out")
    else:
        print "WARNING: using windows copy command!"
        print "if you want a reliable and sane build experience you should get gnu cp and"
        print "put it in your PATH, I am writing this as I am about to give up to make things"
        print "with windows builtin copy and xcopy.exe, this script will use the cmd builtin"
        print "copy for copying, but I know that this has several issues and is slow, if you"
        print "happen to know a better way, please file an issue, thank you"
        w.rule(name="copy", command="cmd /c \"copy $in $out >nul\"")
else:
    w.rule(name="copy", command="cp $in $out")
w.newline()

# - all dlls found in source_directory are copied to build_directory when building, but only
# when platform is windows and the build_directory and source_directory are not the same directory
dlls = []
if build_platform == "windows" and os.path.relpath(build_directory, script_directory) != ".":
    os.chdir(source_directory)
    dlls = glob.glob("*.dll")
    os.chdir(current_directory)

    for d in dlls:
       w.build(d, "copy", os.path.join(source_directory, d))
    w.newline()

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

# - when the glsl-validate.py script is found in path, create validate_glsl rule
# - we use prefix shaders for glsl version compatibilty, and need to prepend those when validating, this only
# with my own glsl-validator.py fork for now
# - the --write parameter is important, it outputs the full shader as flat.full.vert (for example), so that we
# can rely on those files as dependencies in ninja, they serve us no other function
prefix_shader = []
glsl_validate = command_exists("glsl-validate.py")
if glsl_validate:
    prefix_shader = [os.path.join("shader", "prefix.vert"), os.path.join("shader", "prefix.frag")]
    prefix_shader_string = " ".join(prefix_shader)

    w.rule(name="validate_glsl", command="python " + glsl_validate + " " + prefix_shader_string + " $in --write")
    w.newline()

# - for every shader in the source+shader directory, we need to create several build statements:
# 1. when the build is out of source, we need to copy the shader
# 2. when there is a glsl-validate.py command, we want to use it to validate the shader
# 3. we create an additional phony build statement so that we can depend on something simple like flat.vert
# - making things difficult is that ninja does not use a shell but creates a process for every build command,
# and the workaround for that on windows, using cmd /c, does not really work that well for my emacs setup, so
# I won't create build rules with multiple commands, but only build rules which only run one command
shader_directory = os.path.relpath(os.path.join(source_directory, "shader"), current_directory)
shaders = []

# - first things first, I just list all potential shader files here, but without extensions, thats why I am using
# these scary looking list comprehensions instead of just a simple glob
os.chdir(shader_directory)
shader_filenames = []
[shader_filenames.append(os.path.splitext(sf)[0]) for sf in glob.glob("*.vert")]
[shader_filenames.append(os.path.splitext(sf)[0]) for sf in glob.glob("*.frag") if os.path.splitext(sf)[0] not in shader_filenames]
os.chdir(current_directory)

# - iterate unique filenames without extension, then vert_shader is the name (like flat.vert, frag_shader for .frag),
# source_vert_shader is the source path (like ../shader/flat.vert), full_vert_shader is the full file that glsl_validate.py
# writes if given the --write parameter (like shader/flat.full.vert)
# - notice that the full_vert_shader path is relative to the current build directory
for shader_filename in shader_filenames:
    vert_shader = shader_filename + ".vert"
    frag_shader = shader_filename + ".frag"
    source_vert_shader = os.path.join(shader_directory, vert_shader)
    source_frag_shader = os.path.join(shader_directory, frag_shader)
    full_vert_shader = os.path.join("shader", shader_filename + ".full.vert")
    full_frag_shader = os.path.join("shader", shader_filename + ".full.frag")

    # - prefix_deps we create validate and copy rules for the prefix shaders just like for all the other shaders,
    # but all the other shaders will then have the prefix shaders as order_only deps, so that the prefix shaders get
    # copied first, and can then be used in the validate commands for the other shaders
    prefix_deps = []
    if shader_filename != "prefix":
        prefix_deps = [os.path.join("shader", "prefix.vert"), os.path.join("shader", "prefix.frag"), ]

    # - these following two if statements create all the build commands, check if the source actually exist because
    # it may be possible that there is a vert shader without frag equivalent for example
    # - dest_vert_shader is just the destination path for the shader (like shader/flat.vert), just like full_vert_shader
    # it is a path relative to the current build directory
    if os.path.isfile(source_vert_shader):
        dest_vert_shader = os.path.join("shader", vert_shader)

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
        if glsl_validate:
            w.build(full_vert_shader, "validate_glsl", dest_vert_shader, order_only=prefix_deps)
            w.build(vert_shader, "phony", full_vert_shader)
            shaders.append(vert_shader)
        elif os.path.relpath(build_directory, script_directory) != ".":
            w.build(vert_shader, "phony", dest_vert_shader)
            shaders.append(vert_shader)
    w.newline()

    # - same thing as above but for .frag extension instead of .vert
    if os.path.isfile(source_frag_shader):
        dest_frag_shader = os.path.join("shader", frag_shader)

        if os.path.relpath(build_directory, script_directory) != ".":
            w.build(dest_frag_shader, "copy", source_frag_shader, order_only=["shader"])

        if glsl_validate:
            w.build(full_frag_shader, "validate_glsl", dest_frag_shader, order_only=prefix_deps)
            w.build(frag_shader, "phony", full_frag_shader)
            shaders.append(frag_shader)
        else:
            w.build(frag_shader, "phony", dest_frag_shader)
            shaders.append(frag_shader)
    w.newline()
w.newline()

# - the rest should be pretty straightforward, here we create compile and link rules depending on which toolset
# is selected and use the cflags and ldflags that we configured above
if build_toolset == "mingw" or build_toolset == "gcc":
    # -c and /c in gcc and cl.exe mean: compile without linking
    w.rule(name="compile", command="gcc -MMD -MF $out.d -c $in -o $out " + cflags, deps="gcc", depfile="$out.d")
    w.newline()
    w.rule(name="link", command="gcc $in -o $out " + ldflags)
    w.newline()
elif build_toolset == "msvc":
    # - needs /FS to enable synchronous writes to pdb database
    w.rule(name="compile", command="cl.exe /nologo /showIncludes /FS /c $in /Fo$out " + cflags, deps="msvc")
    w.newline()
    w.rule(name="link", command="cl.exe /nologo $in /link " + ldflags + " /out:$out")
    w.newline()

# - chdir to source_directory and enumrate all .c files
os.chdir(source_directory)
sources = glob.glob("*.c")
sources.remove("driver_allegro.c")
os.chdir(current_directory)

# - create build command for every .c file creating an .o (or .obj if platform is windows)
objects = []
for c in sources:
    if build_toolset == "msvc":
        o = c.replace(".c", ".obj")
    else:
        o = c.replace(".c", ".o")
    w.build(o, "compile", os.path.join(source_directory, c))
    objects.append(o)
w.newline()

# - default is to just build all objects
w.default(objects)
w.newline()

# - enumerate all tests and create build rule for all of them, using dlls and shaders as dependencies
# so that building a test triggers shader validation and dll/shader copying
tests_directory = os.path.relpath(os.path.join(source_directory, "tests"), current_directory)
os.chdir(tests_directory)
test_sources = glob.glob("*.c")
os.chdir(current_directory)
for c in test_sources:
    if build_toolset == "msvc":
        o = c.replace(".c", ".obj")
    else:
        o = c.replace(".c", ".o")
    w.build(o, "compile", os.path.join(tests_directory, c))

    binary = c.replace(".c", "")
    if build_platform == "windows":
        w.build(binary + ".exe", "link", [o] + objects)
        w.build(binary, "phony", binary + ".exe", dlls + shaders)
    else:
        w.build(binary + ".bin", "link", [o] + objects)
        w.build(binary, "phony", binary + ".bin", dlls + shaders)

    w.newline()
