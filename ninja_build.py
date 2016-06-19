import ninja_syntax
import glob

f = open("build.ninja","w+")
w = ninja_syntax.Writer(f, 256)

w.variable("warnings", "-Wall")
w.newline()

w.rule("gcc", "gcc $warnings -c $in -o $out")
w.newline()

sources = glob.glob("*.c")
sources.remove("driver_allegro.c")

for input in sources:
    output = input.replace(".c", ".o")
    w.build(output, "gcc", input)
w.newline()
