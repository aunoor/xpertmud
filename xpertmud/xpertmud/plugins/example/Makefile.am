kde_module_LTLIBRARIES = xmud_example.la

xmud_example_la_SOURCES=example.cc

xmud_example_la_METASOURCES = AUTO

xmud_example_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
xmud_example_la_LIBADD = $(LIB_KDECORE)

INCLUDES= -g $(all_includes)

EXTRA_DIST=example.h XMExample.pm

perldir = $(kde_datadir)/xpertmud/perl
perl_DATA = XMExample.pm

