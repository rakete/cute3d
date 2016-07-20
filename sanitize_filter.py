import sys
import re

leakblock_lines = []
inside_leakblock = False
for line in sys.stdin:
    line = line.rstrip('\n')

    firstline_match = False
    if not inside_leakblock:
        firstline_match = re.match("^Indirect leak.*|^Direct leak.*", line)

    if firstline_match or inside_leakblock:
        inside_leakblock = True
        leakblock_lines.append(line)
    else:
        print line
        sys.stdout.flush()

    if len(line) == 0:
        suppress_error = False
        for errorline in leakblock_lines:
            lib_match = re.match(".*i965_dri\.so.*|.*libdrm_intel\.so.*|.*libGL\.so\.1.*|.*libc\.so\.6.*", errorline)
            if lib_match:
                suppress_error = True

        if not suppress_error:
            for errorline in leakblock_lines:
                print errorline
                sys.stdout.flush()

        leakblock_lines = []
        inside_leakblock = False
