AutoPerlInterpret.lo: CXXFLAGS:=$(CXXFLAGS:-ansi=)
AutoPerlInterpret.lo: CXXFLAGS:=$(CXXFLAGS:-pedantic=)
AutoPerlInterpret.lo: CXXFLAGS:=$(CXXFLAGS:-Wall=)


# Sorry to revert your patch, but it has to be done at ./configure time.
# that lines mess up compilation on windows / without KDE, as there this
# Makefile is used to generate some files.
#if WITH_PERL
kde_module_LTLIBRARIES=libxmperlinterpreter.la
#endif

libxmperlinterpreter_la_SOURCES=AutoGuiScriptingWrapper.cc PerlInterpret.h \
	XMPerl.cc XMPerlFactory.cc

EXTRA_DIST=sysinit.pl snake.pl perl-devel.docbook sysinit_plf.h

libxmperlinterpreter_la_LDFLAGS =  $(all_libraries)  \
	$(PERL_LDFLAGS) $(KDE_PLUGIN)

libxmperlinterpreter_la_LIBADD  = AutoPerlInterpret.lo ../libscripting.la \
         $(LIB_KDECORE) $(LIB_QT) $(LIBSOCKET)

INCLUDES= $(all_includes) $(PERL_CPPFLAGS)

libxmperlinterpreter_la_METASOURCES = AUTO

BUILT_SOURCES = sysinit_plf.h perl-devel.docbook

sysinit_plf.h: sysinit.pl ../help/functions.xml ../help/topics.xml ../help/classes.xml helplayout.pm xmlparser.pm pl2h.pl ansicolors.pm
	$(PERL) $(srcdir)/pl2h.pl SYSINIT_PL $(srcdir) < $< > $@

AutoPerlInterpret.cc: ../GuiScriptingBindings.h PerlInterpret_cc.in createCallbacks.pl 
	$(PERL) $(srcdir)/createCallbacks.pl interpret_cc $(srcdir)/PerlInterpret_cc.in < $< > $@

AutoGuiScriptingWrapper.h: ../GuiScriptingBindings.h GuiScriptingWrapper_h.in createCallbacks.pl
	$(PERL) $(srcdir)/createCallbacks.pl wrapper_h $(srcdir)/GuiScriptingWrapper_h.in < $< > $@

AutoGuiScriptingWrapper.cc: ../GuiScriptingBindings.h createCallbacks.pl AutoGuiScriptingWrapper.h sysinit_plf.h
	$(PERL) $(srcdir)/createCallbacks.pl wrapper_cc < $< > $@


# install scripts
perldir = $(kde_datadir)/xpertmud/perl
perl_DATA = snake.pl pipes.pl speedwalk.pl completion.pl

perl-devel.docbook: ../help/functions.xml ../help/topics.xml ../help/classes.xml helpConverter.pl xmlparser.pm ansicolors.pm helplayout.pm
	$(PERL) -I$(srcdir) $(srcdir)/helpConverter.pl $(srcdir)/../help/functions.xml $(srcdir)/../help/topics.xml $(srcdir)/../help/classes.xml > $@

generated: AutoPerlInterpret.cc AutoGuiScriptingWrapper.h AutoGuiScriptingWrapper.cc sysinit_plf.h
 