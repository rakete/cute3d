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

shader_in vec4 frag_color;
shader_in vec2 frag_texcoord;

uniform float line_attenuation;

void main() {
    if( length(frag_texcoord) > line_attenuation ) {
        // - eh, this is probably much more complex then neccessary, that the line transparency goes from
        // fully opaque at the threshold to fully transparent at 1.0, so if threshold is 0.6, then the
        // transparent section fades gradually from 0.6 to 1.0
        // - the length(frag_texcoord) in there is a measurement how far along the lines thickness/2.0 we are,
        // for this current fragment, should be max 1.0 everywhere when we use half circles at the endpoints
        float a = (1.0/(1.0 - line_attenuation)) * ((1.0 - line_attenuation) - (length(frag_texcoord) - line_attenuation));
        gl_FragColor = vec4(frag_color.xyz, a);
    } else {
        gl_FragColor = frag_color;
    }
}
