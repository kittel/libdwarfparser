if HAVE_CYTHON
if HAVE_PYTHON
if ENABLE_PYTHON_BINDING

CLEANFILES =		dwarfparse.cpp setup.py dwarfparse.so

all-local:	dwarfparse_c.pxd dwarfparse.pyx setup.py
	CPPFLAGS="$(CPPFLAGS)" CXXFLAGS="$(CXXFLAGS)" LDFLAGS="$(LDFLAGS)" $(PYTHON) setup.py build_ext

install-exec-local:
	$(PYTHON) setup.py install --prefix=$(DESTDIR)$(prefix)

# This rule cleans up stuff installed by Python's setup.py
# Unfortunately, Python's distutils do not provide an uninstall
# command, so we have to make up for it here in uninstall-local
# hook. This might break if distutils' behaviour changes as automake
# has no control over what distutils install command does.
#
#uninstall-local:
#	rm -f $(DESTDIR)$(libdir)/python*/site-packages/_presage.so
#	rm -f $(DESTDIR)$(libdir)/python*/site-packages/presage.py
#	rm -f $(DESTDIR)$(libdir)/python*/site-packages/presage.pyc
#	rm -f $(DESTDIR)$(libdir)/python*/site-packages/python_presage-*.egg-info
#	rm -f $(DESTDIR)$(bindir)/presage_python_demo

clean-local:
	rm -rf build

#if HAVE_HELP2MAN
#presage_python_demo.1:	presage_python_demo.in $(top_srcdir)/configure.ac
#	chmod u+x ./presage_python_demo
#	help2man --output=$@ --no-info --name="presage demo program (Python)" "$(PYTHON) ./presage_python_demo"
#
#dist_man_MANS =		presage_python_demo.1
#
#DISTCLEANFILES =	presage_python_demo.1
#endif 

endif # ENABLE_PYTHON_BINDING
endif # HAVE_PYTHON
endif # HAVE_SWIG

