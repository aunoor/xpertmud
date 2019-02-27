import re
from xmlparser import * #FIXED
from ansicolors import * #FIXED

class helpdtd (XMXmlDTD):
    def __init__(self):
        XMXmlDTD.__init__(self)
        
        example = XMXmlDTDTag('example')
        example.addAttribute('xml:space', 'preserve')
        self.addTag(example)
        
        code = XMXmlDTDTag('code')
        code.addAttribute('xml:space', 'preserve')
        self.addTag(code)

        for langId in [ "py", "ru", "pe" ]:
            idtag = XMXmlDTDTag(langId)
            idtag.addAttribute('xml:space', 'preserve')
            self.addTag(idtag)

class helplayout:
    def __init__(self, mode):
        self.lang = "python"
        self.Lang = "Python"
        self.langID = "py"
        self.allIDs = [ "py", "ru", "pe" ]
        self.mode = mode
        self.plainHeadingColor = ansicolors.code('cb') + ansicolors.code('ch')
        self.plainUnderlineColor = ansicolors.code('cb')
        self.plainSectionColor = ansicolors.code('cc')
        self.plainArgColor = ansicolors.code('cr') + ansicolors.code('ch')
        self.plainCodeColor = ansicolors.code('cx') + ansicolors.code('ch')
        self.plainSpace = 1
        self.subHelpText = u""

    def setSubHelpText(self, text):
        self.subHelpText = text
    
    def escapeId(self, string):
        ret = string
        if self.mode == 'docbook':
            ret = re.subn(' ', '_', ret)[0]
            ret = re.subn('#', 'Sharp', ret)[0]
            ret = re.subn('%', 'Percent', ret)[0]
        return ret

    def outputSynopsis(self, node):
        synopsis = u""
        if self.mode == 'docbook':
            synopsis += (u"<para><programlisting>")
            synopsis += "Synopsis: " + node.getAttribute('name') + "("
        elif self.mode == 'plain':
            synopsis += self.plainSectionColor
            synopsis += (u"Synopsis:" + ansicolors.end() + u" " +
                         self.plainCodeColor + node.getAttribute('name') + "(")
            
        for arg in node.getSubNodes("arg"):
            synopsis += arg.escape(arg.getAttribute('name')) + ", "
        if synopsis[-1] == " ":
            synopsis = synopsis[:-2]
        synopsis += ")\n"
        if self.mode == 'docbook':
            synopsis += "</programlisting></para>"
        elif self.mode == 'plain':
            synopsis += ansicolors.end()
        return synopsis

    def outputArgs(self, node, prefix, call, depth):
        foundArgs = 0
        s = u""
        if self.mode == 'docbook':
            s += """
    <table>
      <title>Arguments</title>
      <tgroup cols="2">
        <tbody>"""
        elif self.mode == 'plain':
            s += "\n" + self.plainSectionColor + "Arguments:\n" + ansicolors.end()
        for arg in node.getSubNodes('arg'):
            if len(arg.getChilds()) > 0:
                if self.mode == 'docbook':
                    s += """
          <row>
            <entry>%s</entry>
            <entry>""" % arg.getAttribute('name')
                elif self.mode == 'plain':
                    s += "  " + self.plainArgColor + arg.getAttribute('name').strip() + ": " + ansicolors.end()
                    
                for c in arg.getChilds():
                    s += self.generate(c, prefix, call, depth)

                if self.mode == 'docbook':
                    s += """</entry>
          </row>"""
                elif self.mode == 'plain':
                    s += "\n"
                foundArgs += 1
        if self.mode == 'docbook':
            s += """
        </tbody>
      </tgroup>
    </table>
"""
        if foundArgs > 0:
            return s
        return u""

    def outputOverview(self, node, prefix, call, depth):
        s = u""
        tagNames = node.getChildTagNames()
        if 'description' in tagNames:
            tagNames.remove('description')
        if 'example' in tagNames:
            tagNames.remove('example')
        for tagName in tagNames:
            
            title = u""
            if self.mode == 'plain':
                title += self.plainHeadingColor
            if tagName == 'function':
                title += 'Functions'
            elif tagName == 'topic':
                title += 'Topics'
            elif tagName == 'class':
                title += 'Classes'
            elif tagName == 'module':
                title += 'Modules'
            if self.mode == 'plain':
                title += ':' + ansicolors.end()
                s += "\n" + title + "\n"
            i = 0
            line = u""
            for child in node.getSubNodes(tagName):
                if (not node.hasAttribute('restriction') or
                    node.getAttribute('restriction') == self.langID):
                    if self.mode == 'plain':
                        line += child.getAttribute('name')
                    i += 1
                    if i % 3 == 0:
                        i = 0
                        if self.mode == 'plain':
                            s += line + "\n"
                            line = ""
                    else:
                        if self.mode == 'plain':
                            while len(line) % 30 != 0:
                                line += " "
            if line != u'':
                s += line + "\n"
        if self.mode == 'plain':
            s += "\n"
        return s
    
    def outputContainer(self, node, prefix, call, depth):
        s = u""
    
        name = node.getAttribute('name')
        myprefix = name
        if prefix != "":
            myprefix = prefix + "_" + name
        mycall = name
        if call != "":
            mycall = call + "." + name

        if node.getTagName() == "module":
            title = "Module " + name
        elif node.getTagName() == "topic":
            title = "Topic " + name
        elif node.getTagName() == "class":
            title = "Class " + name
        elif node.getTagName() == "function":
            title = name
        else:
            title = self.Lang + " Developer's Guide to Xpertmud"
        sect = "sect%d" % depth
        if depth == 0:
            sect = "chapter"

        if self.mode == 'docbook':
            s += """\
 <%s id="%s">
  <title>%s</title>
""" % (sect, self.escapeId(myprefix), title)
        elif self.mode == 'plain':
            s += self.plainHeadingColor + title + ":" + ansicolors.end() + "\n" + self.plainUnderlineColor + '*' * (len(title)+1) + ansicolors.end() + "\n"

        if not self.mode == 'docbook' and not node.getTagName() == 'function':
            if self.subHelpText != u"":
                s += self.subHelpText + "\n"
            for sub in node.getSubNodes('description'):
                s += self.generate(sub, myprefix, mycall, depth+1)
            for sub in node.getSubNodes('example'):
                s += self.generate(sub, myprefix, mycall, depth+1)
                
        if node.getTagName() == "function":
            s += self.outputSynopsis(node)
            s += self.outputArgs(node, myprefix, mycall, depth+1)
        else:
            s += self.outputOverview(node, myprefix, mycall, depth+1)

        if self.mode == 'docbook' or node.getTagName() == "function":
            for tag in node.getChildTagNames():
                if not tag in [ 'arg' ]:
                    for sub in node.getSubNodes(tag):
                        s += self.generate(sub, myprefix, mycall, depth+1)

        if self.mode == 'docbook':
            s += """\
 </%s>
""" % sect

        return s

    def whiteSpaceKiller(self, s):
        ret = re.subn(r'[\s\n]+', ' ', s)[0]
        if self.plainSpace:
            ret = re.sub(r'^\s', '', ret)
        return ret
            

    def generate(self, node, prefix=u"", call=u"", depth=0, cwhite=0):
        if type(node) == unicode:
            if self.mode == 'docbook':
                return XMXmlNode("").escape(node)
            else:
                if cwhite:
                    self.plainSpace = 0
                    return node
                else:
                    ret = self.whiteSpaceKiller(node)
                    if ret == ' ':
                        ret = u''
                    self.plainSpace = 0
                    return ret

        s = u""
        if (node.getTagName() == "module" or
            node.getTagName() == "class" or
            node.getTagName() == "topic" or
            node.getTagName() == "help" or
            node.getTagName() == "function"):
            if (not node.getTagName() == "function" or
                not node.hasAttribute('restriction') or
                node.getAttribute('restriction') == self.langID):
                s += self.outputContainer(node, prefix, call, depth)
        elif (node.getTagName() in self.allIDs and
              node.getTagName() != self.langID):
            pass
        elif node.getTagName() == self.langID:
            for c in node.getChilds():
                s += self.generate(c, prefix, call, depth, 1)
        elif node.getTagName() == "alt":
            for c in node.getChilds():
                if not type(c) == unicode:
                    s += self.generate(c, prefix, call, depth)
        elif node.getTagName() == "description":
            if len(node.getSubNodes("para")) == 0:
                if self.mode == 'docbook':
                    s += "<para>"
                for c in node.getChilds():
                    if self.mode == 'docbook':
                        s += self.generate(c, prefix, call, depth)
                    elif self.mode == 'plain':
                        s += self.generate(c, prefix, call, depth)
                if self.mode == 'docbook':
                    s += "</para>"
                elif self.mode == 'plain':
                    s += "\n"
                    self.plainSpace = 1
            else:
                for c in node.getChilds():
                    if type(c) == unicode or c.getTagName() == "para":
                        if self.mode == 'docbook':
                            s += "<para>"
                    if self.mode == 'docbook':
                        s += self.generate(c, prefix, call, depth)
                    elif self.mode == 'plain':
                        s += self.generate(c, prefix, call, depth)
                        if (c != node.getChilds()[len(node.getChilds())-1] and
                            not type(c) == unicode and
                            c.getTagName() == "para"):
                            s += "\n"
                            self.plainSpace = 1

                    if type(c) == unicode or c.getTagName() == "para":
                        if self.mode == 'docbook':
                            s += "</para>"
        elif node.getTagName() == "code":
            if self.mode == 'docbook':
                s += "<programlisting>"
            elif self.mode == 'plain':
                s += "\n" + self.plainCodeColor
            for c in node.getChilds():
                s += self.generate(c, prefix, call, depth, 1)
            if self.mode == 'docbook':
                s += "</programlisting>"
            elif self.mode == 'plain':
                s += ansicolors.end() + "\n"
                self.plainSpace = 1
        elif node.getTagName() == "example":
            if self.mode == 'docbook':
                s += "<para><programlisting>Example:\n"
            elif self.mode == 'plain':
                s += "\n" + self.plainSectionColor + "Example:\n" + ansicolors.end() + self.plainCodeColor
            for c in node.getChilds():
                s += self.generate(c, prefix, call, depth, 1)
            if self.mode == 'docbook':
                s += "</programlisting></para>"
            elif self.mode == 'plain':
                s += ansicolors.end() + "\n"
                self.plainSpace = 1
        elif node.getTagName() in ansicolors.keys():
            if self.mode == 'plain':
                s += ansicolors.code(node.getTagName())
            for c in node.getChilds():
                s += self.generate(c, prefix, call, depth)
            if self.mode == 'plain':
                s += ansicolors.end();
        else:
            for c in node.getChilds():
                s += self.generate(c, prefix, call, depth)
        return s

