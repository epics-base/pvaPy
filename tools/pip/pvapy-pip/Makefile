
TOP = ../../..
BUILD_CONF = ${CURDIR}/../../../configure/BUILD.conf
include $(BUILD_CONF)
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
	rm -rf $(BUILD_DIR) $(PACKAGE_DIRS) MANIFEST *.egg-info

tidy: clean
	rm -rf $(DIST_DIR) 

distclean: tidy
