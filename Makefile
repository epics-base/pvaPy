TOP = .

AC_DIR = tools/autoconf
DOC_DIR = documentation
CONFIGURE_DIR = configure
PVACCESS_DIR = src/pvaccess
CONDA_DIR = tools/conda

RELEASE_LOCAL = $(CONFIGURE_DIR)/RELEASE.local
CONFIG_SITE_LOCAL = $(CONFIGURE_DIR)/CONFIG_SITE.local

ifeq ($(filter $(MAKECMDGOALS),configure distclean),)
  # Command-line goal is neither configure nor distclean

  ifeq ($(wildcard $(RELEASE_LOCAL)),)
    # RELEASE.local file doesn't exist

    all:
    %:
	@echo "The required file $(RELEASE_LOCAL) does not exist."
	@echo "See README.txt for instructions on configuring this module"
	@echo "by hand, or autoconfigure the module by running"
	@echo ""
	@echo "    make configure EPICS_BASE=<path> EPICS4_DIR=<path>"
	@echo ""
	@exit 1

  else
    # RELEASE.local file exists

    # Standard EPICS build stuff.
    include $(TOP)/configure/CONFIG

    DIRS = configure src

    src_DEPEND_DIRS = configure

    include $(TOP)/configure/RULES_TOP

  endif # RELEASE.local

else
  # Command-line goal is configure or distclean

  RM ?= rm -f
  RMDIR ?= rm -rf

  configure: $(AC_DIR)/configure
	@$(RM) $(RELEASE_LOCAL) $(CONFIG_SITE_LOCAL)
	$(AC_DIR)/configure --with-top=$(TOP)

  $(AC_DIR)/configure: $(AC_DIR)/configure.ac $(wildcard $(AC_DIR)/m4/*.m4)
	autoreconf --install $(AC_DIR)

  distclean:
	$(RM) config.log $(RELEASE_LOCAL) $(CONFIG_SITE_LOCAL)
	$(RMDIR) bin lib $(PVACCESS_DIR)/O.* $(AC_DIR)/autom4te.cache
	$(RM) $(AC_DIR)/aclocal.m4 $(AC_DIR)/configure $(AC_DIR)/config.log
	$(RM) $(AC_DIR)/config.status $(AC_DIR)/install-sh $(AC_DIR)/missing
	$(RM) $(AC_DIR)/Makefile $(AC_DIR)/Makefile.in config.log
	$(RMDIR) $(AC_DIR)/compile
	$(MAKE) -C $(DOC_DIR) distclean
	$(RMDIR) $(CONFIGURE_DIR)/O.*

endif # Command-line goal

doc:
	$(MAKE) -C $(DOC_DIR)

docclean:
	$(MAKE) -C $(DOC_DIR) clean

srcclean:
	$(MAKE) -C $(PVACCESS_DIR) clean

install-conda: install-pvapy-epics-conda install-pvapy-boost-conda install-pvapy-conda
	
install-pvapy-epics-conda:
	$(MAKE) -C $(CONDA_DIR)/pvapy-epics-conda install

install-pvapy-boost-conda:
	$(MAKE) -C $(CONDA_DIR)/pvapy-boost-conda install

install-pvapy-conda:
	$(MAKE) -C $(CONDA_DIR)/pvapy-conda install

uninstall-conda: uninstall-pvapy-conda uninstall-pvapy-boost-conda uninstall-pvapy-epics-conda 

uninstall-pvapy-epics-conda:
	$(MAKE) -C $(CONDA_DIR)/pvapy-epics-conda uninstall

uninstall-pvapy-boost-conda:
	$(MAKE) -C $(CONDA_DIR)/pvapy-boost-conda uninstall

uninstall-pvapy-conda:
	$(MAKE) -C $(CONDA_DIR)/pvapy-conda uninstall

clean: srcclean docclean

tidy: distclean
	$(RM) config.log $(RELEASE_LOCAL) $(CONFIG_SITE_LOCAL)
	$(MAKE) -C $(DOC_DIR) tidy

.PHONY: configure distclean
.PHONY: doc docclean srcclean clean tidy
.PHONY: install-conda uninstall-conda

