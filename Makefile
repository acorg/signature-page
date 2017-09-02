# -*- Makefile -*-
# Eugene Skepner 2016
# ======================================================================

MAKEFLAGS = -w

# ----------------------------------------------------------------------

SIGNATURE_PAGE_SOURCES = tree.cc tree-export.cc \
			 sdb-chart.cc sdb.cc \
			 signature-page.cc tree-draw.cc time-series-draw.cc clades-draw.cc \
			 mapped-antigens-draw.cc antigenic-maps-layout.cc \
			 antigenic-maps-draw.cc ace-antigenic-maps-draw.cc sdb-antigenic-maps-draw.cc \
			 title-draw.cc coloring.cc settings.cc
SIGNATURE_PAGE_PY_SOURCES = py.cc $(SIGNATURE_PAGE_SOURCES)
SIGP_SOURCES = sigp.cc $(SIGNATURE_PAGE_SOURCES)
SETTINGS_CREATE_SOURCES = settings-create.cc settings.cc
TEST_SETTINGS_COPY_SOURCES = test-settings-copy.cc settings.cc
TEST_DRAW_CHART_SOURCES = test-draw-chart.cc $(SIGNATURE_PAGE_SOURCES)

# ----------------------------------------------------------------------

TARGET_ROOT=$(shell if [ -f /Volumes/rdisk/ramdisk-id ]; then echo /Volumes/rdisk/AD; else echo $(ACMACSD_ROOT); fi)
include $(TARGET_ROOT)/share/Makefile.g++
include $(TARGET_ROOT)/share/Makefile.dist-build.vars

# -fvisibility=hidden and -flto make resulting lib smaller (pybind11) but linking is much slower
OPTIMIZATION = -O3 #-fvisibility=hidden -flto
PROFILE = # -pg
CXXFLAGS = -MMD -g $(OPTIMIZATION) $(PROFILE) -fPIC -std=$(STD) $(WEVERYTHING) $(WARNINGS) -Icc -I$(AD_INCLUDE) $(PKG_INCLUDES)
LDFLAGS = $(OPTIMIZATION) $(PROFILE)

PYTHON_VERSION = $(shell python3 -c 'import sys; print("{0.major}.{0.minor}".format(sys.version_info))')
PYTHON_CONFIG = python$(PYTHON_VERSION)-config
PYTHON_MODULE_SUFFIX = $(shell $(PYTHON_CONFIG) --extension-suffix)
PYTHON_LD_LIB = $(shell $(PYTHON_CONFIG) --ldflags | sed -E 's/-Wl,-stack_size,[0-9]+//')

PKG_INCLUDES = $(shell pkg-config --cflags cairo) $(shell pkg-config --cflags liblzma) $(shell $(PYTHON_CONFIG) --includes)

ACMACSD_LIBS = $(FS_LIB) -L$(AD_LIB) -lacmacsbase -lacmacschart -lacmacsdraw -lacmacsmapdraw -lseqdb -lhidb -llocationdb -lboost_date_time -lboost_program_options -lboost_filesystem -lboost_system
SETTINGS_CREATE_LDLIBS = $(ACMACSD_LIBS) $(shell pkg-config --libs liblzma)
SIGP_LDLIBS = $(ACMACSD_LIBS) $(shell pkg-config --libs cairo) $(shell pkg-config --libs liblzma)

# ----------------------------------------------------------------------

all: check-python $(DIST)/signature_page_backend$(PYTHON_MODULE_SUFFIX) $(DIST)/sigp-settings-create $(DIST)/test-settings-copy $(DIST)/sigp $(DIST)/test-draw-chart

# ----------------------------------------------------------------------

# $(DIST)/: $(patsubst %.cc,$(BUILD)/%.o,$(_SOURCES)) | $(DIST) check-acmacsd-root
#	$(CXX) $(LDFLAGS) -o $@ $^

$(DIST)/signature_page_backend$(PYTHON_MODULE_SUFFIX):  $(patsubst %.cc,$(BUILD)/%.o,$(SIGNATURE_PAGE_PY_SOURCES)) | $(DIST) check-acmacsd-root
	@echo "SHARED     " $@ # '<--' $^
	@$(CXX) -shared $(LDFLAGS) -o $@ $^ $(SIGP_LDLIBS) $(PYTHON_LD_LIB)

$(DIST)/sigp-settings-create: $(patsubst %.cc,$(BUILD)/%.o,$(SETTINGS_CREATE_SOURCES)) | $(DIST)
	@echo "LINK       " $@ # '<--' $^
	@$(CXX) $(LDFLAGS) -o $@ $^ $(SETTINGS_CREATE_LDLIBS)

$(DIST)/test-settings-copy: $(patsubst %.cc,$(BUILD)/%.o,$(TEST_SETTINGS_COPY_SOURCES)) | $(DIST)
	@echo "LINK       " $@ # '<--' $^
	@$(CXX) $(LDFLAGS) -o $@ $^ $(SETTINGS_CREATE_LDLIBS)

$(DIST)/test-draw-chart: $(patsubst %.cc,$(BUILD)/%.o,$(TEST_DRAW_CHART_SOURCES)) | $(DIST)
	@echo "LINK       " $@ # '<--' $^
	@$(CXX) $(LDFLAGS) -o $@ $^ $(SIGP_LDLIBS)

$(DIST)/sigp: $(patsubst %.cc,$(BUILD)/%.o,$(SIGP_SOURCES)) | $(DIST)
	@echo "LINK       " $@ # '<--' $^
	@$(CXX) $(LDFLAGS) -o $@ $^ $(SIGP_LDLIBS)

# ----------------------------------------------------------------------

install: $(DIST)/signature_page_backend$(PYTHON_MODULE_SUFFIX) $(DIST)/sigp | check-acmacsd-root
	@#ln -sf $(abspath bin)/sigp-* $(AD_BIN)
	ln -sf $(DIST)/sigp $(AD_BIN)
	ln -sf $(abspath py)/* $(AD_PY)
	ln -sf $(DIST)/signature_page_backend$(PYTHON_MODULE_SUFFIX) $(AD_PY)

test: install $(DIST)/sigp
	test/test

include $(AD_SHARE)/Makefile.rtags

# ----------------------------------------------------------------------

-include $(BUILD)/*.d

# ----------------------------------------------------------------------

# abspath below is to show full file path by __FILE__ macro used in logging
$(BUILD)/%.o: cc/%.cc | $(BUILD)
	@echo $(CXX_NAME) $(OPTIMIZATION) $<
	@$(CXX) $(CXXFLAGS) -c -o $@ $(abspath $<)

# ----------------------------------------------------------------------

include $(AD_SHARE)/Makefile.dist-build.rules

# ======================================================================
### Local Variables:
### eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
### End:
