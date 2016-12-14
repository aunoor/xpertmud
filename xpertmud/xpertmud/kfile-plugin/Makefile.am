kde_module_LTLIBRARIES = kfile_xmud.la
kfile_xmud_la_SOURCES=kfile_xmud.cc
kfile_xmud_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kfile_xmud_la_LIBADD = $(LIB_KIO)
kfile_xmud_la_METASOURCES = AUTO

services_DATA=kfile_xmud.desktop
servicesdir=$(kde_servicesdir)

INCLUDES= -g $(all_includes)

#messages: rc.cpp
#	$(XGETTEXT) kfile_xmud.cpp -o $(podir)/kfile_xmud.pot