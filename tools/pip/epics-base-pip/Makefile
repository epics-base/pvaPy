
TOP = ../../..
BUILD_CONF = ${CURDIR}/../../../configure/BUILD.conf
DIST_DIR = dist
BUILD_DIR = build
PACKAGE_DIR = epics-base

package: build
	BUILD_CONF=$(BUILD_CONF) pip wheel . -w $(DIST_DIR) 

configure:

build: configure
	python setup.py build_ext 

clean: 
	rm -rf $(BUILD_DIR) $(DIST_DIR) $(PACKAGE_DIR)

distclean: clean
