import ninja_syntax
import glob
import platform
import subprocess
import sys
import os

script_directory = os.path.dirname(os.path.realpath(__file__))
current_directory = os.getcwd()

common_prefix = os.path.commonprefix([script_directory, current_directory])
source_directory = os.path.relpath(script_directory, current_directory)
build_directory = current_directory

build_platform = platform.system().lower()
build_architecture = platform.machine().lower()

build_toolset = "gcc"
if build_platform == "windows":
    build_toolset = "mingw"

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

print "source_directory: " + source_directory
print "build_directory: " + build_directory
print "build_platform: " + build_platform
print "build_architecture: " + build_architecture + " (not used yet)"
print "build_toolset: " + build_toolset

sdl2_cflags = ""
sdl2_libs = ""
features = ""
optimization = ""
warnings = ""
cflags = ""
ldflags = ""
if build_platform == "linux" or build_toolset == "gcc":
    sdl2_cflags = subprocess.check_output(["bash", "sdl2-config", "--cflags"]).rstrip()
    sdl2_libs = subprocess.check_output(["bash", "sdl2-config", "--libs"]).rstrip()

    features = "-pg -DDEBUG"
    optimization = "-fPIC -flto=4 -march=native"
    warnings = "-Wall -Wmaybe-uninitialized -Wsign-conversion -Wno-missing-field-initializers -Wno-missing-braces -pedantic"

    cflags = "-std=c99 " + warnings + " " + features + " " + sdl2_cflags + " " + optimization
    ldflags = "-lm -lGL" + sdl2_libs
elif build_toolset == "mingw":
    sdl2_cflags = "-Ic:/MinGW/include/SDL2 -Dmain=SDL_main"
    sdl2_libs = "-Lc:/MinGW/lib -lmingw32 -lSDL2main -lSDL2 -mwindows"

    features = "-pg -DDEBUG"
    optimization = "-fPIC -flto=4 -march=native"
    warnings = "-Wall -Wmaybe-uninitialized -Wsign-conversion -Wno-missing-field-initializers -Wno-missing-braces -pedantic"

    cflags = "-std=c99 " + warnings + " " + features + " " + sdl2_cflags + " " + optimization
    ldflags = "-lole32 -loleaut32 -limm32 -lwinmm -lversion -lm -lopengl32 " + sdl2_libs

elif build_toolset == "msvc":
    sdl2_cflags = "/Ic:\\VC\\SDL2-2.0.4\\include"
    sdl2_libs = "/LIBPATHc:\\VC\\SDL2-2.0.4\\lib"

    features = "/Zi /DDEBUG /DCUTE_BUILD_MSVC"
    optimization = "/Od"
    warnings = "/W4"

    cflags = warnings + " " + features + " " + sdl2_cflags + " " + optimization
    ldflags = "" + sdl2_libs
else:
    print "building with " + build_toolset + " on " + build_platform + " is not supported yet."
    sys.exit(1)
print "sdl2_cflags: " + sdl2_cflags
print "sdl2_libs: " + sdl2_libs
print "features: " + features
print "optimization: " + optimization
print "warnings: " + warnings
print "cflags: " + cflags
print "ldflags: " + ldflags

f = open("build.ninja","w+")
w = ninja_syntax.Writer(f, 127)

if build_toolset == "mingw" or build_toolset == "gcc":
    # -c and /c in gcc and cl.exe mean: compile without linking
    w.rule(name="cc", command="gcc -MMD -MF $out.d -c $in -o $out " + cflags, deps="gcc", depfile="$out.d")
elif build_toolset == "msvc":
    # - needs /FS to enable synchronous writes to pdb database
    w.rule(name="cc", command="cl.exe /showIncludes /FS /c $in /Fo$out " + cflags, deps="msvc")
w.newline()

os.chdir(source_directory)
sources = glob.glob("*.c")
sources.remove("driver_allegro.c")
os.chdir(current_directory)

for input in sources:
    output = input.replace(".c", ".o")
    w.build(output, "cc", os.path.join(source_directory, input))
w.newline()
