engine: *.c
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC cute_io.c -c -o cute_io.o
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC cute_sdl2.c -c -o cute_sdl2.o -lSDL2 -lSDL2main -lGL -lGLEW
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC math_matrix.c -c -o math_matrix.o
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC math_quaternion.c -c -o math_quaternion.o
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC math_transform.c -c -o math_transform.o
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC render_ogl.c -c -o render_ogl.o
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC render_draw.c -c -o render_draw.o -lGL -lGLEW
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC render_camera.c -c -o render_camera.o
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC render_shader.c -c -o render_shader.o -lGL -lGLEW
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC render_glsl.c -c -o render_glsl.o -lGL -lGLEW
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC render.c -c -o render.o -lGL -lGLEW
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC gui_default_font.c -c -o gui_default_font.o
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC gui.c -c -o gui.o -lGL -lGLEW
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC geometry_grid.c -c -o geometry_grid.o
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC geometry_vbo.c -c -o geometry_vbo.o -lGL -lGLEW
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC geometry_solid.c -c -o geometry_solid.o -lGL -lGLEW
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC geometry_halfedgemesh.c -c -o geometry_halfedgemesh.o
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC physics.c -c -o physics.o
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC physics_collisions.c -c -o physics_collisions.o
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC physics_time.c -c -o physics_time.o

clean:
	rm *.o cute grid io world solid text physics

test-cute: tests/test-grid.c engine
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC -I. tests/test-cute.c -o tests/test-cute *.o -lm -lSDL2 -lSDL2main -lGL -lGLEW -lglut

test-grid: tests/test-grid.c engine
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC -I. tests/test-grid.c -o tests/test-grid *.o -lm -lSDL2 -lSDL2main -lGL -lGLEW -lglut

test-io: tests/test-io.c engine
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC -I. test/test-io.c -o tests/test-io -lm

test-world: tests/test-world.c engine
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC -I. tests/test-world.c -o tests/test-world *.o -lm -lSDL2 -lSDL2main -lGL -lGLEW -lglut

test-solid: tests/test-solid.c engine
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC -I. tests/test-solid.c -o tests/test-solid *.o -lm -lSDL2 -lSDL2main -lGL -lGLEW -lglut

test-text: tests/test-text.c engine
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC -I. tests/test-text.c -o tests/test-text *.o -lm -lSDL2 -lSDL2main -lGL -lGLEW -lglut

test-physics: tests/test-physics.c engine
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC -I. tests/test-physics.c -o tests/test-physics *.o -lm -lSDL2 -lSDL2main -lGL -lGLEW

test-halfedge: tests/test-halfedge.c engine
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC -I. tests/test-halfedge.c -o tests/test-halfedge *.o -lm -lSDL2 -lSDL2main -lGL -lGLEW
