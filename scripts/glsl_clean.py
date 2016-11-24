#!/usr/bin/python

#  The MIT License (MIT)
#
# Copyright (c) 2013 Felix Palmer
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

import argparse
import os
import re

DIR = os.path.dirname(os.path.realpath(__file__))


def load_shader(shader_file):
    output = ""
    # Keep track of line numbers
    line_labels = []
    with open(shader_file, 'r') as f:
        line_num = 1
        for line in f:
            include_match = re.match("#include (.*)", line)
            if include_match:
                include_file = include_match.group(1)
                fullpath = os.path.join(os.path.dirname(shader_file),
                                        include_file)
                (included_shader, included_line_labels) = load_shader(fullpath)
                output += included_shader
                line_labels += included_line_labels
            else:
                output += line
                line_labels.append("%s:%d" % (shader_file, line_num))
            line_num += 1
    return (output, line_labels)


args = None


def validate_shader(shader_file):
    extension = os.path.splitext(shader_file)[1]

    # Load in actual shader
    (shader, line_labels) = load_shader(shader_file)

    # Check if marked as RawShader
    if not args.raw and "RawShader" not in shader:
        # Prepend the prefix shader unless we are in raw mode
        prefix_shader_file = os.path.join(DIR, "prefix/prefix%s" % extension)
        (prefix_shader, prefix_line_labels) = load_shader(prefix_shader_file)
        shader = prefix_shader + shader
        line_labels = prefix_line_labels + line_labels

    uniforms = []
    for line in shader.split("\n"):
        uniform_match = re.match("uniform .* (.*);", line)
        if uniform_match:
            uniforms.append(uniform_match.group(1))

    unused = []
    for uniform in uniforms:
        if uniform in ['modelMatrix',
                       'modelViewMatrix',
                       'normalMatrix',
                       'viewMatrix',
                       'cameraPosition']:
            continue
        if shader.count(uniform) is 1:
            unused.append(uniform)

    if len(unused) > 0:
        print "Unused uniforms in", shader_file
        for uniform in unused:
            print uniform
        print ""

def standalone():
    parser = argparse.ArgumentParser(description='Validate three.js shaders')
    parser.add_argument('files', metavar='FILE', type=str, nargs='+',
                        help='files to validate')
    parser.add_argument('--raw', dest='raw', action='store_true',
                        help='Do not prepend standard THREE.js prefix block (useful for RawShaderMaterial)')
    global args
    args = parser.parse_args()
    files = args.files
    bad_extensions = filter(lambda f: not re.match('^\.(vert|frag)$', os.path.splitext(f)[1]), files)
    for f in bad_extensions:
        print "Invalid file: %s, only support .frag and .vert files" % f
        exit(1)

    map(validate_shader, files)

if __name__ == "__main__":
    standalone()
