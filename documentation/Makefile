TOP = ..
CONFIG_SITE_LOCAL = $(strip $(wildcard $(TOP)/configure/CONFIG_SITE.local))

ifneq ($(CONFIG_SITE_LOCAL),)

    include $(CONFIG_SITE_LOCAL)
    PVAPY_SETUP = $(strip $(wildcard $(PVAPY_SETUP_SH)))

    ifneq ($(PVAPY_SETUP),)
    default: 
	. $(PVAPY_SETUP); $(MAKE) -C sphinx default
    %: 
	. $(PVAPY_SETUP); $(MAKE) -C sphinx $@
    else
    default: 
	$(MAKE) -C sphinx default
    %: 
	$(MAKE) -C sphinx $@
    endif
else

    default: 
	$(MAKE) -C sphinx default
    %: 
	$(MAKE) -C sphinx $@

endif

