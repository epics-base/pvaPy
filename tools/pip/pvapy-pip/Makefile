
TOP = ../../..
DEPLOY_CONF = ${CURDIR}/../../../configure/DEPLOY.conf
DIST_DIR = dist
BUILD_DIR = build
PACKAGE_DIRS = pvapy pvaccess

package: build
	DEPLOY_CONF=$(DEPLOY_CONF) pip wheel . -w $(DIST_DIR) -r requirements.txt

configure:

build: configure
	python setup.py build_ext 

clean: 
	rm -rf $(BUILD_DIR) $(DIST_DIR) $(PACKAGE_DIRS) MANIFEST

distclean: clean
