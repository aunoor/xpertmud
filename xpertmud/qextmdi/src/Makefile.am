####### kdevelop will overwrite this part!!! (begin)##########


EXTRA_DIST = qextmdichildarea.cpp qextmdichildfrm.cpp qextmdichildfrmcaption.cpp qextmdichildview.cpp qextmdimainfrm.cpp qextmditaskbar.cpp 

####### xm will overwrite this part!!! (end)############
# Here resides the qextmdi library.

INCLUDES = -I$(top_srcdir)/qextmdi/include -I$(top_srcdir)/qextmdi/res $(all_includes)

noinst_LTLIBRARIES = libxmqextmdi.la
libxmqextmdi_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) #-version-info 2:0
libxmqextmdi_la_LIBADD  = $(LIB_KDEUI) $(LIB_KPARTS) 

libxmqextmdi_la_SOURCES = qextmdichildarea.cpp qextmdichildfrm.cpp qextmdichildfrmcaption.cpp qextmdichildview.cpp qextmdimainfrm.cpp qextmditaskbar.cpp

noinst_HEADERS = ../include/qextmdichildarea.h ../include/qextmdichildfrm.h ../include/qextmdichildfrmcaption.h ../include/qextmdichildview.h ../include/qextmdimainfrm.h ../include/qextmditaskbar.h ../include/qextmdiiterator.h ../include/qextmdilistiterator.h ../include/qextmdinulliterator.h

KDE_OPTIONS = nofinal
METASOURCES = AUTO

META_INCLUDES=$(srcdir)/../include
