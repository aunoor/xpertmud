TEMPLATE        = lib
CONFIG		+= qt warn_on plugin thread release exceptions
unix:TARGET	= xmperlinterpreter
HEADERS         = PerlInterpret.h \
                  AutoGuiScriptingWrapper.h \
		  XMPerl.h \
		  XMPerlFactory.h \
		  ../Scripting.h \
		  ../GuiScriptingBindings.h \
		  ../../../compat/klibloader.h
SOURCES		= AutoPerlInterpret.cc \
                  AutoGuiScriptingWrapper.cc \
		  XMPerl.cc \
		  XMPerlFactory.cc \
		  ../Scripting.cc \
		  ../../../compat/kglobal.cc
DEFINES += NO_KDE QT_DLL

unix:INCLUDEPATH += $$system($PERL -MExtUtils::Embed -e perl_inc| $PERL -pe 's/-I//g' )
#win32:INCLUDEPATH += $$system(%PERL% -MExtUtils::Embed -e perl_inc| %PERL% -pe 's/-I//g' )
win32:INCLUDEPATH += C:\Perl\lib\CORE
INCLUDEPATH += ../../../compat
unix:LIBS=$$system($PERL -MExtUtils::Embed -e ldopts | $PERL -pe 's/-l(gdbm|ndbm|posix|db)\b//g')
#win32:LIBS=$$system(%PERL% -MExtUtils::Embed -e ldopts)
win32:LIBS+=C:\Perl\lib\CORE\perl58.lib
macx:DEFINES += MACOS_X
win32:target.files=libxmperlinterpreter.dll
target.path = dll/interpreter
scripts.path = appdata/perl
scripts.files = completion.pl snake.pl speedwalk.pl pipes.pl easywalk.pl easywalk.demo.pl
INSTALLS += target scripts
