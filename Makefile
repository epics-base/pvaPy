TOP = .
RELEASE_LOCAL = $(TOP)/configure/RELEASE.local
#$(shell if [ ! -z $$EPICS_DIR ]; then echo "HERE" ; cmd="cat $(RELEASE_LOCAL) | sed 's/EPICS_DIR=.*/EPICS_DIR=$$EPICS_DIR/g' > $(RELEASE_LOCAL).2 && mv $(RELEASE_LOCAL).2 $(RELEASE_LOCAL)"; echo $$cmd; fi)

include $(TOP)/configure/CONFIG
#DIRS := $(DIRS) $(filter-out $(DIRS), $(wildcard *src*))
#DIRS := $(DIRS) $(filter-out $(DIRS), $(wildcard *Src*))
DIRS := src
include $(TOP)/configure/RULES_DIRS

