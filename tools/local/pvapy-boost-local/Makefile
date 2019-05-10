
TOP = ../../..
BUILD_CONF = $(TOP)/configure/BUILD.conf
include $(BUILD_CONF)

BOOST_DOWNLOAD_VERSION=$(shell echo $(BOOST_VERSION) | sed 's?\.?_?g')

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

