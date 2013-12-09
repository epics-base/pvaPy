TOP = .
RELEASE_LOCAL = $(TOP)/configure/RELEASE.local

# Check for configure/distclean targets 
ifeq ($(filter $(MAKECMDGOALS),configure distclean),)

# Make sure RELEASE.local has been created
ifeq ($(wildcard $(RELEASE_LOCAL)),)

all %:
		@echo "File $(RELEASE_LOCAL) does not exist. Please create this file manually, or by running:"
		@echo ""
		@echo "  EPICS_BASE=<path> EPICS4_DIR=<path> make configure"
		@echo ""

else

# Include standard epics config files
include $(TOP)/configure/CONFIG
DIRS := src
include $(TOP)/configure/RULES_DIRS

endif # ifeq ($(wildcard $(RELEASE_LOCAL)),)

endif # ifeq ($(filter $(MAKECMDGOALS),configure distclean),)

configure: tools/autoconf/configure
	@if [ -f $(RELEASE_LOCAL) ]; then echo "File $(RELEASE_LOCAL) already exists. Please remove it if you would like to reconfigure pvaPy build."; exit 1; fi
	cd tools/autoconf && ./configure

bootstrap tools/autoconf/configure:
	cd tools/autoconf && autoreconf --install 

distclean: 
	rm -rf lib
	rm -rf src/pvaccess/O.*
	rm -f configure/RELEASE.local
	cd tools/autoconf && rm -rf autom4te.cache aclocal.m4 config.log config.status install-sh missing Makefile Makefile.in 

tidy: distclean
	rm -f setup.sh
	rm -f setup.csh
	rm -f tools/autoconf/configure

.PHONY: configure 

