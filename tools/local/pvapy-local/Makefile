
TOP = ../../..
BUILD_CONF = ${CURDIR}/../../../configure/BUILD.conf
PVA_PY_VERSION=$(shell cat $(BUILD_CONF) | grep PVA_PY_VERSION | cut -f2 -d'=')

default: install

configure:

build: configure
	./build.linux.sh

package: build

install: build

uninstall:

test: 
	./test.sh

clean:
	rm -rf build dist

distclean: clean

