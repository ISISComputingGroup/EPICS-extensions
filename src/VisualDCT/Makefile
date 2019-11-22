.PHONY: install clean uninstall distclean realclean

MVNFLAGS=-T 4

install:
	mvn $(MVNFLAGS) clean verify

clean distclean realclean:
	mvn $(MVNFLAGS) clean

uninstall:
