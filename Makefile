SUBDIRS=common architecture_characterization trace_inspector t_test_high_order

.PHONY: subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@
clean:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
		$(MAKE) -C $$dir clean_images; \
	done
