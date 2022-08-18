VALGRIND_FLAGS = --tool=cachegrind --branch-sim=yes -q

BUILD_DIR ?= .

.PHONY: cache

cache: cg_annotate.bucket.out cg_annotate.qsort.out
	./cache_report.sh $^ | tee report.txt

cachegrind.%.out: all
	valgrind $(VALGRIND_FLAGS) --cachegrind-out-file=$@ $(BUILD_DIR)/$(TARGET) $*

cg_annotate.%.out: cachegrind.%.out
	cg_annotate $< > $@
