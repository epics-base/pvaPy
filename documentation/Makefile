TOP = ..
CONFIG_SITE_LOCAL = $(strip $(wildcard $(TOP)/configure/CONFIG_SITE.local))

ifneq ($(CONFIG_SITE_LOCAL),)

    include $(CONFIG_SITE_LOCAL)
    PVA_PY_SETUP = $(strip $(wildcard $(PVA_PY_SETUP_SH)))

    ifneq ($(PVA_PY_SETUP),)
    default: 
	source $(PVA_PY_SETUP); $(MAKE) -C sphinx default
    %: 
	source $(PVA_PY_SETUP); $(MAKE) -C sphinx $@
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

