
TOP = ../../..
DEPLOY_CONF = $(TOP)/configure/DEPLOY.conf
PVA_PY_VERSION=$(shell cat $(DEPLOY_CONF) | grep PVA_PY_VERSION | cut -f2 -d'=')
PACKAGE = pvapy
CONDA_ENV_FLAGS += -y

configure:
	eval "cat meta.yaml.template | sed 's?PVA_PY_VERSION?${PVA_PY_VERSION}?g' > meta.yaml"

build: configure
	conda build .

install: build
	conda install $(CONDA_ENV_FLAGS) $(PACKAGE) --use-local

uninstall: 
	conda remove $(CONDA_ENV_FLAGS) $(PACKAGE) 

clean:  uninstall

distclean: clean

