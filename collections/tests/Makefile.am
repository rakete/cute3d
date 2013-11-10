#AM_CFLAGS = $(DEPS_CFLAGS) -I ../src/ -L../src/ -L.
AM_CFLAGS = -std=gnu99 -Wall -pedantic -g -ggdb -pg -O0 -D_DEBUG -DUNICODE -D_XOPEN_SOURCE -D_XOPEN_SOURCE_EXTENDED -I ../src/
LDADD     = -lm $(top_builddir)/lib/.libs/libcollections.a


bin_PROGRAMS = \
$(top_builddir)/bin/test-array \
$(top_builddir)/bin/test-bench-mark \
$(top_builddir)/bin/test-bheap \
$(top_builddir)/bin/test-bitset \
$(top_builddir)/bin/test-dlist \
$(top_builddir)/bin/test-flat-db \
$(top_builddir)/bin/test-hash-map \
$(top_builddir)/bin/test-hash-table \
$(top_builddir)/bin/test-lhash-table \
$(top_builddir)/bin/test-pvector \
$(top_builddir)/bin/test-pbheap \
$(top_builddir)/bin/test-slist \
$(top_builddir)/bin/test-tstring \
$(top_builddir)/bin/test-rbtree \
$(top_builddir)/bin/test-tree-map \
$(top_builddir)/bin/test-variant \
$(top_builddir)/bin/test-vector

__top_builddir__bin_test_array_SOURCES       = test-array.c
__top_builddir__bin_test_bench_mark_SOURCES  = test-bench-mark.c
__top_builddir__bin_test_bheap_SOURCES       = test-bheap.c
__top_builddir__bin_test_bitset_SOURCES      = test-bitset.c
__top_builddir__bin_test_dlist_SOURCES       = test-dlist.c
__top_builddir__bin_test_flat_db_SOURCES     = test-flat-db.c
__top_builddir__bin_test_hash_map_SOURCES    = test-hash-map.c
__top_builddir__bin_test_hash_table_SOURCES  = test-hash-table.c
__top_builddir__bin_test_lhash_table_SOURCES = test-lhash-table.c
__top_builddir__bin_test_pvector_SOURCES     = test-pvector.c
__top_builddir__bin_test_pbheap_SOURCES      = test-pbheap.c
__top_builddir__bin_test_tstring_SOURCES     = test-tstring.c
__top_builddir__bin_test_slist_SOURCES       = test-slist.c
__top_builddir__bin_test_rbtree_SOURCES      = test-rbtree.c
__top_builddir__bin_test_tree_map_SOURCES    = test-tree-map.c
__top_builddir__bin_test_vector_SOURCES      = test-vector.c
__top_builddir__bin_test_variant_SOURCES     = test-variant.c
