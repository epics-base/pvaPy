
TOP = ../../..
BUILD_CONF = ${CURDIR}/../../../configure/BUILD.conf
PVA_PY_VERSION=$(shell cat $(BUILD_CONF) | grep PVA_PY_VERSION | cut -f2 -d'=')
DIST_DIR = dist
BUILD_DIR = build

PACKAGE_DIRS = pvapy pvaccess

package: build
	BUILD_CONF=$(BUILD_CONF) pip wheel . -w $(DIST_DIR) -r requirements.txt

configure:

build: configure
	PVA_PY_VERSION=$(PVA_PY_VERSION) python setup.py build_ext 

clean: 
	rm -rf $(BUILD_DIR) $(DIST_DIR) $(PACKAGE_DIRS) MANIFEST

distclean: clean
