#!/usr/bin/python
import sys
import re

class RegState:
    def sub(self, regExp, toExp, string):
        self.match = re.search(regExp, string)
        (self.string, self.count) = re.subn(regExp, toExp, string)
        return (self.count > 0)

reS = RegState()

#if len(sys.argv) < 2:
#    sys.stderr.write("Expecting an argument!\n")
#    sys.exit(1)


functions = ""
definitions = ""
classDefs = {}

line = sys.stdin.readline()
while line != "":
    origLine = line
    if(reS.sub(r'^\s+virtual\s+([^\s]+)\s+([^\(]+)\(', '', line)):
        line = reS.string
        functionType = reS.match.group(1)
        functionName = reS.match.group(2)
        spacedName = "P" + functionName

        pythonName = re.sub(r'^XM_', '', functionName)
        pythonName = re.sub(r'_raise$', '_lift', pythonName);
        pythonName = re.sub(r'_print', '_write', pythonName);
        classMatch = re.match(r'^(.*)_([^_]+)$', pythonName)
        if classMatch != None:
            pythonForwardArgs = "self.getID(),"
            pythonSelfArgs = "self,"
            pythonClass = 'XM' + classMatch.group(1)
            if not classDefs.has_key(pythonClass):
                classDefs[pythonClass] = ""
            pythonFunction = classMatch.group(2)

        definitions += """    {const_cast<char *>("%s"), \t%s, \tMETH_VARARGS, const_cast<char *>("Not documented")},\n""" % (pythonName, spacedName)

        callBack = "    "
        if functionType != "void":
            callBack += "%s ret = " % functionType
        callBack += "currentCallBack->\n      %s(" % functionName
        readVariables = ""
        convert = ""
        typeString = ""
        typeArguments = ""
        hasCodec = 0
        hasDefault = 0
        while reS.sub(r'^\s*([^,\)]+),?', '', line):
            line = reS.string
            m = re.match(r'^(.*)\s+([^\s]+)$', reS.match.group(1))
            if m == None:
                print "ERROR: " + origLine
            type = m.group(1)
            type = re.sub(r'const QString&', 'PyObject*', type)
            name = m.group(2)

            defaultValue = None
            m = re.match(r'^([^=]+)=(.*)$', name)
            if m != None:
                name = m.group(1)
                defaultValue = m.group(2)

            if classMatch != None and name != 'id':
                pythonForwardArgs += name + ','
                pythonSelfArgs += name
                if defaultValue != None:
                    pythonSelfArgs += '=' + defaultValue
                pythonSelfArgs += ','

            readVariables += "    %s %s" % (type, name)
            if defaultValue != None and type != 'PyObject*':
                readVariables += " = %s;\n" % defaultValue
            elif type == 'PyObject*':
                readVariables += " = NULL;\n";
            else:
                readVariables += ";\n"

            if defaultValue != None and hasDefault == 0:
                hasDefault = 1
                typeString += "|"
            if type == 'PyObject*':
                callBack += "q%s," % name
                typeArguments += ", &%s" % (name)
                typeString += "O"
                convert += """
    QString q%s;
    if(%s == NULL) { // unspecified argument
""" % (name, name)
                if defaultValue != None:
                    convert += """\
      q%s = %s;
""" % (name, defaultValue)
                else:
                    convert += """\
      return NULL;
"""
                convert += """\
    } else {
      PyObject* %sUni;
      if(PyUnicode_Check(%s)) {
        %sUni = %s;
      } else {
        %sUni = PyUnicode_FromObject(%s);
      }
      if(%sUni == NULL) {
        return NULL;
      }
      int %sLen = PyUnicode_GET_DATA_SIZE(%sUni) / sizeof(Py_UNICODE);
      q%s = QString((QChar*)NULL, %sLen);
      Py_UNICODE* p%s = PyUnicode_AS_UNICODE(%sUni);
      for(int i=0; i<%sLen; ++i) {
        q%s.ref(i) = QChar(p%s[i]);
      }
    }
""" % (name, name, name, name, name, name, name, name, name, name, name, name, name, name, name, name)

            elif type == 'bool' or type == 'int':
                callBack += "%s," % name
                typeArguments += ", &%s" % name
                typeString += "i"
            elif type == 'unsigned int':
                callBack += "%s," % name
                typeArguments += ", &%s" % name
                typeString += "i"

        if classMatch != None:
            pythonSelfArgs = re.sub(r',$', '', pythonSelfArgs)
            pythonForwardArgs = re.sub(r',$', '', pythonForwardArgs)
            middle = ""
            if pythonFunction == 'close':
                middle = "import XM\n      "
            if functionType != "void":
                middle += "return "
            classDefs[pythonClass] += "    def %s(%s):\n      %sXM.%s(%s)\n\n" % (pythonFunction, pythonSelfArgs, middle, pythonName, pythonForwardArgs)
        

        functions += "  PyObject *%s(PyObject * /* unused: self */, PyObject *" % spacedName;
        if len(typeString) > 0:
            readVariables += "    if(!PyArg_ParseTuple(args, const_cast<char *>(\"%s\")%s))\n      return NULL;\n" % (typeString, typeArguments)
            readVariables += convert
            functions += "args) {\n"
        else:
            functions += " /* unused: args */) {\n"

        callBack = re.sub(r',\s*$', '', callBack)
        readVariables += "\n" + callBack + ");\n\n"

        functions += readVariables
        
        if functionType == "int" or functionType == "bool":
            functions += "    return Py_BuildValue(const_cast<char *>(\"i\"), ret);\n  }\n\n"
        elif functionType == "unsigned int":
            functions += "    return Py_BuildValue(const_cast<char *>(\"i\"), ret);\n  }\n\n"
        elif functionType == "char":
            functions += "    return Py_BuildValue(const_cast<char *>(\"b\"), ret);\n  }\n\n"
        elif functionType == "QString":
            functions += """
    Py_UNICODE* retUni = new Py_UNICODE[ret.length()];
    {for(unsigned int i=0; i<ret.length(); ++i)
      retUni[i] = ret.at(i).unicode();
    }
    return Py_BuildValue(const_cast<char *>(\"u#\"),
                         retUni, ret.length());
    delete[] retUni;
}"""
        elif functionType == "void":
            functions += "    Py_INCREF(Py_None);\n    return Py_None;\n  }\n\n"
        
            
    line = sys.stdin.readline()

f = open(sys.argv[1], "r")
line = f.readline()
while(line != ""):
    if re.match(r'___IMPLEMENTATIONS___', line):
        sys.stdout.write(functions)
    elif re.match(r'___DEFINITIONS___', line):
        sys.stdout.write(definitions)
    else:
        written = 0
        for key in classDefs.keys():
            if re.match('___' + key.upper() + '___METHODS___', line):
                written = 1
                sys.stdout.write(classDefs[key])
        if not written:
            sys.stdout.write(line)
    line = f.readline()


