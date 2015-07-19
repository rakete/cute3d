cute3d: *.c
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC io.c -c -o io.o
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC ogl.c -c -o ogl.o
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC camera.c -c -o camera.o
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC matrix.c -c -o matrix.o
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC quaternion.c -c -o quaternion.o
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC transform.c -c -o transform.o
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC ascii.c -c -o ascii.o
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC grid.c -c -o grid.o
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC vbo.c -c -o vbo.o -lGL -lGLEW
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC shader.c -c -o shader.o -lGL -lGLEW
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC render.c -c -o render.o -lGL -lGLEW
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC solid.c -c -o solid.o -lGL -lGLEW
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC glsl.c -c -o glsl.o -lGL -lGLEW
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC draw.c -c -o draw.o -lGL -lGLEW
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC text.c -c -o text.o -lGL -lGLEW
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC physics.c -c -o physics.o
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC collisions.c -c -o collisions.o
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC gametime.c -c -o gametime.o
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC halfedge.c -c -o halfedge.o
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC sdl2.c -c -o sdl2.o -lSDL2 -lSDL2main -lGL -lGLEW
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC cute3d.c -o cute3d *.o -lm -lSDL2 -lSDL2main -lGL -lGLEW -lglut

clean:
	rm *.o cute3d grid io world solid text physics

grid: tests/test-grid.c cute3d
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC -I. tests/test-grid.c -o grid *.o -lm -lSDL2 -lSDL2main -lGL -lGLEW -lglut

io: tests/test-io.c cute3d
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC -I. test/test-io.c -o io -lm

world: tests/test-world.c cute3d
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC -I. tests/test-world.c -o world *.o -lm -lSDL2 -lSDL2main -lGL -lGLEW -lglut

solid: tests/test-solid.c cute3d
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC -I. tests/test-solid.c -o solid *.o -lm -lSDL2 -lSDL2main -lGL -lGLEW -lglut

text: tests/test-text.c cute3d
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC -I. tests/test-text.c -o text *.o -lm -lSDL2 -lSDL2main -lGL -lGLEW -lglut

physics: tests/test-physics.c cute3d
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC -I. tests/test-physics.c -o physics *.o -lm -lSDL2 -lSDL2main -lGL -lGLEW

halfedge: tests/test-halfedge.c cute3d
	gcc -std=c99 -Wall -pedantic -g -DDEBUG -std=c99 -fPIC -I. tests/test-halfedge.c -o halfedge *.o -lm -lSDL2 -lSDL2main -lGL -lGLEW
