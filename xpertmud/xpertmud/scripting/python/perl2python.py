#!/usr/bin/python
import sys
import re

line = sys.stdin.readline()
while(line != ""):
    line = re.sub(r' {', r':', line)
    line = re.sub(r'}\s*', r'', line)
    line = re.sub(r'->', r'.', line)
    line = re.sub(r'sub', r'def', line)
    line = re.sub(r'elsif', r'elif', line)
    line = re.sub(r'\$(\d)', r'reS.match.group(\1)', line)
    line = re.sub(r'\$(\w+)', r'\1', line)
    line = re.sub(r';$', r'', line)
    line = re.sub(r'\.=', r'+=', line)
    line = re.sub(r'^(\s*print\s+.*)\\n("\s*.*)$', r'\1\2', line)
    line = re.sub(r'\s+eq\s+', r' == ', line)
    line = re.sub(r'::', r'.', line)
    line = re.sub(r'&&', r'and', line)
    line = re.sub(r'\|\|', r'or', line)
    line = re.sub(r'length\(', r'len(', line)
    line = re.sub(r'my ', r'', line)
    line = re.sub(r'defined ', r'', line)
    line = re.sub(r'foreach', r'for', line)
    line = re.sub(r'(\s*)(.*[^\w\d_])([\w\d_]+)\s+=~\s+s/([^/]*)/([^/]*)/(.*)',
                  r"\1\2reS.perlSub(r'\4', r'\5', \3)\6\n\1\3 = reS.string", line)
    line = re.sub(r'/([^/]*)/',
                  r"reS.perlMatch(r'\1', line)", line)
    line = re.sub(r'push\(@(.*), ', r'\1.append(', line)
    

    sys.stdout.write(line)
    
    line = sys.stdin.readline()
