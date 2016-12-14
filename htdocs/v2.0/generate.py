#!/usr/bin/python
import sys
import re

fn = sys.argv[1]
uppern = sys.argv[2]
lowern = sys.argv[3]
title = sys.argv[4]

print "file name : " + fn
print "upper     : " + uppern
print "lower     : " + lowern
print "title     : " + title

middlen = re.sub(r'\.html', r'-m.html', fn)

print "middle    : " + middlen

f = open(fn, 'w')

middle = open(middlen, 'r')
line = middle.readline()
while line != "":
    if re.match(r'^<!-- TITLE -->', line) != None:
        print "writing title ..."
        f.write("<title>" + title + "</title>\n")
    elif re.match(r'^<!-- UPPER -->', line) != None:
        print "writing upper ..."
        upper = open(uppern, 'r')
        uline = upper.readline()
        while uline != "":
            m = re.match(r'^//(([^/#]+)(#[^/]*)?)//(.*)', uline)
            if m != None:
                if m.group(2) == fn:
                    f.write("<span class=\"selected\">" +
                                     m.group(4) + "</span><br />\n")
                else:
                    f.write("<a href=\"" + m.group(1) +
                                     "\">" + m.group(4) +
                                     "</a><br />\n")
            else:
                f.write(uline)
            uline = upper.readline()
        upper.close()
    elif re.match(r'^<!-- LOWER -->', line) != None:
        print "writing lower ..."
        lower = open(lowern, 'r')
        lline = lower.readline()
        while lline != "":
            f.write(lline)
            lline = lower.readline()
        lower.close()
    else:
        f.write(line)
    line = middle.readline()

middle.close()
f.close()


