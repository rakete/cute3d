import ninja_syntax
import glob
import platform
import subprocess
import sys
import os

def command_exists(cmd):
    return any(
        os.access(os.path.join(path, cmd), os.X_OK) or
        os.access(os.path.join(path, cmd + ".exe"), os.X_OK)
        for path in os.environ["PATH"].split(os.pathsep)
    )

def pairwise(it):
    it = iter(it)
    while True:
        yield next(it), next(it)

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
    if command_exists("sdl2-config") and command_exists("bash"):
        sdl2_cflags = subprocess.check_output(["bash", "sdl2-config", "--cflags"]).rstrip()
        sdl2_libs = subprocess.check_output(["bash", "sdl2-config", "--libs"]).rstrip()
    else:
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
    sdl2_libs = "/LIBPATH:c:\\VC\\SDL2-2.0.4\\lib\\x86 SDL2.lib SDL2main.lib"

    features = "/MD /Oi /Zi /DDEBUG /DCUTE_BUILD_MSVC"
    optimization = "/Od"
    # warning C4204: nonstandard extension used: non-constant aggregate initializer
    warnings = "/W4 /wd4204"
    linking = ""
    libraries = "msvcurt.lib libucrt.lib opengl32.lib chkstk.obj " + sdl2_libs
    includes = "/I" + source_directory

    cflags = features + " " + warnings + " " + linking + " " + sdl2_cflags + " " + optimization + " " + includes
    ldflags = "/SUBSYSTEM:CONSOLE /ENTRY:main" + linking + " " + libraries
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

build_file_handle = open("build.ninja", "w+")
w = ninja_syntax.Writer(build_file_handle, 127)

w.variable("filename", "\"$in\"")
w.newline()

cmdexe = "cmd"
if build_platform == "windows" and command_exists("cmdproxy"):
    cmdexe = "cmdproxy"

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
        w.rule(name="copy", command=cmdexe + " /c \"copy.exe $in $out >nul\"")
else:
    w.rule(name="copy", command="cp $in $out")
w.newline()

dlls = []
if build_platform == "windows" and os.path.relpath(build_directory, script_directory) != ".":
    os.chdir(source_directory)
    dlls = glob.glob("*.dll")
    os.chdir(current_directory)

    for d in dlls:
       w.build(d, "copy", os.path.join(source_directory, d))
    w.newline()

shader_directory = os.path.relpath(os.path.join(source_directory, "shader"), current_directory)

if build_platform == "windows":
    if command_exists("mkdir.exe"):
        w.rule(name="make_shader_directory", command="mkdir -p $out")
    else:
        w.rule(name="make_shader_directory", command=cmdexe + " /c \"if not exist shader mkdir shader\"")
else:
    w.rule(name="make_shader_directory", command="bash -c \"mkdir -p $out\"")
w.newline()

w.build("shader", "make_shader_directory")
w.newline()

prefix_shader = []
if command_exists("glsl-validate.py"):
    prefix_shader = [os.path.join(shader_directory, "prefix.vert"), os.path.join(shader_directory, "prefix.frag")]
    prefix_shader_string = " ".join(prefix_shader)

    if build_platform == "windows":
        if command_exists("cp.exe"):
            w.rule(name="validate_glsl", command=cmdexe + " /c \"glsl-validate.py " + prefix_shader_string + " $in && cp $in shader\"")
        else:
            w.rule(name="validate_glsl", command=cmdexe + " /c \"glsl-validate.py " + prefix_shader_string + " $in & for %I in ($in) do copy %I shader >nul\"")
    else:
        w.rule(name="validate_glsl", command="bash -c \"glsl-validate.py " + prefix_shader_string + " $in; cp $in shader\"")
else:
    if build_platform == "windows":
        if command_exists("cp.exe"):
            w.rule(name="validate_glsl", command="cp $in shader")
        else:
            w.rule(name="validate_glsl", command=cmdexe + " /c \"for %I in ($in) do copy %I shader >nul\"")
    else:
        w.rule(name="validate_glsl", command="bash -c \"cp $in shader\"")
w.newline()

shaders = []
if os.path.relpath(build_directory, script_directory) != ".":
    os.chdir(shader_directory)
    shader_filenames = []
    [shader_filenames.append(os.path.splitext(sf)[0]) for sf in glob.glob("*.vert")]
    [shader_filenames.append(os.path.splitext(sf)[0]) for sf in glob.glob("*.frag") if os.path.splitext(sf)[0] not in shader_filenames]

    os.chdir(current_directory)
    for shader_filename in shader_filenames:
        source_vert_shader = os.path.join(shader_directory, shader_filename + ".vert")
        source_frag_shader = os.path.join(shader_directory, shader_filename + ".frag")

        dest_shaders = []
        source_shaders = []
        if os.path.isfile(source_vert_shader):
            source_shaders.append(source_vert_shader)
            dest_vert_shader = os.path.join("shader", shader_filename + ".vert")
            dest_shaders.append(dest_vert_shader)
            w.build(dest_vert_shader, "validate_glsl", source_vert_shader)

        if os.path.isfile(source_frag_shader):
            source_shaders.append(source_frag_shader)
            dest_frag_shader = os.path.join("shader", shader_filename + ".frag")
            dest_shaders.append(dest_frag_shader)
            w.build(dest_frag_shader, "validate_glsl", source_frag_shader)

        deps = ["shader"]
        if shader_filename != "prefix":
            deps += [os.path.join("shader", "prefix.vert"), os.path.join("shader", "prefix.frag"), ]

        #w.build(dest_shaders, "validate_glsl", source_shaders, deps)

        shaders += dest_shaders
    w.newline()

if build_toolset == "mingw" or build_toolset == "gcc":
    # -c and /c in gcc and cl.exe mean: compile without linking
    w.rule(name="compile", command="gcc -MMD -MF $out.d -D__FILENAME__=\"\\\"$filename\\\"\" -c $in -o $out " + cflags, deps="gcc", depfile="$out.d")
    w.newline()
    w.rule(name="link", command="gcc $in -o $out " + ldflags)
    w.newline()
elif build_toolset == "msvc":
    # - needs /FS to enable synchronous writes to pdb database
    w.rule(name="compile", command="cl.exe /nologo /showIncludes /FS /D__FILENAME__=\"\\\"$filename\\\"\" /c $in /Fo$out " + cflags, deps="msvc")
    w.newline()
    w.rule(name="link", command="cl.exe /nologo $in /link " + ldflags + " /out:$out")
    w.newline()

os.chdir(source_directory)
sources = glob.glob("*.c")
sources.remove("driver_allegro.c")
os.chdir(current_directory)

objects = []
for c in sources:
    if build_toolset == "msvc":
        o = c.replace(".c", ".obj")
    else:
        o = c.replace(".c", ".o")
    w.build(o, "compile", os.path.join(source_directory, c))
    w.variable("filename", os.path.splitext(c)[0], indent=1)
    objects.append(o)
w.newline()

w.default(objects)
w.newline()

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
    w.variable("filename", os.path.join("tests", os.path.splitext(c)[0]), indent=1)

    binary = c.replace(".c", "")
    if build_platform == "windows":
        w.build(binary + ".exe", "link", [o] + objects)
        w.build(binary, "phony", binary + ".exe", dlls + shaders)
    else:
        w.build(binary + ".bin", "link", [o] + objects)
        w.build(binary, "phony", binary + ".bin", dlls + shaders)

    w.newline()
