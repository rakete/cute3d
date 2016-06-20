import ninja_syntax
import glob
import platform
import subprocess
import sys
import os

script_directory = os.path.dirname(os.path.realpath(__file__))
current_directory = os.getcwd()

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
linking = ""
libraries = ""
includes = ""
cflags = ""
ldflags = ""
if build_platform == "linux" or build_toolset == "gcc":
    sdl2_cflags = subprocess.check_output(["bash", "sdl2-config", "--cflags"]).rstrip()
    sdl2_libs = subprocess.check_output(["bash", "sdl2-config", "--libs"]).rstrip()

    features = "-std=c99 -pg -DDEBUG"
    optimization = "-flto=4 -march=native"
    warnings = "-Wall -Wmaybe-uninitialized -Wsign-conversion -Wno-missing-field-initializers -Wno-missing-braces -pedantic"
    linking = "-fPIC"
    libraries = "-lm -lGL " + sdl2_libs
    includes = "-I" + source_directory

    cflags = features + " " + warnings + " " + linking + " " + sdl2_cflags + " " + optimization + " " + includes
    ldflags = linking + " " + libraries
elif build_toolset == "mingw":
    sdl2_cflags = "-Ic:/MinGW/include/SDL2 -Dmain=SDL_main"
    sdl2_libs = "-Lc:/MinGW/lib -lmingw32 -lSDL2main -lSDL2 -mwindows"

    features = "-std=c99 -pg -DDEBUG"
    optimization = "-flto=4 -march=native"
    warnings = "-Wall -Wmaybe-uninitialized -Wsign-conversion -Wno-missing-field-initializers -Wno-missing-braces -Wno-pedantic-ms-format -pedantic"
    linking = "-fPIC"
    libraries = "-lole32 -loleaut32 -limm32 -lwinmm -lversion -lm -lopengl32 " + sdl2_libs
    includes = "-I" + source_directory

    cflags = features + " " + warnings + " " + linking + " " + sdl2_cflags + " " + optimization + " " + includes
    ldflags = linking + " " + libraries
elif build_toolset == "msvc":
    sdl2_cflags = "/Ic:\\VC\\SDL2-2.0.4\\include"
    sdl2_libs = "/LIBPATHc:\\VC\\SDL2-2.0.4\\lib"

    features = "/Zi /DDEBUG /DCUTE_BUILD_MSVC"
    optimization = "/Od"
    warnings = "/W4"
    linking = ""
    libraries = "" + sdl2_libs
    includes = "/I" + source_directory

    cflags = features + " " + warnings + " " + linking + " " + sdl2_cflags + " " + optimization + " " + includes
    ldflags = linking + " " + libraries
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

f = open("build.ninja", "w+")
w = ninja_syntax.Writer(f, 127)

w.variable("basefilename", "\"$$in\"")
w.newline()

def command_exists(cmd):
    return any(
        os.access(os.path.join(path, cmd), os.X_OK) or
        os.access(os.path.join(path, cmd + ".exe"), os.X_OK)
        for path in os.environ["PATH"].split(os.pathsep)
    )

if build_platform == "windows":
    w.rule(name="copy", command="cmd /c copy.exe $in $out >nul")
else:
    w.rule(name="copy", command="cp $in $out")
w.newline()

if build_platform == "windows":
    os.chdir(source_directory)
    dlls = glob.glob("*.dll")
    os.chdir(current_directory)

    for d in dlls:
        w.build(d, "copy", os.path.join(source_directory, d))
    w.newline()

shader_directory = os.path.relpath(os.path.join(source_directory, "shader") , current_directory)

if build_platform == "windows":
    w.rule(name="make_shader_directory", command="cmd /c mkdir shader & copy NUL $out")
else:
    w.rule(name="make_shader_directory", command="mkdir -p shader; touch $out")
w.newline()

w.build("shader\directory", "make_shader_directory")
w.newline()

if command_exists("glsl-validate.py"):
    if build_platform == "windows":
        w.rule(name="copy_shader", command="cmd /c glsl-validate.py $in & for %I in ($in) do copy %I shader >nul")
    else:
        w.rule(name="copy_shader", command="bash -c \"glsl-validate.py $in; cp $in shader\"")
else:
    if build_platform == "windows":
        w.rule(name="copy_shader", command="cmd /c for %I in ($in) do copy %I shader >nul")
    else:
        w.rule(name="copy_shader", command="bash -c \"cp $in shader\"")
w.newline()

def pairwise(it):
    it = iter(it)
    while True:
        yield next(it), next(it)

os.chdir(shader_directory)
shader_filenames = sorted(glob.glob("*.[frag|vert]*"))
os.chdir(current_directory)
shaders = []
for frag_shader_filename, vert_shader_filename in pairwise(shader_filenames):
    vert_shader = os.path.join("shader", vert_shader_filename)
    frag_shader = os.path.join("shader", frag_shader_filename)
    w.build(vert_shader, "copy_shader", os.path.join(shader_directory, vert_shader_filename), "shader\directory")
    w.build(frag_shader, "copy_shader", os.path.join(shader_directory, frag_shader_filename), "shader\directory")
    shaders.append(vert_shader)
    shaders.append(frag_shader)
w.newline()

if build_toolset == "mingw" or build_toolset == "gcc":
    # -c and /c in gcc and cl.exe mean: compile without linking
    w.rule(name="compile", command="gcc -MMD -MF $out.d -c $in -o $out -DBASE_FILE_NAME=\"$basefilename\" " + cflags, deps="gcc", depfile="$out.d")
    w.newline()
    w.rule(name="link", command="gcc $in -o $out " + ldflags)
    w.newline()
elif build_toolset == "msvc":
    # - needs /FS to enable synchronous writes to pdb database
    w.rule(name="compile", command="cl.exe /showIncludes /FS /c $in /Fo$out " + cflags, deps="msvc")
    w.newline()

os.chdir(source_directory)
sources = glob.glob("*.c")
sources.remove("driver_allegro.c")
os.chdir(current_directory)

objects = []
for c in sources:
    o = c.replace(".c", ".o")
    w.build(o, "compile", os.path.join(source_directory, c))
    w.variable("  basefilename", c)
    objects.append(o)
w.newline()

w.default(objects)
w.newline()

tests_directory = os.path.relpath(os.path.join(source_directory, "tests") , current_directory)

os.chdir(tests_directory)
test_sources = glob.glob("*.c")
os.chdir(current_directory)
for c in test_sources:
    o = c.replace(".c", ".o")
    w.build(o, "compile", os.path.join(tests_directory, c))
    w.variable("  basefilename", os.path.join("tests", c))

    exe = c.replace(".c", "")
    if build_platform == "windows":
        winexe  = exe + ".exe"
        w.build(exe, "phony", winexe)
        exe = winexe
    w.build(exe, "link", [o] + objects, dlls + shaders)
    w.newline()
