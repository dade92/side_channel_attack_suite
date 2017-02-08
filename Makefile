SUBDIRS=common architecture_characterization trace_inspector t_test_high_order filter trace_aligner aes

.PHONY: subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@
clean:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done
clean_images:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean_images; \
	done
