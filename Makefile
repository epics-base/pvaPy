TOP = .

AC_DIR = tools/autoconf
DOC_DIR = documentation
CONFIGURE_DIR = configure
PVACCESS_DIR = src/pvaccess
CONDA_DIR = tools/conda
PIP_DIR = tools/pip
LOCAL_DIR = tools/local
DIST_DIR = dist
WHEEL_DIR = wheelhouse
TEST_DIR = test
PYTHON_VERSION ?= 3

RELEASE_LOCAL = $(CONFIGURE_DIR)/RELEASE.local
CONFIG_SITE_LOCAL = $(CONFIGURE_DIR)/CONFIG_SITE.local

RM ?= rm -f
RMDIR ?= rm -rf

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

  configure: $(AC_DIR)/configure
	@$(RM) $(RELEASE_LOCAL) $(CONFIG_SITE_LOCAL)
	 PYTHON_VERSION=$(PYTHON_VERSION) $(AC_DIR)/configure --with-top=$(TOP)

  $(AC_DIR)/configure: $(AC_DIR)/configure.ac $(wildcard $(AC_DIR)/m4/*.m4)
	autoreconf --install $(AC_DIR)

  distclean:
	$(RM) config.log $(RELEASE_LOCAL) $(CONFIG_SITE_LOCAL)
	$(RMDIR) bin lib dist $(PVACCESS_DIR)/O.* $(AC_DIR)/autom4te.cache
	$(RM) $(AC_DIR)/aclocal.m4 $(AC_DIR)/configure $(AC_DIR)/config.log
	$(RM) $(AC_DIR)/config.status $(AC_DIR)/install-sh $(AC_DIR)/missing
	$(RM) $(AC_DIR)/Makefile $(AC_DIR)/Makefile.in config.log
	$(RMDIR) $(AC_DIR)/compile
	$(MAKE) -C $(DOC_DIR) distclean
	$(MAKE) -C $(CONDA_DIR) distclean
	$(MAKE) -C $(PIP_DIR) distclean
	$(MAKE) -C $(LOCAL_DIR) distclean
	$(RMDIR) $(CONFIGURE_DIR)/O.*

endif # Command-line goal

doc:
	PYTHONPATH=../.. $(MAKE) -C $(DOC_DIR)

doc-clean:
	$(MAKE) -C $(DOC_DIR) clean

src-clean:
	$(MAKE) -C $(PVACCESS_DIR) clean

package-pip pip: package-epics-base-pip package-pvapy-boost-pip package-pvapy-pip
	
package-epics-base-pip:
	$(MAKE) -C $(PIP_DIR)/epics-base-pip package

package-pvapy-boost-pip:
	$(MAKE) -C $(PIP_DIR)/pvapy-boost-pip package

package-pvapy-pip:
	$(MAKE) -C $(PIP_DIR)/pvapy-pip package

pip-clean:
	$(MAKE) -C $(PIP_DIR)/epics-base-pip clean
	$(MAKE) -C $(PIP_DIR)/pvapy-boost-pip clean
	$(MAKE) -C $(PIP_DIR)/pvapy-pip clean

pip-test:
	$(MAKE) -C $(PIP_DIR)/pvapy-pip test

package-local local: package-epics-base-local package-pvapy-boost-local package-pvapy-local
	
package-epics-base-local:
	$(MAKE) -C $(LOCAL_DIR)/epics-base-local package

package-pvapy-boost-local:
	$(MAKE) -C $(LOCAL_DIR)/pvapy-boost-local package

package-pvapy-local:
	$(MAKE) -C $(LOCAL_DIR)/pvapy-local package

local-clean:
	$(MAKE) -C $(LOCAL_DIR)/epics-base-local clean
	$(MAKE) -C $(LOCAL_DIR)/pvapy-boost-local clean
	$(MAKE) -C $(LOCAL_DIR)/pvapy-local clean

package-conda conda: package-epics-base-conda package-pvapy-boost-conda package-pvapy-conda
	
package-epics-base-conda:
	$(MAKE) -C $(CONDA_DIR)/epics-base-conda package

package-pvapy-boost-conda:
	$(MAKE) -C $(CONDA_DIR)/pvapy-boost-conda package

package-pvapy-conda:
	$(MAKE) -C $(CONDA_DIR)/pvapy-conda package

install-conda: install-epics-base-conda install-pvapy-boost-conda install-pvapy-conda
	
install-epics-base-conda:
	$(MAKE) -C $(CONDA_DIR)/epics-base-conda install

install-pvapy-boost-conda:
	$(MAKE) -C $(CONDA_DIR)/pvapy-boost-conda install

install-pvapy-conda:
	$(MAKE) -C $(CONDA_DIR)/pvapy-conda install

uninstall-conda: uninstall-pvapy-conda uninstall-pvapy-boost-conda uninstall-epics-base-conda 

uninstall-epics-base-conda:
	$(MAKE) -C $(CONDA_DIR)/epics-base-conda uninstall

uninstall-pvapy-boost-conda:
	$(MAKE) -C $(CONDA_DIR)/pvapy-boost-conda uninstall

uninstall-pvapy-conda:
	$(MAKE) -C $(CONDA_DIR)/pvapy-conda uninstall

conda-clean: 
	$(MAKE) -C $(CONDA_DIR)/pvapy-conda clean || /bin/true
	$(MAKE) -C $(CONDA_DIR)/pvapy-boost-conda clean || /bin/true
	$(MAKE) -C $(CONDA_DIR)/epics-base-conda clean || /bin/true

tests: 
	$(MAKE) -C $(TEST_DIR)

clean: src-clean doc-clean pip-clean conda-clean

tidy: distclean
	$(RM) config.log $(RELEASE_LOCAL) $(CONFIG_SITE_LOCAL)
	$(RMDIR) $(WHEEL_DIR)
	$(MAKE) -C $(DOC_DIR) tidy

.PHONY: configure distclean
.PHONY: doc doc-clean src-clean pip-clean conda-clean local-clean clean tidy
.PHONY: package-pip pip package-conda conda install-conda uninstall-conda

