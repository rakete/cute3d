//CUTE_FRAGMENT

// Cute3D, a simple opengl based framework for writing interactive realtime applications

// Copyright (C) 2013-2017 Andreas Raster

// This file is part of Cute3D.

// Cute3D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// Cute3D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with Cute3D.  If not, see <http://www.gnu.org/licenses/>.

uniform sampler2D diffuse_texture;

uniform vec4 ambient_light;

uniform int enable_texture;

shader_in vec2 frag_texcoord;
shader_in vec4 frag_color;
shader_in float intensity;

void main() {
    // - instead of using an if, I could do this:
    //gl_FragColor = mix(frag_color, texture2D(diffuse_texture, vec2(frag_texcoord.x,frag_texcoord.y)), float(enable_texture > 0));

    gl_FragColor = mix( mix( vec4(ambient_light[3]*frag_color[0], ambient_light[3]*frag_color[1], ambient_light[3]*frag_color[2], frag_color[3]),
                             vec4(ambient_light[0], ambient_light[1], ambient_light[2], frag_color[3]),
                             ambient_light[3] ),
                        frag_color,
                        intensity );
}
