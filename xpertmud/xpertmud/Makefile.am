# this 10 paths are KDE specific. Use them:
# kde_htmldir       Where your docs should go to. (contains lang subdirs)
# kde_appsdir       Where your application file (.kdelnk) should go to. 
# kde_icondir       Where your icon should go to.
# kde_minidir       Where your mini icon should go to.
# kde_datadir       Where you install application data. (Use a subdir)
# kde_locale        Where translation files should go to.(contains lang subdirs)
# kde_cgidir        Where cgi-bin executables should go to.
# kde_confdir       Where config files should go to.
# kde_mimedir       Where mimetypes should go to.
# kde_toolbardir    Where general toolbar icons should go to.
# kde_wallpaperdir  Where general wallpapers should go to.

SUBDIRS=scripting . muds plugins kfile-plugin

bin_PROGRAMS = xpertmud

xpertmud_SOURCES = MCCP.cc TextBufferHistoryView.cc main.cc  xpertmud.cc xpertmud_scripting.cc \
 DCOPxpertmud.skel mxinputline.cc PluginWrapper.cc \
 configdialog.cc \
 InputAction.cc \
 InputStreamHandler.cc TelnetFilter.cc  \
 Connection.cc ConnectionHandler.cc IacHandlerCollection.cc \
 CtrlParser.cc Bookmark.cc BookmarkEditor.cc \
 TextBuffer.cc TextBufferView.cc LogToFile.cc

EXTRA_DIST = xpertmud.desktop x-xpertmud-bookmark.desktop xpertmud.protocol \
 DCOPxpertmud_skel.cc DCOPxpertmud.kidl

# the library search path. 
xpertmud_LDFLAGS = $(all_libraries) $(KDE_RPATH) 
xpertmud_LDADD   = $(top_builddir)/qextmdi/src/libxmqextmdi.la $(LIB_KIO) $(LIB_KDECORE) $(LIB_QT) $(LIBSOCKET) $(LIB_KPARTS) $(LIB_KDEUI) 
xpertmud_METASOURCES = AUTO


# set the include path for X, qt and KDE
INCLUDES= -I$(top_srcdir)/qextmdi/include $(all_includes)


# install XMLGUI
rcdir = $(kde_datadir)/xpertmud
rc_DATA = xpertmudui.rc

# install Menu entry
appsdir   = $(kde_appsdir)/Games
apps_DATA = xpertmud.desktop

# install Menu entry
applicationsdir   = $(datadir)/applications/kde
applications_DATA = xpertmud.desktop

# install Mime type
mimedir   = $(kde_mimedir)/application
mime_DATA = x-xpertmud-bookmark.desktop

# install telnet:// link support
servicedir   = $(kde_servicesdir)
service_DATA = xpertmud.protocol
# TODO? Remove/renamp original telnet.protocol entry?

pixmapdir = $(datadir)/pixmaps
pixmap_DATA = xpertmud.xpm xpertmud-16.xpm

KDE_ICON = AUTO

# TODO: Still needed/functional in KDE3 Build environment?
messages: rc.cpp
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
	 $(XGETTEXT) $$LIST -o $(podir)/xpertmud.pot; \
	fi
