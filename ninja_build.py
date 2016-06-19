import ninja_syntax
import glob
import platform
import subprocess
import sys

f = open("build.ninja","w+")
w = ninja_syntax.Writer(f, 128)

build_platform = platform.system().lower()
build_toolset = "gcc"
build_architecture = platform.machine().lower()

if len(sys.argv) > 1:
    user_toolset = sys.argv[1].lower()
    if user_toolset == "gcc" or user_toolset == "clang" or user_toolset == "mingw":
        build_toolset = user_toolset
    if user_toolset == "msvc" and build_platform == "windows":
        build_toolset = user_toolset

print "build_platform: " + build_platform
print "build_toolset: " + build_toolset
print "build_architecture: " + build_architecture

sdl2_cflags = ""
sdl2_libs = ""
if build_platform == "linux":
    sdl2_cflags = subprocess.check_output(["bash", "sdl2-config", "--cflags"]).rstrip()
    sdl2_libs = subprocess.check_output(["bash", "sdl2-config", "--libs"]).rstrip()
elif build_toolset == "mingw":
    sdl2_cflags = ""
    sdl2_libs = ""
elif build_toolset == "msvc":
    sdl2_cflags = ""
    sdl2_libs = ""
print "sdl2_cflags: " + sdl2_cflags
print "sdl2_libs: " + sdl2_libs

features = "-pg -DDEBUG"
optimization = "-fPIC -flto=4 -march=native"
warnings = "-Wall -Wmaybe-uninitialized -Wsign-conversion -Wno-missing-field-initializers -Wno-missing-braces -pedantic"
print "features: " + features
print "optimization: " + optimization
print "warnings: " + warnings

cflags = "-std=c99" + " " + warnings + " " + features + " " + sdl2_cflags + " " + optimization
ldflags = "-lm -lGL" + sdl2_libs
print "cflags: " + cflags
print "ldflags: " + ldflags

w.rule("cc", "gcc -c $in -o $out " + cflags + " " + ldflags)
w.newline()

sources = glob.glob("*.c")
sources.remove("driver_allegro.c")

for input in sources:
    output = input.replace(".c", ".o")
    w.build(output, "cc", input)
w.newline()
