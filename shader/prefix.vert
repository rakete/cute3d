#version 100

precision highp float;

#if __VERSION__ == 100
#define shader_in attribute
#define shader_out varying
#define smooth
#endif

#if __VERSION__ == 130
#define shader_in in
#define shader_out out
#endif

#if __VERSION__ == 300
#define shader_in in
#define shader_out out
#endif
