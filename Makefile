TOP = .
RELEASE_LOCAL = $(TOP)/configure/RELEASE.local
SETUP_SH = $(TOP)/setup.sh
CONFIG_SITE_LOCAL = $(TOP)/configure/CONFIG_SITE.local
DOC_DIR = doc

# Check for configure/distclean targets 
ifeq ($(filter $(MAKECMDGOALS),configure distclean),)

# Make sure RELEASE.local has been created
ifeq ($(wildcard $(RELEASE_LOCAL)),)

all %:
		@echo "File $(RELEASE_LOCAL) does not exist. Please create both $(RELEASE_LOCAL) and $(CONFIG_SITE_LOCAL) manually, or by running:"
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
	@if [ -f $(RELEASE_LOCAL) ]; then echo "File $(RELEASE_LOCAL) already exists. Please remove both $(RELEASE_LOCAL) and $(CONFIG_SITE_LOCAL) if you would like to reconfigure pvaPy build."; exit 1; fi
	@if [ -f $(CONFIG_SITE_LOCAL) ]; then echo "File $(CONFIG_SITE_LOCAL) already exists. Please remove both $(RELEASE_LOCAL) and $(CONFIG_SITE_LOCAL) if you would like to reconfigure pvaPy build."; exit 1; fi
	cd tools/autoconf && ./configure

bootstrap tools/autoconf/configure: tools/autoconf/configure.ac $(wildcard tools/autoconf/m4/*.m4)
	cd tools/autoconf && autoreconf --install 

.PHONY: doc docclean distclean tidy
doc:
	cd $(DOC_DIR) && make

docclean:
	cd $(DOC_DIR) && make clean

distclean: 
	rm -rf lib
	rm -rf src/pvaccess/O.*
	rm -f configure/RELEASE.local
	rm -f configure/CONFIG_SITE.local
	cd tools/autoconf && rm -rf autom4te.cache aclocal.m4 config.log config.status missing Makefile Makefile.in 
	cd $(DOC_DIR) && make distclean

tidy: distclean
	rm -f setup.sh
	rm -f setup.csh
	cd tools/autoconf && rm -f configure install-sh
	cd $(DOC_DIR) && make tidy

.PHONY: configure 

