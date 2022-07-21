
TOP = ../../..
BUILD_CONF = $(TOP)/configure/BUILD.conf
include $(BUILD_CONF)

PACKAGE = epics-base
CONDA_LIST_FLAGS := $(CONDA_ENV_FLAGS)
CONDA_BUILD_FLAGS += BUILD_NUMBER=${BUILD_NUMBER}
CONDA_BUILD_FLAGS += EPICS_BASE_VERSION=${EPICS_BASE_VERSION}
CONDA_ENV_FLAGS += -y

default: package

configure:
	#eval "cat meta.yaml.template | sed 's?BUILD_NUMBER?${BUILD_NUMBER}?g' | sed 's?EPICS_BASE_VERSION?${EPICS_BASE_VERSION}?g' > meta.yaml"

build: configure
	$(CONDA_BUILD_FLAGS) conda build .

package: build

install: build
	conda install $(CONDA_ENV_FLAGS) $(PACKAGE) --use-local

uninstall: 
	conda remove $(CONDA_ENV_FLAGS) $(PACKAGE) || echo "Cannot uninstall $(PACKAGE)"

clean: 
	rm -rf build dist

distclean: clean

