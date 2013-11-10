cute3d: *.c
	gcc -g -std=c99 -fPIC io.c -c -o io.o
	gcc -g -std=c99 -fPIC matrix.c -c -o matrix.o
	gcc -g -std=c99 -fPIC quaternion.c -c -o quaternion.o
	gcc -g -std=c99 -fPIC transform.c -c -o transform.o
	gcc -g -std=c99 -fPIC ascii.c -c -o ascii.o
	gcc -g -std=c99 -fPIC grid.c -c -o grid.o
	gcc -g -std=c99 -fPIC world.c -c -o world.o
	gcc -g -std=c99 -fPIC geometry.c -c -o geometry.o -lGL -lGLEW
	gcc -g -std=c99 -fPIC shader.c -c -o shader.o -lGL -lGLEW
	gcc -g -std=c99 -fPIC render.c -c -o render.o -lGL -lGLEW
	gcc -g -std=c99 -fPIC solid.c -c -o solid.o -lGL -lGLEW
	gcc -g -std=c99 -fPIC glsl.c -c -o glsl.o -lGL -lGLEW
	gcc -g -std=c99 -fPIC draw.c -c -o draw.o -lGL -lGLEW
	gcc -g -std=c99 -fPIC text.c -c -o text.o -lGL -lGLEW
	gcc -g -std=c99 -fPIC allegro.c -c -o allegro.o -lallegro -lallegro_main -lallegro_video -lGL -lGLEW -lglut
	gcc -g -std=c99 -fPIC cute3d.c -o cute3d *.o -lm -lallegro -lallegro_main -lallegro_video -lGL -lGLEW -lglut

clean:
	rm *.o

grid: tests/grid-test.c cute3d
	gcc -g -std=c99 -fPIC -I. tests/grid-test.c -o grid *.o -lm -lallegro -lallegro_main -lallegro_video -lGL -lGLEW -lglut

io: tests/io-test.c cute3d
	gcc -g -std=c99 -fPIC -I. test/io-test.c -o io -lm -lallegro -lallegro_main -lallegro_video -lGL -lGLEW -lglut

world: tests/world-test.c cute3d
	gcc -g -std=c99 -fPIC -I. tests/world-test.c -o world *.o -lm -lallegro -lallegro_main -lallegro_video -lGL -lGLEW -lglut
