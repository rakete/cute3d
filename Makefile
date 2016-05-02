glsl_validator=glsl-validate.py
glsl_validator_exists := $(shell $(glsl_validator) -h)

src = $(filter-out driver_allegro.c, $(wildcard *.c))
obj = $(src:.c=.o)
tests_src = $(wildcard tests/*.c)
tests_bin = $(tests_src:tests/%.c=%)

glsl_vert_shader = $(filter-out shader/prefix.vert, $(wildcard shader/*.vert))
glsl_frag_shader = $(filter-out shader/prefix.frag, $(wildcard shader/*.frag))

# eventually I'll have to deal with all those vla's I have been allocating on the stack: -Wstack-usage=100000
FEATURES=-pg -DDEBUG `sdl2-config --cflags` # -DCUTE_BUILD_ES2
WARNINGS=-Wall -Wmaybe-uninitialized -Wsign-conversion -Wno-missing-field-initializers -Wno-missing-braces -pedantic
ifeq ($(OS), Windows_NT)
	OPTIMIZATION=-flto=4 -march=native
	LDFLAGS=-lm -lopengl32 `sdl2-config --libs`
	CC=gcc
	CFLAGS=-std=c99 $(WARNINGS) $(FEATURES) $(OPTIMIZATION)
else
	OPTIMIZATION=-fPIC -flto=4 -march=native
	LDFLAGS=-lm -lGL `sdl2-config --libs`
	CC=gcc-5
	CFLAGS=-std=c99 $(WARNINGS) $(FEATURES) $(OPTIMIZATION)
endif

validate-glsl:
ifndef glsl_validator_exists
	@echo "no glsl validator found, not validating any glsl code"
else
	$(glsl_validator) $(glsl_vert_shader) $(glsl_frag_shader)
endif

# the default that make defines for every .c file is enough to compile all cute3d sources into .o's
cute3d: validate-glsl $(obj)

# generate a compile rule for each test- target, when I enter make test-cute, this should generate the rule:
# test-cute: cute3d
# 	cc -std=c99 -Wall -pedantic -g -DDEBUG -fPIC -I. tests/test-cute.c -o test-cute *.o -lm -lSDL2 -lSDL2main -lGL -lGLEW
test-%: cute3d
	$(CC) $(CFLAGS) -I. tests/$@.c -o $@ $(obj) $(LDFLAGS)

# this is dependency tracking implemented from http://nuclear.mutantstargoat.com/articles/make/
# the cc line outputs a single makefile rule for each .c file, like so:
# ~/c/cute3d:master> cc gui_font.c -MM -MT gui_font.o
# gui_font.o: gui_font.c gui_font.h math_types.h render_ogl.h render_glsl.h math_matrix.h math_quaternion.h cute_io.h
dep = $(obj:.o=.d)
-include $(dep)
%.d: %.c
	@$(CC) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

.PHONY: clean
clean:
	rm -f $(obj)
	rm -f $(dep)
	rm -f $(tests_bin)
