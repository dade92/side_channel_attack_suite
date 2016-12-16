SUBDIRS=architecture_characterization trace_inspector t_test_high_order

.PHONY: subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@
