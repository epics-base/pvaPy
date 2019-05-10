
TOP = ../../..
BUILD_CONF = $(TOP)/configure/BUILD.conf
include $(BUILD_CONF)

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

