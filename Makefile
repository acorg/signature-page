# -*- Makefile -*-
# Eugene Skepner 2016
# ======================================================================

MAKEFLAGS = -w

# ----------------------------------------------------------------------

DRAW_SOURCES = surface-cairo.cc
SIGNATURE_PAGE_SOURCES = tree.cc tree-export.cc tree-draw.cc $(DRAW_SOURCES)
SIGNATURE_PAGE_CC_PY_SOURCES = py.cc $(SIGNATURE_PAGE_SOURCES)
TEST_CAIRO_SOURCES = test-cairo.cc $(DRAW_SOURCES)
TEST_CAIRO_FONTS_SOURCES = test-cairo-fonts.cc $(DRAW_SOURCES)
TEST_DRAW_TREE_SOURCES = test-draw-tree.cc $(SIGNATURE_PAGE_SOURCES)

# ----------------------------------------------------------------------

CLANG = $(shell if g++ --version 2>&1 | grep -i llvm >/dev/null; then echo Y; else echo N; fi)
ifeq ($(CLANG),Y)
  WEVERYTHING = -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded
  WARNINGS = -Wno-weak-vtables # -Wno-padded
  STD = c++14
else
  WEVERYTHING = -Wall -Wextra
  WARNINGS =
  STD = c++14
endif

# -fvisibility=hidden and -flto make resulting lib smaller (pybind11) but linking is much slower
OPTIMIZATION = -O3 #-fvisibility=hidden -flto
PROFILE = # -pg
CXXFLAGS = -MMD -g $(OPTIMIZATION) $(PROFILE) -fPIC -std=$(STD) $(WEVERYTHING) $(WARNINGS) -Icc -I$(BUILD)/include -I$(ACMACSD_ROOT)/include $(PKG_INCLUDES)
LDFLAGS = $(OPTIMIZATION) $(PROFILE)

LIB_DIR = $(ACMACSD_ROOT)/lib
SIGP_LDLIBS = -L$(LIB_DIR) -lacmacsbase -lseqdb -lhidb $$(pkg-config --libs cairo) $$(pkg-config --libs liblzma) $(PYTHON_LD_LIB)
TEST_CAIRO_LDLIBS = -L$(LIB_DIR) -lacmacsbase $$(pkg-config --libs cairo)
TEST_DRAW_TREE_LDLIBS = -L$(LIB_DIR) -lacmacsbase -lseqdb -lhidb $$(pkg-config --libs cairo) $$(pkg-config --libs liblzma)

PYTHON_VERSION = $(shell python3 -c 'import sys; print("{0.major}.{0.minor}".format(sys.version_info))')
PYTHON_CONFIG = python$(PYTHON_VERSION)-config
PYTHON_MODULE_SUFFIX = $(shell $(PYTHON_CONFIG) --extension-suffix)
PYTHON_LD_LIB = $$($(PYTHON_CONFIG) --ldflags | sed -E 's/-Wl,-stack_size,[0-9]+//')

PKG_INCLUDES = $$(pkg-config --cflags cairo) $$($(PYTHON_CONFIG) --includes)

# ----------------------------------------------------------------------

BUILD = build
DIST = $(abspath dist)

# ----------------------------------------------------------------------

all: check-python $(DIST)/signature_page_cc$(PYTHON_MODULE_SUFFIX) $(DIST)/test-cairo $(DIST)/test-cairo-fonts $(DIST)/test-draw-tree

# ----------------------------------------------------------------------

# $(DIST)/: $(patsubst %.cc,$(BUILD)/%.o,$(_SOURCES)) | $(DIST) check-acmacsd-root
#	g++ $(LDFLAGS) -o $@ $^

$(DIST)/signature_page_cc$(PYTHON_MODULE_SUFFIX):  $(patsubst %.cc,$(BUILD)/%.o,$(SIGNATURE_PAGE_CC_PY_SOURCES)) | $(DIST) check-acmacsd-root
	g++ -shared $(LDFLAGS) -o $@ $^ $(SIGP_LDLIBS)

$(DIST)/test-cairo: $(patsubst %.cc,$(BUILD)/%.o,$(TEST_CAIRO_SOURCES)) | $(DIST)
	g++ $(LDFLAGS) -o $@ $^ $(TEST_CAIRO_LDLIBS)

$(DIST)/test-cairo-fonts: $(patsubst %.cc,$(BUILD)/%.o,$(TEST_CAIRO_FONTS_SOURCES)) | $(DIST)
	g++ $(LDFLAGS) -o $@ $^ $(TEST_CAIRO_LDLIBS)

$(DIST)/test-draw-tree: $(patsubst %.cc,$(BUILD)/%.o,$(TEST_DRAW_TREE_SOURCES)) | $(DIST)
	g++ $(LDFLAGS) -o $@ $^ $(TEST_DRAW_TREE_LDLIBS)

# ----------------------------------------------------------------------

install: $(DIST)/signature_page_cc$(PYTHON_MODULE_SUFFIX) | check-acmacsd-root
	ln -sf $(abspath bin)/sigp-* $(ACMACSD_ROOT)/bin
	ln -sf $(abspath py)/* $(ACMACSD_ROOT)/py
	ln -sf $(DIST)/signature_page_cc$(PYTHON_MODULE_SUFFIX) $(ACMACSD_ROOT)/py

clean:
	rm -rf $(DIST) $(BUILD)/*.o $(BUILD)/*.d

distclean: clean
	rm -rf $(BUILD)

test: install $(DIST)/test-cairo $(DIST)/test-cairo-fonts $(DIST)/test-draw-tree
	test/test

# ----------------------------------------------------------------------

-include $(BUILD)/*.d

# ----------------------------------------------------------------------

$(BUILD)/%.o: cc/%.cc | $(BUILD)
	@echo $<
	@g++ $(CXXFLAGS) -c -o $@ $<

# ----------------------------------------------------------------------

check-acmacsd-root:
ifndef ACMACSD_ROOT
	$(error ACMACSD_ROOT is not set)
endif

check-python:
	@printf 'import sys\nif sys.version_info < (3, 5):\n print("Python 3.5 is required")\n exit(1)' | python3

$(DIST):
	mkdir -p $(DIST)

$(BUILD):
	mkdir -p $(BUILD)

.PHONY: check-acmacsd-root check-python

# ======================================================================
### Local Variables:
### eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
### End:
