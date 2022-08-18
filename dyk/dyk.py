#!/usr/bin/env python3

import random
import subprocess
import re

whitelist = [
    'ls',
    'grep',
    'awk',
    'sed'
]

def main():
    prog = "ls"
    prog = random.choice(whitelist)
    prog_out = subprocess.run([prog, '--help'], capture_output=True, text=True).stdout
    man_out = subprocess.run(['man', prog], capture_output=True, text=True).stdout

    man_lines = man_out.split("\n")
    assert(man_lines[2] == "NAME")
    print(man_lines[3])

    options = []
    for (li, line) in enumerate(man_lines):
        if re.match(r'^       -.*', line):
            option = man_lines[li] + "\n"
            #print(man_lines[li])
            li2 = li+1
            while (man_lines[li2] != ''):
                #print(man_lines[li2])
                option += man_lines[li2] + "\n"
                li2 += 1
            options.append(option)

    print("Did you know...")
    print("for the program %s" % prog)
    print(random.choice(options))

if __name__ == "__main__":
    main()
