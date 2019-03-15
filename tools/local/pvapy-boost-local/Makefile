
TOP = ../../..
DEPLOY_CONF = $(TOP)/configure/DEPLOY.conf
BOOST_VERSION=$(shell cat $(DEPLOY_CONF) | grep BOOST_VERSION | cut -f2 -d'=')
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

