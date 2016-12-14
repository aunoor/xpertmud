import sys
import re

from xmlparser import *
from helplayout import *

dtd = helpdtd()
allHelp = XMXmlNode('help')
allHelp.setAttribute('name', 'help')

for file in sys.argv:
    p = XMXmlParser(dtd)
    p.parseFile(file)
    for node in p.getNodes():
        for inner in node.getSubNodes():
            allHelp.addChild(inner)

mode = 'docbook'

if mode == "docbook":
    layout = helplayout('docbook')
    sys.stdout.write(layout.generate(allHelp))

