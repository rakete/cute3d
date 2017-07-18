#!/usr/bin/env python2
#
# Copyright (c) 2009-2011, David Sveningsson <ext-gcc-color@sidvind.com>
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
#
#     * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
#     * Neither the name of the author nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
from __future__ import print_function

import sys, os, subprocess, traceback
import socket
from select import select
import platform

filter = True
if 'NOCOLOR' in os.environ:
    # don't filter if the variable is set but empty
    if os.environ['NOCOLOR'] == '':
        filter = False

    # don't filter if the variable is set to y*
    elif os.environ['NOCOLOR'][0] == 'y':
        filter = False

    # don't filter if the variable is set to true
    elif os.environ['NOCOLOR'] == 'true':
        filter = False

p = subprocess.Popen( args = sys.argv[1:],
                      stdout = subprocess.PIPE, stderr=subprocess.STDOUT,
                      cwd = os.getcwd(),
                      env = os.environ,
                      shell = False )

bold          = "\033[01m"
red_light     = "\033[00;31m"
red_bold      = "\033[01;31m"
magenta_light = "\033[00;35m"
magenta_bold  = "\033[01;35m"
yellow_light  = "\033[00;33m"
yellow_bold   = "\033[01;33m"
reset         = "\033[0m"

while True:
    p.poll()
    if platform.system().lower() != "windows":
        r,w,x = select([p.stdout], [], [], 1.0)
        if len(r) == 0:
            continue

    line = p.stdout.readline()
    if len(line) == 0: # EOF
        if p.returncode is None:
            continue
        else:
            break
    line = line[:-1]

    if not filter:
        print(line)
        continue

    try:
        tokens = line.split()
        if len(tokens) < 2:
            print(line)
        elif tokens[1] in ['undefined']:
            lline = ' '.join(tokens[1:])
            print(bold + tokens[0], red_bold + lline + reset)
        elif tokens[1] in ['fel:', 'error:']: # error
            lline = ' '.join(tokens[2:])
            print(bold + tokens[0], tokens[1], red_bold + lline + reset)
        elif len(tokens) > 2 and tokens[2] in ['fel:', 'error:']: # fatal error
            lline = ' '.join(tokens[3:])
            print(bold + ' '.join(tokens[0:3]), red_bold + lline + reset)
        elif tokens[1] in ['varning:', 'warning:']:
            lline = ' '.join(tokens[2:])
            print(bold + tokens[0], tokens[1], yellow_bold + lline + reset)
        elif tokens[1] in ['In'] and tokens[2] in ['function']:
            lline = ' '.join(tokens[3:])
            print(tokens[0], tokens[1], tokens[2], magenta_bold + lline + reset)
        elif tokens[1] in ['In'] and tokens[2] in ['member'] and tokens[3] in ['function']:
            lline = ' '.join(tokens[4:])
            print(tokens[0], tokens[1], tokens[2], tokens[3], magenta_bold + lline + reset)
        else:
            print(line)
    except:
        traceback.print_exc()
        print(line)

sys.exit(p.returncode)
