BUILD_DIR = _build
STATIC_DIR = _static
TEMPLATES_DIR = _templates

SPHINX_MAKEFILE   = Makefile.sphinx
SPHINX_BUILD_PATH = $(PVA_PY_SPHINX_BUILD)
ifeq ($(wildcard $(SPHINX_BUILD_PATH)),)
    SPHINX_BUILD_PATH = $(shell which sphinx-build 2> /dev/null)
endif

ifeq ($(wildcard $(SPHINX_BUILD_PATH)),)
    DOC_TARGETS = skip_doc_build
else
    DOC_TARGETS = html 
endif


default: $(DOC_TARGETS)

.PHONY: help clean html dirhtml pickle json htmlhelp qthelp latex changes linkcheck doctest

# Override sphinx targets where needed
help clean:
	$(MAKE) -f $(SPHINX_MAKEFILE) $@
html dirhtml pickle json htmlhelp qthelp latex changes linkcheck doctest: dirs
	export PATH=`dirname $(SPHINX_BUILD_PATH)`:$(PATH) && $(MAKE) -f $(SPHINX_MAKEFILE) $@

# Other targets
configure:

distclean: 
	rm -rf $(BUILD_DIR) $(STATIC_DIR) $(TEMPLATES_DIR) 

tidy: distclean

dirs: $(BUILD_DIR) $(STATIC_DIR) $(TEMPLATES_DIR) 

$(BUILD_DIR) $(STATIC_DIR) $(TEMPLATES_DIR):
	mkdir -p $@

skip_doc_build:
	@echo 
	@echo "*************************************"
	@echo "Warning:"
	@echo "    The sphinx-build executable could not be found."
	@echo "    Documentation will not be generated."
	@echo "*************************************"
	@echo 

