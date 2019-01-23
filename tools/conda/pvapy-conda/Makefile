
TOP = ../../..
DEPLOY_CONF = $(TOP)/configure/DEPLOY.conf
PVA_PY_VERSION=$(shell cat $(DEPLOY_CONF) | grep PVA_PY_VERSION | cut -f2 -d'=')
ifeq ($(PVA_PY_VERSION),)
    PVA_PY_VERSION=$(shell git rev-parse --short HEAD)
endif
PVA_PY_GIT_VERSION=$(shell cat $(DEPLOY_CONF) | grep PVA_PY_GIT_VERSION | cut -f2 -d'=')
  

PACKAGE = pvapy
CONDA_ENV_FLAGS += -y

configure:
	eval "cat meta.yaml.template | sed 's?PVA_PY_VERSION?${PVA_PY_VERSION}?g' | sed 's?PVA_PY_GIT_VERSION?${PVA_PY_GIT_VERSION}?g' | sed 's?EPICS_BASE_VERSION?${EPICS_BASE_VERSION}?g' | sed 's?BOOST_VERSION?${BOOST_VERSION}?g' > meta.yaml"

build: configure
	conda build .

package: build

install: build
	conda install $(CONDA_ENV_FLAGS) $(PACKAGE) --use-local

uninstall: 
	conda remove $(CONDA_ENV_FLAGS) $(PACKAGE) 

clean:  uninstall

distclean: clean

