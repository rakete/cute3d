cute3d: geometry.c cute3d.c io.c matrix.c transform.c debug.c quaternion.c shader.c render.c
	gcc -g -std=c99 -fPIC io.c -c -o io.o
	gcc -g -std=c99 -fPIC debug.c -c -o debug.o
	gcc -g -std=c99 -fPIC matrix.c -c -o matrix.o
	gcc -g -std=c99 -fPIC quaternion.c -c -o quaternion.o
	gcc -g -std=c99 -fPIC transform.c -c -o transform.o
	gcc -g -std=c99 -fPIC geometry.c -c -o geometry.o -lGL -lGLEW
	gcc -g -std=c99 -fPIC shader.c -c -o shader.o -lGL -lGLEW
	gcc -g -std=c99 -fPIC render.c -c -o render.o -lGL -lGLEW
	gcc -g -std=c99 -fPIC cute3d.c -o cute3d render.o geometry.o shader.o transform.o matrix.o quaternion.o debug.o io.o -lallegro -lallegro_main -lallegro_video -lGL -lGLEW -lglut
