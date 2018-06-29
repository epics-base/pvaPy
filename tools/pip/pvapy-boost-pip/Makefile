
TOP = ../../..
DEPLOY_CONF = ${CURDIR}/../../../configure/DEPLOY.conf
DIST_DIR = dist
BUILD_DIR = build
PACKAGE_DIR = pvapy-boost

package: build
	DEPLOY_CONF=$(DEPLOY_CONF) pip wheel . -w $(DIST_DIR) 

configure:

build: configure
	python setup.py build_ext 

clean: 
	rm -rf $(BUILD_DIR) $(DIST_DIR) $(PACKAGE_DIR)

distclean: clean
