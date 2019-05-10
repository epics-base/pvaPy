
TOP = ../../..
BUILD_CONF = $(TOP)/configure/BUILD.conf
include $(BUILD_CONF)

BOOST_DOWNLOAD_VERSION=$(shell echo $(BOOST_VERSION) | sed 's?\.?_?g')

PACKAGE = pvapy-boost
CONDA_ENV_FLAGS += -y

default: package

configure:
	eval "cat meta.yaml.template | sed 's?BUILD_NUMBER?${BUILD_NUMBER}?g' | sed 's?BOOST_VERSION?${BOOST_VERSION}?g' | sed 's?BOOST_DOWNLOAD_VERSION?${BOOST_DOWNLOAD_VERSION}?g' > meta.yaml"

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

