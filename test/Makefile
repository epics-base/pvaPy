TOP            = ..
PACKAGE        = pvapy
TEST_TARGETS   = test
TEST_FLAGS     ?= -rx
TEST_PY_FILES  = $(shell find . -name 'test*.py' | grep -v testUtility | grep -v testServer.py)
ALL_PY_FILES   = $(shell find . -name '*.py')
PYTHONPATH     ?= $(shell python -c "import $(PACKAGE)" || echo $(TOP))

.PHONY: test clean distclean tidy 

default: $(TEST_TARGETS)

test: $(ALL_PY_FILES) 
	PYTHONPATH=$(PYTHONPATH):$(TOP) python testServer.py 30 &
	PYTHONPATH=$(PYTHONPATH):$(TOP) pytest $(TEST_FLAGS) $(TEST_PY_FILES)

clean: 

distclean: clean

tidy: distclean

