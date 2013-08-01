cute3d: geometry.c cute3d.c io.c matrix.c transform.c glsl.c quaternion.c shader.c render.c solid.c text.c ascii.c
	gcc -g -std=c99 -fPIC io.c -c -o io.o
	gcc -g -std=c99 -fPIC matrix.c -c -o matrix.o
	gcc -g -std=c99 -fPIC quaternion.c -c -o quaternion.o
	gcc -g -std=c99 -fPIC transform.c -c -o transform.o
	gcc -g -std=c99 -fPIC ascii.c -c -o ascii.o
	gcc -g -std=c99 -fPIC geometry.c -c -o geometry.o -lGL -lGLEW
	gcc -g -std=c99 -fPIC shader.c -c -o shader.o -lGL -lGLEW
	gcc -g -std=c99 -fPIC render.c -c -o render.o -lGL -lGLEW
	gcc -g -std=c99 -fPIC solid.c -c -o solid.o -lGL -lGLEW
	gcc -g -std=c99 -fPIC glsl.c -c -o glsl.o -lGL -lGLEW
	gcc -g -std=c99 -fPIC draw.c -c -o draw.o -lGL -lGLEW
	gcc -g -std=c99 -fPIC text.c -c -o text.o -lGL -lGLEW
	gcc -g -std=c99 -fPIC cute3d.c -o cute3d draw.o text.o ascii.o solid.o render.o geometry.o shader.o transform.o matrix.o quaternion.o glsl.o io.o -lallegro -lallegro_main -lallegro_video -lGL -lGLEW -lglut

grid: grid.c
	gcc -g -std=c99 -fPIC grid.c -o grid
