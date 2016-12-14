####### kdevelop will overwrite this part!!! (begin)##########

SUBDIRS = src 

####### kdevelop will overwrite this part!!! (end)############
EXTRA_DIST = include res

dist-hook:
	rm -rf `find $(distdir)/include -name CVS`
	rm -rf `find $(distdir)/res -name CVS`
