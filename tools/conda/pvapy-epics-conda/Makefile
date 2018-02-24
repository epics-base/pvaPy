
TOP = ../../..
DEPLOY_CONF = $(TOP)/configure/DEPLOY.conf
EPICS_BASE_VERSION=$(shell cat $(DEPLOY_CONF) | grep EPICS_BASE_VERSION | cut -f2 -d'=')
PACKAGE = pvapy-epics
CONDA_LIST_FLAGS := $(CONDA_ENV_FLAGS)
CONDA_ENV_FLAGS += -y

configure:
	conda list $(CONDA_LIST_FLAGS) conda-build | grep conda-build || conda install $(CONDA_ENV_FLAGS) conda-build
	eval "cat meta.yaml.template | sed 's?EPICS_BASE_VERSION?${EPICS_BASE_VERSION}?g' > meta.yaml"

build: configure
	conda build .

install: build
	conda install $(CONDA_ENV_FLAGS) $(PACKAGE) --use-local

uninstall: 
	conda remove $(CONDA_ENV_FLAGS) $(PACKAGE)

clean: uninstall

distclean: clean

