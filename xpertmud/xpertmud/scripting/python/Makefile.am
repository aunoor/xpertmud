if WITH_PYTHON
kde_module_LTLIBRARIES=libxmpythoninterpreter.la
endif

libxmpythoninterpreter_la_SOURCES= AutoXMPython.cc XMPythonFactory.cc

EXTRA_DIST = bt3030.py python-devel.docbook sysinit_pyf.h tapp.py

libxmpythoninterpreter_la_LDFLAGS =  $(all_libraries) \
	$(PYTHON_LDFLAGS) $(KDE_PLUGIN) 

libxmpythoninterpreter_la_LIBADD  =  ../libscripting.la \
         $(LIB_KDECORE) $(LIB_QT) $(LIBSOCKET) -lutil -lpthread 

INCLUDES= $(all_includes) $(PYTHON_CPPFLAGS)

libxmpythoninterpreter_la_METASOURCES = AUTO


BUILT_SOURCES = sysinit_pyf.h python-devel.docbook

sysinit_pyf.h: AutoSysinit.py ../help/functions.xml ../help/topics.xml ../help/classes.xml helplayout.py xmlparser.py py2h.py ansicolors.py
	$(PYTHON) $(srcdir)/py2h.py SYSINIT_PY $(srcdir) < $< > $@

AutoXMPython.cc: ../GuiScriptingBindings.h XMPython_cc.in createCallbacks.py sysinit_pyf.h
	$(PYTHON) $(srcdir)/createCallbacks.py $(srcdir)/XMPython_cc.in < $< > $@

AutoSysinit.py: ../GuiScriptingBindings.h sysinit_py.in createCallbacks.py
	$(PYTHON) $(srcdir)/createCallbacks.py $(srcdir)/sysinit_py.in < $< > $@

clean:
	rm -f Auto* sysinit_pyf.h

# install scripts
pythondir = $(kde_datadir)/xpertmud/python
python_DATA = bt3030.py tapp.py

python-devel.docbook: ../help/functions.xml ../help/topics.xml ../help/classes.xml helpConverter.py xmlparser.py ansicolors.py helplayout.py
	$(PYTHON) $(srcdir)/helpConverter.py $(srcdir)/../help/functions.xml $(srcdir)/../help/topics.xml $(srcdir)/../help/classes.xml > $@

generated: sysinit_pyf.h AutoXMPython.cc AutoSysinit.py
