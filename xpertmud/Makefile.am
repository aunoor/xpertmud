
SUBDIRS = qextmdi xpertmud po doc

EXTRA_DIST = AUTHORS COPYING ChangeLog INSTALL INSTALL-LOCAL README TODO xpertmud.lsm admin $(wildcard admin/*.*)

# TODO: change automake version to 1.5 or 1.6, then merge the AUTOMAKE_1_5 
# Branch into the admin dir, or grab a new copy of admin from the KDE.
AUTOMAKE_OPTIONS =  1.4

$(top_srcdir)/configure.in: configure.in.in $(top_srcdir)/subdirs
	cd $(top_srcdir) && $(MAKE) -f admin/Makefile.common configure.in ;

$(top_srcdir)/subdirs:
	cd $(top_srcdir) && $(MAKE) -f admin/Makefile.common subdirs

$(top_srcdir)/acinclude.m4: $(top_srcdir)/admin/acinclude.m4.in $(top_srcdir)/admin/libtool.m4.in
	@cd $(top_srcdir) && cat admin/acinclude.m4.in admin/libtool.m4.in > acinclude.m4

MAINTAINERCLEANFILES = subdirs configure.in acinclude.m4 configure.files 

package-messages:
	$(MAKE) -f admin/Makefile.common package-messages
	$(MAKE) -C po merge

dist-hook:
	cd $(top_distdir) && perl admin/am_edit -padmin
	cd $(top_distdir) && $(MAKE) -f admin/Makefile.common subdirs
	rm -rf `find $(distdir)/admin -name CVS`	
