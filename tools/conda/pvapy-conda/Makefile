
TOP = ../../..
BUILD_CONF = $(TOP)/configure/BUILD.conf
include $(BUILD_CONF)

ifeq ($(PVAPY_VERSION),)
    PVAPY_VERSION=$(shell git rev-parse --short HEAD)
endif

PACKAGE = pvapy
CONDA_BUILD_FLAGS += EPICS_BASE_VERSION=${EPICS_BASE_VERSION}
CONDA_BUILD_FLAGS += BOOST_VERSION=${BOOST_VERSION}
CONDA_BUILD_FLAGS += PVAPY_VERSION=${PVAPY_VERSION}
CONDA_BUILD_FLAGS += PVAPY_GIT_VERSION=${PVAPY_GIT_VERSION}
CONDA_BUILD_FLAGS += PVAPY_USE_CPP11=${PVAPY_USE_CPP11}
CONDA_ENV_FLAGS += -y

default: package

configure:

build: configure
	$(CONDA_BUILD_FLAGS) BUILD_FLAGS=${BUILD_FLAGS} conda build -c local .

package: build

install: build
	conda install $(CONDA_ENV_FLAGS) $(PACKAGE) --use-local

uninstall: 
	conda remove $(CONDA_ENV_FLAGS) $(PACKAGE) || echo "Cannot uninstall $(PACKAGE)"

clean:  
	rm -rf build dist

distclean: clean

