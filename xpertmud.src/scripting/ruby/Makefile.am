if WITH_RUBY
kde_module_LTLIBRARIES=libxmrubyinterpreter.la
endif

libxmrubyinterpreter_la_SOURCES= AutoXMRuby.cc XMRubyFactory.cc

EXTRA_DIST = sysinit.rb sysinit_rb.h

libxmrubyinterpreter_la_LDFLAGS =  $(all_libraries) \
	$(KDE_PLUGIN) 

libxmrubyinterpreter_la_LIBADD  =  ../libscripting.la \
         $(RUBY_LIBS) $(LIB_KDECORE) $(LIB_QT) $(LIBSOCKET) -lutil -lpthread 

INCLUDES= $(all_includes) $(RUBY_CPPFLAGS)

libxmrubyinterpreter_la_METASOURCES = AUTO

BUILT_SOURCES=sysinit_rb.h ruby-devel.docbook AutoXMRuby.cc
# install scripts
rubydir = $(kde_datadir)/xpertmud/ruby
ruby_DATA = 


ruby-devel.docbook:
	echo "Not yet available"

sysinit_rb.h: sysinit.rb rb2h.rb
	$(RUBY) $(srcdir)/rb2h.rb SYSINIT < $< > $@



AutoXMRuby.cc: ../GuiScriptingBindings.h XMRuby_cc.in createCallbacks.rb sysinit_rb.h
	$(RUBY) $(srcdir)/createCallbacks.rb $(srcdir)/XMRuby_cc.in < $< > $@
