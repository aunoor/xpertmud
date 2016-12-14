import re
import sys

class XMXmlNode:
    def __init__(self, tagName, dtd = None):
        self.childs = []
        self.attributes = {}
        self.tagName = tagName
        if dtd != None:
            tag = dtd.getTag(self.tagName)
            for defaultAttr in tag.getDefaultAttributes():
                self.setAttribute(defaultAttr, tag.getDefault(defaultAttr))
                

    def setAttribute(self, name, value):
        self.attributes[name] = value

    def getAttribute(self, name):
        return self.attributes[name]

    def hasAttribute(self, name):
        return self.attributes.has_key(name)

    def getChildTagNames(self):
        ret = []
        for child in self.childs:
            if (type(child) != unicode and
                not child.getTagName() in ret):
                ret.append(child.getTagName())
        return ret

    def removeChild(self, child):
        self.childs.remove(child)

    def addChild(self, child):
        if type(child) == unicode and child == "":
            return

        if (type(child) == unicode and len(self.childs) > 0 and
            type(self.childs[len(self.childs)-1]) == unicode):
            self.childs[len(self.childs)-1] += child
            return
            
        if type(child) == str:
            raise "Child must be unicode"
        else:
            self.childs.append(child)

    def getChilds(self):
        return self.childs

    def getSubNodes(self, name=""):
        ret = []
        for child in self.childs:
            if type(child) != unicode and (name == "" or
                                           name == child.getTagName()):
                ret.append(child)
        return ret

    def getTagName(self):
        return self.tagName

    def escape(self, s):
        ret = u""
        for char in s:
            if ord(char) <= 6 or ord(char) > 255:
                ret += "&#" + unicode(ord(char)) + ";"
            elif char == u"&":
                ret += u"&amp;"
            elif char == u"<":
                ret += u"&lt;"
            elif char == u">":
                ret += u"&gt;"
            elif char == u"\"":
                ret += u"&quot;"
            else:
                ret += char
        return ret

    def toString(self, indent=""):
        s = "<" + self.getTagName()
        for attr in self.attributes.keys():
            s += ' ' + attr + '="' + self.escape(self.attributes[attr]) + '"'
        
        if len(self.childs) > 0:
            s += ">"
            for child in self.childs:
                if type(child) == unicode:
                    s += self.escape(child)
                else:
                    s += child.toString(indent + " ")
            s += "</" + self.getTagName() + ">"
        else:
            s += "/>"

        return s

class XMXmlDTDTag:
    def __init__(self, name):
        self.attributes = {}
        self.name = name

    def addAttribute(self, name, value):
        if not self.attributes.has_key(name):
            self.attributes[name] = []
        self.attributes[name].append(value)

    def getDefault(self, name):
        if not self.attributes.has_key(name):
            return u""
        return self.attributes[name][0]

    def getDefaultAttributes(self):
        return self.attributes

    def getName(self):
        return self.name

class XMXmlDTD:
    def __init__(self):
        self.tags = {}

    def addTag(self, tag):
        self.tags[tag.getName()] = tag

    def getTag(self, name):
        if not self.tags.has_key(name):
            return XMXmlDTDTag(name)
        return self.tags[name]

# pseudo xml parser...
class XMXmlParser:
    inWordExtra = re.compile(r'[0-9]')
    startWord = re.compile(r'[a-zA-Z]')
    
    def __init__(self, dtd):
        self.state = "parsechild"
        self.wasspace = 0
        self.stack = [ XMXmlNode("document") ]
        self.lineno = 1
        self.column = 1
        self.attr = ""
        self.buffer = u""
        self.closing = 0
        self.dtd = dtd

    def entity(self, name):
        if name == "amp":
	    return "&"
        elif name == "lt":
            return "<"
        elif name == "gt":
            return ">"
        elif name == "quot":
            return '"'
        elif len(name) > 0 and name[0] == '#':
            return unichr(int(name[1:]))
        return ""

    def isSpace(self, char):
        return (char == " " or char == "\t" or
                char == "\n" or char == "\r")

    def isSlash(self, char):
        return (char == "/")

    def isLeft(self, char):
        return (char == "<")

    def isRight(self, char):
        return (char == ">")

    def isEq(self, char):
        return (char == "=")

    def isQuote(self, char):
        return (char == "\"")

    def isAmp(self, char):
        return (char == "&")

    def isSemicolon(self, char):
        return (char == ";")

    def allowedName(self, char, position):
        return (self.startWord.match(char) or
                (position > 0 and self.inWordExtra.match(char)))
            
    def error(self, description):
        s = ""
        if self.file != '':
            s += self.file + ": "
        s += "(%d, %d) - %s" % (self.lineno, self.column, description)
        raise s

    def parseChild(self, char):
