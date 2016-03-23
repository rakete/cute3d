#version 100

precision highp float;

#if __VERSION__ == 100
#define shader_in varying
#define shader_out varying
#define smooth
#endif

#if __VERSION__ == 130
#define shader_in in
#define shader_out out
#define texture2D texture
#define texture3D texture
#define texture2DRect texture
#define textureCube texture
#endif

#if __VERSION__ == 300
#define shader_in in
#define shader_out out
#endif
