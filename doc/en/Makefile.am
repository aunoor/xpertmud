KDE_LANG = en
KDE_DOCS = xpertmud
#KDE_DOCS_DEPS = perl-devel.docbook python-devel.docbook

EXTRA_DIST=index.cache.bz2

index.cache.bz2: $(KDE_XSL_STYLESHEET) perl-devel.docbook python-devel.docbook localpath
	@if test -n "$(MEINPROC)"; then echo $(MEINPROC) --check --cache \
            index.cache.bz2 index.docbook; $(MEINPROC) --check \
            --cache index.cache.bz2 index.docbook; fi

localpath:
	-cp -f $(srcdir)/index.docbook .

perl-devel.docbook:
	rm -f perl-devel.docbook
	if [ -f $(top_builddir)/xpertmud/scripting/perl/perl-devel.docbook ]; \
	then cp $(top_builddir)/xpertmud/scripting/perl/perl-devel.docbook .; \
	else \
	if [ -f $(top_srcdir)/xpertmud/scripting/perl/perl-devel.docbook ]; \
	then cp $(top_srcdir)/xpertmud/scripting/perl/perl-devel.docbook .; \
	else touch perl-devel.docbook; \
	fi \
	fi

python-devel.docbook:
	rm -f python-devel.docbook
	if [ -f $(top_builddir)/xpertmud/scripting/python/python-devel.docbook ]; \
	then cp $(top_builddir)/xpertmud/scripting/python/python-devel.docbook .; \
	else \
	if [ -f $(top_srcdir)/xpertmud/scripting/python/python-devel.docbook ]; \
	then cp $(top_srcdir)/xpertmud/scripting/python/python-devel.docbook .; \
	else touch python-devel.docbook; \
	fi \
	fi

.PHONY: perl-devel.docbook python-devel.docbook
