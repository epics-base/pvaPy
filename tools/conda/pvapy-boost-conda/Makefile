
TOP = ../../..
DEPLOY_CONF = $(TOP)/configure/DEPLOY.conf
BOOST_VERSION=$(shell cat $(DEPLOY_CONF) | grep BOOST_VERSION | cut -f2 -d'=')
BOOST_DOWNLOAD_VERSION=$(shell echo $(BOOST_VERSION) | sed 's?\.?_?g')
PACKAGE = pvapy-boost
CONDA_ENV_FLAGS += -y

configure:
	eval "cat meta.yaml.template | sed 's?BOOST_VERSION?${BOOST_VERSION}?g' | sed 's?BOOST_DOWNLOAD_VERSION?${BOOST_DOWNLOAD_VERSION}?g' > meta.yaml"

build: configure
	conda build .

package: build

install: build
	conda install $(CONDA_ENV_FLAGS) $(PACKAGE) --use-local

uninstall: 
	conda remove $(CONDA_ENV_FLAGS) $(PACKAGE) 

clean:  uninstall

distclean: clean

