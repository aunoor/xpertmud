kde_module_LTLIBRARIES = xmud_html.la

xmud_html_la_SOURCES=XMhtml.cc

xmud_html_la_METASOURCES = AUTO

xmud_html_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
xmud_html_la_LIBADD = $(LIB_KDECORE) $(LIB_KHTML)

INCLUDES= -g $(all_includes)

EXTRA_DIST=XMhtml.h XMHTML.pm

perldir = $(kde_datadir)/xpertmud/perl
perl_DATA = XMHTML.pm

