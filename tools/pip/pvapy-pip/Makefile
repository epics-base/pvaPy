
TOP = ../../..
BUILD_CONF = ${CURDIR}/../../../configure/BUILD.conf
PVAPY_VERSION=$(shell cat $(BUILD_CONF) | grep PVAPY_VERSION | cut -f2 -d'=')
DIST_DIR = dist
BUILD_DIR = build

PACKAGE_DIRS = pvapy pvaccess

package: build
	BUILD_CONF=$(BUILD_CONF) pip wheel . -w $(DIST_DIR) -r requirements.txt

configure:

build: configure
	PVAPY_VERSION=$(PVAPY_VERSION) python setup.py build_ext 

test: 
	./run_test.sh
clean: 
	rm -rf $(BUILD_DIR) $(DIST_DIR) $(PACKAGE_DIRS) MANIFEST

distclean: clean
