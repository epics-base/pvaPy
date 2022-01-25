
TOP = ../../..
BUILD_CONF = ${CURDIR}/../../../configure/BUILD.conf
DIST_DIR = dist
BUILD_DIR = build
PACKAGE_DIR = pvapy-boost

package: build
	BUILD_CONF=$(BUILD_CONF) pip wheel . -w $(DIST_DIR) 

configure:

build: configure
	python setup.py build_ext 

clean: 
	rm -rf $(BUILD_DIR) $(PACKAGE_DIR) *.egg-info

tidy: clean
	rm -rf $(DIST_DIR) 

distclean: tidy
