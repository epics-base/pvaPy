
TOP = ../../..
BUILD_CONF = $(TOP)/configure/BUILD.conf
EPICS_BASE_VERSION=$(shell cat $(BUILD_CONF) | grep EPICS_BASE_VERSION | cut -f2 -d'=')

default: install

configure:

build: configure
	./build.linux.sh

package: build

install: build

uninstall: 

clean: 
	rm -rf build dist

distclean: clean

