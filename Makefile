# -*- Makefile -*-
# Eugene Skepner 2016
# ======================================================================

MAKEFLAGS = -w

# ----------------------------------------------------------------------

SIGNATURE_PAGE_SOURCES = tree.cc tree-export.cc \
			 sdb-chart.cc sdb.cc \
			 signature-page.cc tree-draw.cc time-series-draw.cc clades-draw.cc \
			 mapped-antigens-draw.cc \
			 antigenic-maps-draw.cc ace-antigenic-maps-draw.cc sdb-antigenic-maps-draw.cc \
			 title-draw.cc coloring.cc settings.cc
SIGNATURE_PAGE_PY_SOURCES = py.cc $(SIGNATURE_PAGE_SOURCES)
SIGP_SOURCES = sigp.cc $(SIGNATURE_PAGE_SOURCES)
SETTINGS_CREATE_SOURCES = settings-create.cc settings.cc
TEST_SETTINGS_COPY_SOURCES = test-settings-copy.cc settings.cc
TEST_DRAW_CHART_SOURCES = test-draw-chart.cc $(SIGNATURE_PAGE_SOURCES)

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

PYTHON_VERSION = $(shell python3 -c 'import sys; print("{0.major}.{0.minor}".format(sys.version_info))')
PYTHON_CONFIG = python$(PYTHON_VERSION)-config
PYTHON_MODULE_SUFFIX = $(shell $(PYTHON_CONFIG) --extension-suffix)
PYTHON_LD_LIB = $$($(PYTHON_CONFIG) --ldflags | sed -E 's/-Wl,-stack_size,[0-9]+//')

PKG_INCLUDES = $$(pkg-config --cflags cairo) $$(pkg-config --cflags liblzma) $$($(PYTHON_CONFIG) --includes)

LIB_DIR = $(ACMACSD_ROOT)/lib
ACMACSD_LIBS = -L$(LIB_DIR) -lacmacsbase -lacmacschart -lacmacsdraw -lseqdb -lhidb -llocationdb -lboost_date_time -lboost_program_options -lboost_filesystem -lboost_system
SETTINGS_CREATE_LDLIBS = $(ACMACSD_LIBS) $$(pkg-config --libs liblzma)
SIGP_LDLIBS = $(ACMACSD_LIBS) $$(pkg-config --libs cairo) $$(pkg-config --libs liblzma)

# ----------------------------------------------------------------------

BUILD = build
DIST = $(abspath dist)

# ----------------------------------------------------------------------

all: check-python $(DIST)/signature_page_backend$(PYTHON_MODULE_SUFFIX) $(DIST)/sigp-settings-create $(DIST)/test-settings-copy $(DIST)/sigp $(DIST)/test-draw-chart

# ----------------------------------------------------------------------

# $(DIST)/: $(patsubst %.cc,$(BUILD)/%.o,$(_SOURCES)) | $(DIST) check-acmacsd-root
#	g++ $(LDFLAGS) -o $@ $^

$(DIST)/signature_page_backend$(PYTHON_MODULE_SUFFIX):  $(patsubst %.cc,$(BUILD)/%.o,$(SIGNATURE_PAGE_PY_SOURCES)) | $(DIST) check-acmacsd-root
	g++ -shared $(LDFLAGS) -o $@ $^ $(SIGP_LDLIBS) $(PYTHON_LD_LIB)

$(DIST)/sigp-settings-create: $(patsubst %.cc,$(BUILD)/%.o,$(SETTINGS_CREATE_SOURCES)) | $(DIST)
	g++ $(LDFLAGS) -o $@ $^ $(SETTINGS_CREATE_LDLIBS)

$(DIST)/test-settings-copy: $(patsubst %.cc,$(BUILD)/%.o,$(TEST_SETTINGS_COPY_SOURCES)) | $(DIST)
	g++ $(LDFLAGS) -o $@ $^ $(SETTINGS_CREATE_LDLIBS)

$(DIST)/test-draw-chart: $(patsubst %.cc,$(BUILD)/%.o,$(TEST_DRAW_CHART_SOURCES)) | $(DIST)
	g++ $(LDFLAGS) -o $@ $^ $(SIGP_LDLIBS)

$(DIST)/sigp: $(patsubst %.cc,$(BUILD)/%.o,$(SIGP_SOURCES)) | $(DIST)
	g++ $(LDFLAGS) -o $@ $^ $(SIGP_LDLIBS)

# ----------------------------------------------------------------------

install: $(DIST)/signature_page_backend$(PYTHON_MODULE_SUFFIX) $(DIST)/sigp | check-acmacsd-root
	@#ln -sf $(abspath bin)/sigp-* $(ACMACSD_ROOT)/bin
	ln -sf $(DIST)/sigp $(ACMACSD_ROOT)/bin
	ln -sf $(abspath py)/* $(ACMACSD_ROOT)/py
	ln -sf $(DIST)/signature_page_backend$(PYTHON_MODULE_SUFFIX) $(ACMACSD_ROOT)/py

clean:
	rm -rf $(DIST) $(BUILD)/*.o $(BUILD)/*.d

distclean: clean
	rm -rf $(BUILD)

test: install $(DIST)/sigp
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
