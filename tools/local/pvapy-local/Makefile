
TOP = ../../..
DEPLOY_CONF = ${CURDIR}/../../../configure/DEPLOY.conf
PVA_PY_VERSION=$(shell cat $(DEPLOY_CONF) | grep PVA_PY_VERSION | cut -f2 -d'=')

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

