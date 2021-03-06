# -*- coding: utf-8 -*-
import os.path
import string
import sys
import re

def xxd(file_path, name_prefix, null_terminate):
    array_name = os.path.split(file_path.replace('.', '_'))[1]
    array_name = name_prefix + array_name
    output = "unsigned char %s[] = {" % array_name
    with open(file_path, 'r') as f:
        length = 0
        while True:
            buf = f.read(12)

            if not buf:
                if not length % 12:
                    output += "\n  "
                output += "0x00"
                break
            else:
                output += "\n  "

            for i in buf:
                output += "0x%02x, " % ord(i)
                length += 1
        output += "\n};\n"
        output += "__attribute__ ((unused)) unsigned int %s_len = %d;\n" % (array_name, length)

    return output

if __name__ == '__main__':
    if not os.path.exists(sys.argv[1]):
        print >> (sys.stderr, "The input file doesn't exist.")
        sys.exit(1)

    if len(sys.argv) == 3:
        with open(sys.argv[2], 'w+') as f:
            f.write(xxd(sys.argv[1], "", True))
    elif len(sys.argv) == 4:
        with open(sys.argv[2], 'w+') as f:
            f.write(xxd(sys.argv[1], sys.argv[3], True))
    else:
        print xxd(sys.argv[1], "", True)