#        sys.stderr.write("BUFFER: \"" + self.buffer + "\"\n")
#        sys.stderr.write("Charactor: " + str(ord(char)) + " \"" + char + "\"\n")
        if self.isLeft(char):
            if self.wasspace:
                self.buffer += ' '
            self.stack[len(self.stack)-1].addChild(self.buffer)
            self.buffer = u""
            self.closing = 0
            self.state = "tagname"
        elif self.isAmp(char):
            self.entityname = ""
            self.laststate = "parsechild"
            self.state = "entityname"
        else:
            curnode = self.stack[len(self.stack)-1]
            if (curnode.hasAttribute('xml:space') and
                curnode.getAttribute('xml:space') == 'preserve'):
                self.buffer += char
            else:
                if self.isSpace(char):
                    if (len(self.buffer) > 0 or
                        len(curnode.getChilds()) > 0):
                        self.wasspace = 1
#                        sys.stderr.write("Space\n")
                else:
                    if self.wasspace:
#                        sys.stderr.write("WasSpace\n")
                        self.buffer += ' '
                        self.wasspace = 0
#                    sys.stderr.write("Adding\n")
                    self.buffer += char
        
    def tagName(self, char):
        if (not self.closing and
            self.isSpace(char)) or self.isRight(char):
            if len(self.buffer) > 0:
                if self.closing:
                    if self.stack[len(self.stack)-1].getTagName() != self.buffer:
                        self.error("Closing tag mismatch")
                    if len(self.stack) == 0:
                        self.error("You mustn't close the document tag")
                    else:
                        self.stack.pop()
                else:
                    node = XMXmlNode(self.buffer, self.dtd)
                    self.stack[len(self.stack)-1].addChild(node)
                    self.stack.append(node)
                self.buffer = u""
                self.wasspace = 0
                if self.isSpace(char):
                    self.state = "waitforattr"
                else:
                    self.state = "parsechild"
            else:
                self.error("Need tag name after <")
        elif self.isSlash(char) and len(self.buffer) == 0:
            self.closing = 1
        elif self.allowedName(char, len(self.buffer)):
            self.buffer += char
        else:
            self.error("Character not allowed in tag name")

    def waitForAttr(self, char):
        if self.allowedName(char, 0):
            self.buffer += char
            self.state = "attrname"
        elif self.isRight(char):
            self.state = "parsechild"
        elif self.isSlash(char):
            self.state = "nochild"
        elif not self.isSpace(char):
            self.error("Character not allowed at beginning of attribute name")

    def noChild(self, char):
        if self.isRight(char):
            self.stack.pop()
            self.state = "parsechild"
        else:
            self.error("> expected after / in tag")

    def attrName(self, char):
        if self.allowedName(char, 1):
            self.buffer += char
        elif self.isSpace(char):
            self.attr = self.buffer
            self.buffer = u""
            self.state = "waitforeq"
        elif self.isEq(char):
            self.attr = self.buffer
            self.buffer = u""
            self.state = "waitforvalue"
        else:
            self.error("Character not allowed in attribute name")

    def waitForEq(self, char):
        if self.isEq(char):
            self.state = "waitforvalue"
        elif not self.isSpace(char):
            self.error("= expected")

    def waitForValue(self, char):
        if self.isQuote(char):
            self.state = "attrvalue"
        elif not self.isSpace(char):
            self.error("\" expected")

    def attrValue(self, char):
        if self.isQuote(char):
            self.stack[len(self.stack)-1].setAttribute(self.attr, self.buffer)
            self.buffer = u""
            self.state = "waitforattr"
        elif self.isAmp(char):
            self.entityname = ""
            self.laststate = "attrvalue"
            self.state = "entityname"
        else:
            self.buffer += char

    def entityName(self, char):
        if self.isSemicolon(char):
            self.buffer += self.entity(self.entityname)
            self.state = self.laststate
        else:
            self.entityname += char
                
    def parse(self, chunk):
        for char in chunk:
            if self.lineno == 1:
                pass
            elif self.state == "tagname":
                self.tagName(char)
            elif self.state  == "waitforattr":
                self.waitForAttr(char)
            elif self.state == "attrname":
                self.attrName(char)
            elif self.state == "waitforeq":
                self.waitForEq(char)
            elif self.state == "waitforvalue":
                self.waitForValue(char)
            elif self.state == "attrvalue":
                self.attrValue(char)
            elif self.state == "parsechild":
                self.parseChild(char)
            elif self.state == "nochild":
                self.noChild(char)
            elif self.state == "entityname":
                self.entityName(char)

            self.column += 1
            if char == "\n":
                self.lineno += 1
                self.column = 1

    def finish(self):
        self.state = "parsechild"
        self.wasspace = 0
        self.lineno = 1
        self.column = 1
        self.attr = ""
        self.buffer = u""
        self.closing = 0

    def getNodes(self):
        return self.stack[0].getSubNodes()

            
    def parseFile(self, file):
        self.file = file
        f = open(file, 'r')
        line = f.readline()
        while line != "":
            self.parse(unicode(line))
            line = f.readline()
