TEMPLATE	= app
CONFIG		+= qt warn_on release thread exceptions
HEADERS		= \
		  xpertmud/TextBuffer.h \
		  xpertmud/TextBufferView.h \
		  xpertmud/TextBufferHistoryView.h \
		  xpertmud/mxinputline.h \
		  xpertmud/xpertmud.h \
		  xpertmud/InputAction.h \
		  xpertmud/scripting/Scripting.h \
		  xpertmud/PluginWrapper.h \
		  xpertmud/configdialog.h \
		  xpertmud/ColorChar.h \
		  xpertmud/Connection.h \
		  xpertmud/ConnectionHandler.h \
		  xpertmud/IacHandlerCollection.h \
		  xpertmud/CtrlParser.h \
		  xpertmud/TelnetFilter.h \
		  xpertmud/InputStreamHandler.h \
		  xpertmud/Bookmark.h \
		  xpertmud/BookmarkEditor.h \
		  xpertmud/LogToFile.h \
		  compat/kaction.h \
		  compat/kfontdialog.h \
		  compat/kapplication.h \
		  compat/kcolorbutton.h \
		  compat/kdialogbase.h \
		  compat/kextendedsocket.h \
		  compat/kfiledialog.h \
		  compat/klibloader.h \
		  compat/kmainwindow.h \
		  compat/kmessagebox.h \
		  compat/kpopupmenu.h \
		  compat/ktoolbar.h \
		  compat/dummykpartsdockmainwindow.h \
		  compat/kdocktabctl.h \
		  compat/kdockwidget.h \
		  compat/kdockwidget_private.h \
		  qextmdi/include/qextmditaskbar.h \
		  qextmdi/include/qextmdichildarea.h \
		  qextmdi/include/qextmdichildfrmcaption.h \
		  qextmdi/include/qextmdichildfrm.h \
		  qextmdi/include/qextmdichildview.h \
		  qextmdi/include/qextmdidefines.h \
		  qextmdi/include/qextmdiiterator.h \
		  qextmdi/include/qextmdilistiterator.h \
		  qextmdi/include/qextmdinulliterator.h \
		  qextmdi/include/qextmdimainfrm.h
unix:HEADERS +=   xpertmud/MCCP.h

		  

SOURCES		= \
		  xpertmud/TextBuffer.cc \
		  xpertmud/TextBufferView.cc \
		  xpertmud/TextBufferHistoryView.cc \
		  xpertmud/mxinputline.cc \
		  xpertmud/InputAction.cc \
		  xpertmud/scripting/Scripting.cc \
		  xpertmud/xpertmud.cc \
		  xpertmud/main.cc \
		  xpertmud/xpertmud_scripting.cc \
		  xpertmud/PluginWrapper.cc \
		  xpertmud/Connection.cc \
		  xpertmud/ConnectionHandler.cc \
		  xpertmud/IacHandlerCollection.cc \
		  xpertmud/CtrlParser.cc \
		  xpertmud/TelnetFilter.cc \
		  xpertmud/InputStreamHandler.cc \
		  xpertmud/configdialog.cc \
		  xpertmud/LogToFile.cc \
		  compat/klibloader.cc \
		  compat/kapplication.cc \
		  xpertmud/Bookmark.cc \
		  xpertmud/BookmarkEditor.cc \
		  compat/kglobal.cc \
		  compat/kdocktabctl.cpp \
		  compat/kmainwindow.cc \
		  compat/kdockwidget.cpp \
		  compat/kdockwidget_private.cpp \
		  qextmdi/src/qextmdichildarea.cpp \
		  qextmdi/src/qextmdichildfrmcaption.cpp \
		  qextmdi/src/qextmdichildfrm.cpp \
		  qextmdi/src/qextmdichildview.cpp \
		  qextmdi/src/qextmdimainfrm.cpp \
		  qextmdi/src/qextmditaskbar.cpp 
unix:SOURCES +=   xpertmud/MCCP.cc


DEFINES += QT_DLL NO_KDE 

# for qextmdi
DEFINES +=NO_KDE2 NO_INCLUDE_MOCFILES \
	   MAKEDLL_QEXTMDI

INCLUDEPATH	+=  compat qextmdi/include qextmdi/res

macx:DEFINES += MACOS_X

TARGET=xpertmud-qt
unix:LIBS += -lz
win32:target.files=xpertmud-qt.exe
target.path=.
icons.path=icon
icons.files=xpertmud/*.png

INSTALLS=target icons
