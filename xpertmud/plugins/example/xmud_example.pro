TEMPLATE	= lib
CONFIG		+= qt warn_on plugin thread release exceptions
unix:TARGET	= xmud_example
win32:target	= xmud_example
HEADERS		= example.h \
		  ../../../compat/klibloader.h \
		  ../../../compat/kgenericfactory.h
SOURCES		= example.cc \
		  ../../../compat/kglobal.cc

DEFINES += NO_KDE QT_DLL

INCLUDEPATH += ../../../compat
macx:DEFINES += MACOS_X
target.path=dll/plugins
win32:target.files=xmud_example.dll
perlscripts.path=appdata/perl
perlscripts.files=*.pm
pythonscripts.path=appdata/python
pythonscripts.files=*.py
INSTALLS=perlscripts pythonscripts
INSTALLS += target