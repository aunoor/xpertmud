TEMPLATE	= lib
CONFIG		+= qt warn_on plugin thread release
unix:TARGET	= xmpythoninterpreter

HEADERS		= \
		  XMPython.h \
		  XMPythonFactory.h \
		  ../Scripting.h \
		  ../../../compat/klibloader.h
SOURCES		= \
		  AutoXMPython.cc \
		  XMPythonFactory.cc \
		  ../Scripting.cc \
		  ../../../compat/kglobal.cc
DEFINES += NO_KDE QT_DLL 

macx {
# TODO:
#  INCLUDEPATH +=$$system()
  INCLUDEPATH += /usr/include/python2.2

  INCLUDEPATH += ../../../compat

#TODO
#  LIBS=$$system($PYTHON -c "import sys\nprint '-I%s/include/python%s' % (sys.prefix, sys.version[:3])")
  LIBS += -L/usr/lib/python2.2 -lpython2.2
  DEFINES += MACOS_X
  target.path = dll/interpreter
  scripts.path = appdata/python
  scripts.files = bt3030.py tapp.py ansicolors.py
  INSTALLS += target scripts
}


win32:DEPENDPATH	= . ../../../compat ../../ ../../scripting ../../../qextmdi/include ../../../qextmdi/src
win32:DEF_FILE        = libxmpythoninterpreter.def
win32:TMAKE_CXXFLAGS	= /FD /GZ /TP -GX /DEBUG /LD
win32:INCLUDEPATH	=  /usr/include/python2.3/ ../../../compat
win32:TMAKE_LIBS	+= /usr/lib/libpython2.3.so
win32:INCLUDEPATH	= C:\Programme\Python2.2\include ../../../compat C:\Programme\Python22\include
win32:TMAKE_LIBS      += python22.lib
win32:TMAKE_LFLAGS	= /LIBPATH:C:\Python22\libs /LIBPATH:C:\Programme\Python2.2\libs
