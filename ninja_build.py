import ninja_syntax
import ninja_cute3d

import glob
import platform
import subprocess
import sys
import os
import re

if not ninja_cute3d.command_exists("ninja"):
    print "could not find ninja, is ninja-build package installed?"
    sys.exit(1)

# - I support out of tree builds, and so I take the path where this script lies, an absolute path,
# and then use that script_directory to compute a relative path from the current_directory to the
# source_directory
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
# - I try to organize these. settings into different categories:
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
errors = ""
linking = ""
libraries = ""
includes = ""
cflags = ""
ldflags = ""
# - I want this whole section to only set defaults in the future, and then maybe use an ini file or something that the user
# is supposed to create with the actual, mmh, maybe only the things that I can not know, like paths to libraries and stuff
# like that
if build_platform == "linux" or build_toolset == "gcc":
    if ninja_cute3d.command_exists("sdl2-config"):
        sdl2_cflags = subprocess.check_output(["bash", "sdl2-config", "--cflags"]).rstrip()
        sdl2_libs = subprocess.check_output(["bash", "sdl2-config", "--libs"]).rstrip()
    else:
        print "could not find sdl2-config, is libsdl2-dev package installed?"
        sys.exit(1)

    features = "-std=c11 -g -DDEBUG -fsanitize=address -fno-omit-frame-pointer "
    optimization = "-O0" # "-flto=4 -march=native"
    warnings = "-Wall -Wmaybe-uninitialized -Wsign-conversion -Wno-missing-field-initializers -Wno-missing-braces -Wno-pedantic-ms-format -Wno-unknown-pragmas -pedantic"
    errors = "-Werror=implicit-function-declaration"
    linking = "-fPIC"
    # - because of bug in gcc(?), I need to explicitly link with -lasan when I use -fsanitize=address, otherwise I get
    # tons of unresolved symbols
    # - asan should come first
    libraries = "-lasan -lm -lGL " + sdl2_libs
    includes = "-I" + source_directory
    defines = "-DCUTE_SHADER_SEARCH_PATH=\\\"shader/:cute3d/shader/\\\""

    cflags = features + " " + warnings + " " + errors + " " + linking + " " + sdl2_cflags + " " + optimization + " " + includes + " " + defines
    ldflags = linking + " " + libraries
elif build_toolset == "mingw":
    if ninja_cute3d.command_exists("sdl2-config") and ninja_cute3d.command_exists("bash"):
        sdl2_cflags = subprocess.check_output(["bash", "sdl2-config", "--cflags"]).rstrip()
        sdl2_libs = subprocess.check_output(["bash", "sdl2-config", "--libs"]).rstrip()
    else:
        print "could not find sdl2-config, trying to build with:"
        print "sdl2_cflags = \"-Ic:/MinGW/include/SDL2 -Dmain=SDL_main\""
        print "sdl2_libs = \"-Lc:/MinGW/lib -lmingw32 -lSDL2main -lSDL2 -mwindows\""
        sdl2_cflags = "-Ic:/MinGW/include/SDL2 -Dmain=SDL_main"
        sdl2_libs = "-Lc:/MinGW/lib -lSDL2main -lSDL2"

    features = "-posix -std=c11 -g -DDEBUG "
    optimization = "-O0" # "-flto=4 -march=native"
    warnings = "-Wall -Wmaybe-uninitialized -Wsign-conversion -Wno-missing-field-initializers -Wno-missing-braces -Wno-pedantic-ms-format -Wno-unknown-pragmas -pedantic"
    errors = "-Werror=implicit-function-declaration"
    linking = "-fPIC"
    libraries = sdl2_libs + " -lmingw32 -mwindows -mwindows -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lshell32 -luuid -lole32 -loleaut32 -limm32 -lwinmm -lversion -lm -lopengl32"
    includes = "-I" + source_directory
    defines = "-DCUTE_SHADER_SEARCH_PATH=\\\"shader/:cute3d/shader/\\\""

    cflags = features + " " + warnings + " " + errors + " " + linking + " " + sdl2_cflags + " " + optimization + " " + includes + " " + defines
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
    errors = ""
    linking = ""
    # - chkstk.obj for _alloca
    # - msvcurt.lib: Static library for the pure managed CRT.
    # - /ENTRY:main needs libucrt.lib
    # - /ENTRY:main made exe hang on exit
    libraries = "msvcrt.lib opengl32.lib chkstk.obj " + sdl2_libs
    includes = "/I" + source_directory
    defines = "/DCUTE_SHADER_SEARCH_PATH=\\\"shader/:cute3d/shader/\\\""

    # - /STACK seems to be a linker option only
    cflags = features + " " + warnings + " " + errors + " " + linking + " " + sdl2_cflags + " " + optimization + " " + includes + " " + defines
    ldflags = "/SUBSYSTEM:CONSOLE /STACK:8388608 " + linking + " " + libraries
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

# - generic rules for tools that I've put into ninja_cute3d to make them reusable elsewhere
ninja_cute3d.copy(w, build_platform)
ninja_cute3d.mkdir(w, build_platform)
ninja_cute3d.glsl_validate(w)
ninja_cute3d.xxd(w, source_directory)

# - I want to be able to use build_shaders in other projects to validate shaders and copy them where they belong
# - there is a lot of complexity hidden behind this one function call
shaders = ninja_cute3d.build_shaders(w, build_platform, source_directory, build_directory, script_directory, "shader")

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

# - the rest should be pretty straightforward, here we create compile and link rules depending on which toolset
# is selected and use the cflags and ldflags that we configured above
if build_toolset == "mingw" or build_toolset == "gcc":
    # - use gcc_compiler_color
    gcc_compiler_color_path = os.path.join(script_directory, "scripts", "gcc_compiler_color.py")
    # -c and /c in gcc and cl.exe mean: compile without linking
    w.rule(name="compile", command="python " + gcc_compiler_color_path + " gcc -MMD -MF $out.d -c $in -o $out " + cflags, deps="gcc", depfile="$out.d")
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
