# -*- Makefile -*-
# Eugene Skepner 2017
# ======================================================================

MAKEFLAGS = -w

# ----------------------------------------------------------------------

TARGETS = \
	$(DIST)/sigp-settings-create \
	$(DIST)/test-settings-copy \
	$(DIST)/sigp

SIGNATURE_PAGE_SOURCES = tree.cc tree-export.cc \
			 signature-page.cc tree-draw.cc time-series-draw.cc clades-draw.cc \
			 mapped-antigens-draw.cc antigenic-maps-layout.cc \
			 antigenic-maps-draw.cc ace-antigenic-maps-draw.cc \
			 title-draw.cc coloring.cc settings.cc
SIGP_SOURCES = sigp.cc $(SIGNATURE_PAGE_SOURCES)
SETTINGS_CREATE_SOURCES = settings-create.cc settings.cc
TEST_SETTINGS_COPY_SOURCES = test-settings-copy.cc settings.cc
# TEST_DRAW_CHART_SOURCES = test-draw-chart.cc $(SIGNATURE_PAGE_SOURCES)

# ----------------------------------------------------------------------

include $(ACMACSD_ROOT)/share/makefiles/Makefile.g++
include $(ACMACSD_ROOT)/share/makefiles/Makefile.dist-build.vars

CXXFLAGS = -MMD -g $(OPTIMIZATION) $(PROFILE) -fPIC -std=$(STD) $(WARNINGS) -Icc -I$(AD_INCLUDE) $(PKG_INCLUDES)
LDFLAGS = $(OPTIMIZATION) $(PROFILE)

PKG_INCLUDES = $(shell pkg-config --cflags cairo) $(shell pkg-config --cflags liblzma)

ACMACSD_LIBS = \
	$(AD_LIB)/$(call shared_lib_name,libacmacsbase,1,0) \
	$(AD_LIB)/$(call shared_lib_name,liblocationdb,1,0) \
	$(AD_LIB)/$(call shared_lib_name,libacmacschart,2,0) \
	$(AD_LIB)/$(call shared_lib_name,libhidb,5,0) \
	$(AD_LIB)/$(call shared_lib_name,libseqdb,2,0) \
	$(AD_LIB)/$(call shared_lib_name,libacmacsdraw,1,0) \
	$(AD_LIB)/$(call shared_lib_name,libacmacsmapdraw,2,0) \
	-L$(AD_LIB) -lboost_date_time $(FS_LIB)

SETTINGS_CREATE_LDLIBS = $(ACMACSD_LIBS) $(shell pkg-config --libs liblzma)
LDLIBS = $(ACMACSD_LIBS) $(shell pkg-config --libs cairo) $(shell pkg-config --libs liblzma)

# ----------------------------------------------------------------------

all: $(TARGETS)

install: $(TARGETS)
	$(call install_py_lib,$(SIGNATURE_PAGE_PY_LIB))
	@#ln -sf $(abspath bin)/sigp-* $(AD_BIN)
	ln -sf $(DIST)/sigp $(AD_BIN)
	ln -sf $(abspath py)/* $(AD_PY)

test: install $(DIST)/sigp
	test/test

# ----------------------------------------------------------------------

-include $(BUILD)/*.d
include $(ACMACSD_ROOT)/share/makefiles/Makefile.dist-build.rules
include $(ACMACSD_ROOT)/share/makefiles/Makefile.rtags

# ----------------------------------------------------------------------

$(SIGNATURE_PAGE_PY_LIB): $(patsubst %.cc,$(BUILD)/%.o,$(SIGNATURE_PAGE_PY_SOURCES)) | $(DIST)
	@printf "%-16s %s\n" "SHARED" $@
	@$(call make_shared,$(SIGNATURE_PAGE_PY_LIB_NAME),$(SIGNATURE_PAGE_PY_LIB_MAJOR),$(SIGNATURE_PAGE_PY_LIB_MINOR)) $(LDFLAGS) -o $@ $^ $(LDLIBS) $(PYTHON_LDLIBS)

$(DIST)/sigp-settings-create: $(patsubst %.cc,$(BUILD)/%.o,$(SETTINGS_CREATE_SOURCES)) | $(DIST)
	@printf "%-16s %s\n" "LINK" $@
	@$(CXX) $(LDFLAGS) -o $@ $^ $(SETTINGS_CREATE_LDLIBS)

$(DIST)/test-settings-copy: $(patsubst %.cc,$(BUILD)/%.o,$(TEST_SETTINGS_COPY_SOURCES)) | $(DIST)
	@printf "%-16s %s\n" "LINK" $@
	@$(CXX) $(LDFLAGS) -o $@ $^ $(SETTINGS_CREATE_LDLIBS)

$(DIST)/sigp: $(patsubst %.cc,$(BUILD)/%.o,$(SIGP_SOURCES)) | $(DIST)
	@printf "%-16s %s\n" "LINK" $@
	@$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

# ======================================================================
### Local Variables:
### eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
### End:
