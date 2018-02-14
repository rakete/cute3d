# Cute3D

A framework for creating interactive realtime simulations in 3D. My goal is to create a library of functionality that makes it easy to quickly prototype and implement interactive 3D applications. That means I want to implement algorithms from various topics of game development, starting from simple rendering, ranging over acceleration structures for dealing with 3D geometry, collision detection and occlusion culling, all the way to physics simulation, animation and procedural generation.

I attempt to create something that compiles quickly and is portable. That is why I choose to use C as programming language and use very few external dependencies. Whenever I use OpenGL, I use only functionality that is available in OpenGL ES 2.0, so that porting Cute3D code to WebGL should be easy.

The code I write should ideally be easy to understand (easier said then done) and as a result easy to modify and adapt to specific use cases. To achieve this the framework is seperated into several modules, each of which is written so that it should be useable with the least amount of dependance on all the other modules of the framework. All necessary communication between modules is done with only basic data types and avoids relying on any complex data structure whenever possible. The algorithmic code has extensive commentary and uses only the low-level abstractions available in C, and together with the seperation into modules I hope this will make understanding the complex parts of the framework very straightforward.

<p align="center">
<img src="screenshots/test-cute_2016-12-16.png" width="250"/> <img src="screenshots/test-shading_2016-12-16.png" width="250"/> <img src="screenshots/test-collisions_2016-12-16.png" width="250"/>
<img src="screenshots/test-solid_2016-12-16.png" width="250"/> <img src="screenshots/test-lines_2016-12-16.png" width="250"/>
</p>

## Getting Started

The best way to get started with Cute3D in its current state is to just clone the repository, build the included tests and start experimenting with the source code directly:

    git clone https://github.com/rakete/cute3d.git
    cd cute3d

Cute3D tries to have as few dependencies as possible, it only needs SDL2, python, the ninja build system and a C99 compiler. If you are running Ubuntu Linux you can use the following command to install everything neccessary to build Cute3D:

    sudo apt-get install libsdl2-dev ninja-build python gcc

If you are using any other Linux distribution or Microsoft Windows, you have to make sure these dependencies are correctly installed and accessible from the shell in which you are attempting to build Cute3D.

Once you have everything neccessary installed you can build Cute3D. Make sure you are inside the cloned cute3d repository and execute the following commands:

    mkdir build
    cd build
    python ../ninja_cute3d.py

This should output a brief summary of build settings in your terminal. If you are building on Windows make sure the paths shown for the location of the SDL2 include headers and library files are correct! If these are not correct, you have to edit ninja_build.py to contain the correct paths and then run the last command again.

The python command should have generated a build.ninja file in your build/ directory. You should now be able to build the Cute3D files by just executing:

    ninja

Or to build Cute3D together with a few tests you can execute:

    ninja test-collisions test-cute test-halfedge test-lines test-shading test-solid

Which should produce executables in the current directory, named with the suffix .bin on Linux and .exe on Windows.

Run any of them to see Cute3D in action. Most tests have some basic camera controls. Holding left mouse button while moving the mouse rotates the camera, holding right mouse button while moving the mouse pans the camera and using the mouse wheel zooms the camera in and out.
