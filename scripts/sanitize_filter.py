from __future__ import print_function

import sys
import re
import os
import subprocess
import platform
from select import select

p = subprocess.Popen( args = sys.argv[1:],
                      stdin = subprocess.PIPE,
                      stdout = subprocess.PIPE,
                      stderr = subprocess.STDOUT,
                      cwd = os.getcwd(),
                      env = os.environ,
                      universal_newlines=False )

leakblock_lines = []
inside_leakblock = False
count_suppressed = 0
while p.poll() is None:
    if platform.system().lower() != "windows":
        r,w,x = select([p.stdout], [], [], 1.0)
        if len(r) == 0:
            continue

    line = p.stdout.readline()
    line = line.rstrip('\n')

    if re.match("^SUMMARY: AddressSanitizer.*", line):
        print("Suppressed " + str(count_suppressed) + " leak sanitizer messages.\n")

    firstline_match = False
    if not inside_leakblock:
        firstline_match = re.match("^Indirect leak.*|^Direct leak.*", line)

    if firstline_match or inside_leakblock:
        inside_leakblock = True
        leakblock_lines.append(line)
    else:
        print(line)
        sys.stdout.flush()

    if len(line) == 0:
        suppress_error = False
        for errorline in leakblock_lines:
            lib_match = re.match(".*i965_dri\.so.*|.*libdrm_intel\.so.*|.*libGL\.so\.1.*|.*libc\.so\.6.*|.*libGLX_nvidia\.so\.0.*", errorline)
            if lib_match:
                suppress_error = True
                count_suppressed += 1

        if not suppress_error:
            for errorline in leakblock_lines:
                print(errorline)
                sys.stdout.flush()


        leakblock_lines = []
        inside_leakblock = False
