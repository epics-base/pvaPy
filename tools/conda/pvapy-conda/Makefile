
TOP = ../../..
BUILD_CONF = $(TOP)/configure/BUILD.conf
include $(BUILD_CONF)

ifeq ($(PVAPY_VERSION),)
    PVAPY_VERSION=$(shell git rev-parse --short HEAD)
endif
PVAPY_YAML_TEMPLATE ?= meta.yaml.template.local

PACKAGE = pvapy
CONDA_ENV_FLAGS += -y

default: package

configure:
	eval "cat ${PVAPY_YAML_TEMPLATE} | sed 's?BUILD_NUMBER?${BUILD_NUMBER}?g' | sed 's?PVAPY_VERSION?${PVAPY_VERSION}?g' | sed 's?PVAPY_GIT_VERSION?${PVAPY_GIT_VERSION}?g' | sed 's?EPICS_BASE_VERSION?${EPICS_BASE_VERSION}?g' | sed 's?BOOST_VERSION?${BOOST_VERSION}?g' > meta.yaml"

build: configure
	BUILD_FLAGS=${BUILD_FLAGS} conda build .

package: build

install: build
	conda install $(CONDA_ENV_FLAGS) $(PACKAGE) --use-local

uninstall: 
	conda remove $(CONDA_ENV_FLAGS) $(PACKAGE) || echo "Cannot uninstall $(PACKAGE)"

clean:  
	rm -rf build dist

distclean: clean

