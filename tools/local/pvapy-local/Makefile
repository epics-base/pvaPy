
TOP = ../../..
BUILD_CONF = ${CURDIR}/../../../configure/BUILD.conf
include $(BUILD_CONF)

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

