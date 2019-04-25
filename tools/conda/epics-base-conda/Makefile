
TOP = ../../..
DEPLOY_CONF = $(TOP)/configure/DEPLOY.conf
EPICS_BASE_VERSION=$(shell cat $(DEPLOY_CONF) | grep EPICS_BASE_VERSION | cut -f2 -d'=')
BUILD_NUMBER=$(shell cat $(DEPLOY_CONF) | grep BUILD_NUMBER | cut -f2 -d'=')
PACKAGE = epics-base
CONDA_LIST_FLAGS := $(CONDA_ENV_FLAGS)
CONDA_ENV_FLAGS += -y

default: package

configure:
	eval "cat meta.yaml.template | sed 's?BUILD_NUMBER?${BUILD_NUMBER}?g' | sed 's?EPICS_BASE_VERSION?${EPICS_BASE_VERSION}?g' > meta.yaml"

build: configure
	conda build .

package: build

install: build
	conda install $(CONDA_ENV_FLAGS) $(PACKAGE) --use-local

uninstall: 
	conda remove $(CONDA_ENV_FLAGS) $(PACKAGE) || echo "Cannot uninstall $(PACKAGE)"

clean: 
	rm -rf build dist

distclean: clean

