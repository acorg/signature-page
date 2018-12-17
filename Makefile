# -*- Makefile -*-
# ======================================================================

TARGETS = \
  $(DIST)/sigp \
  $(DIST)/sigp-settings-create \
  $(DIST)/test-settings-copy \
  $(DIST)/make-isig \
  $(DIST)/tree-aa-info \
  $(DIST)/tree-text \
  $(DIST)/tree-chart-sections

SIGNATURE_PAGE_SOURCES = \
  tree.cc tree-export.cc \
  signature-page.cc tree-draw.cc time-series-draw.cc clades-draw.cc \
  mapped-antigens-draw.cc aa-at-pos-draw.cc antigenic-maps-layout.cc \
  antigenic-maps-draw.cc ace-antigenic-maps-draw.cc \
  title-draw.cc coloring.cc settings.cc settings-initializer.cc

SIGP_SOURCES = sigp.cc $(SIGNATURE_PAGE_SOURCES)
SETTINGS_CREATE_SOURCES = settings-create.cc  $(SIGNATURE_PAGE_SOURCES)
TEST_SETTINGS_COPY_SOURCES = test-settings-copy.cc $(SIGNATURE_PAGE_SOURCES)
# TEST_DRAW_CHART_SOURCES = test-draw-chart.cc $(SIGNATURE_PAGE_SOURCES)

MAKE_ISIG_SOURCES = make-isig.cc tree.cc tree-export.cc
TREE_AA_INFO_SOURCES = tree-aa-info.cc tree.cc tree-export.cc
TREE_TEXT_SOURCES = tree-text.cc tree.cc tree-export.cc
TREE_CHART_SECTIONS_SOURCES = tree-chart-sections.cc tree.cc tree-export.cc

# ----------------------------------------------------------------------

all: install

CONFIGURE_BOOST = 1
CONFIGURE_CAIRO = 1
include $(ACMACSD_ROOT)/share/Makefile.config

ACMACSD_LIBS = \
  $(AD_LIB)/$(call shared_lib_name,libacmacsbase,1,0) \
  $(AD_LIB)/$(call shared_lib_name,liblocationdb,1,0) \
  $(AD_LIB)/$(call shared_lib_name,libacmacschart,2,0) \
  $(AD_LIB)/$(call shared_lib_name,libhidb,5,0) \
  $(AD_LIB)/$(call shared_lib_name,libseqdb,2,0) \
  $(AD_LIB)/$(call shared_lib_name,libacmacsdraw,1,0) \
  $(AD_LIB)/$(call shared_lib_name,libacmacsmapdraw,2,0) \
  $(L_BOOST) -lboost_date_time $(CXX_LIBS)

SETTINGS_CREATE_LDLIBS = $(ACMACSD_LIBS) $(XZ_LIBS)
LDLIBS = $(ACMACSD_LIBS) $(CAIRO_LIBS) $(XZ_LIBS)

# ----------------------------------------------------------------------

install: $(TARGETS)
	$(call install_py_lib,$(SIGNATURE_PAGE_PY_LIB))
	$(call symbolic_link,$(DIST)/sigp,$(AD_BIN))
	$(call symbolic_link,$(DIST)/make-isig,$(AD_BIN))
	$(call symbolic_link_wildcard,$(abspath bin)/sigp-*,$(AD_BIN))
	$(call symbolic_link_wildcard,$(DIST)/tree-*,$(AD_BIN))

test: install $(DIST)/sigp
	test/test
.PHONY: test

# ----------------------------------------------------------------------

$(SIGNATURE_PAGE_PY_LIB): $(patsubst %.cc,$(BUILD)/%.o,$(SIGNATURE_PAGE_PY_SOURCES)) | $(DIST)
	$(call echo_shared_lib,$@)
	$(call make_shared_lib,$(SIGNATURE_PAGE_PY_LIB_NAME),$(SIGNATURE_PAGE_PY_LIB_MAJOR),$(SIGNATURE_PAGE_PY_LIB_MINOR)) $(LDFLAGS) -o $@ $^ $(LDLIBS) $(PYTHON_LIBS)

$(DIST)/sigp-settings-create: $(patsubst %.cc,$(BUILD)/%.o,$(SETTINGS_CREATE_SOURCES)) | $(DIST)
	$(call echo_link_exe,$@)
	$(CXX) $(LDFLAGS) -o $@ $^ $(SETTINGS_CREATE_LDLIBS) $(AD_RPATH)

$(DIST)/test-settings-copy: $(patsubst %.cc,$(BUILD)/%.o,$(TEST_SETTINGS_COPY_SOURCES)) | $(DIST)
	$(call echo_link_exe,$@)
	$(CXX) $(LDFLAGS) -o $@ $^ $(SETTINGS_CREATE_LDLIBS) $(AD_RPATH)

$(DIST)/sigp: $(patsubst %.cc,$(BUILD)/%.o,$(SIGP_SOURCES)) | $(DIST)
	$(call echo_link_exe,$@)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS) $(AD_RPATH)

$(DIST)/make-isig: $(patsubst %.cc,$(BUILD)/%.o,$(MAKE_ISIG_SOURCES)) | $(DIST)
	$(call echo_link_exe,$@)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS) $(AD_RPATH)

$(DIST)/tree-aa-info: $(patsubst %.cc,$(BUILD)/%.o,$(TREE_AA_INFO_SOURCES)) | $(DIST)
	$(call echo_link_exe,$@)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS) $(AD_RPATH)

$(DIST)/tree-text: $(patsubst %.cc,$(BUILD)/%.o,$(TREE_TEXT_SOURCES)) | $(DIST)
	$(call echo_link_exe,$@)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS) $(AD_RPATH)

$(DIST)/tree-chart-sections: $(patsubst %.cc,$(BUILD)/%.o,$(TREE_CHART_SECTIONS_SOURCES)) | $(DIST)
	$(call echo_link_exe,$@)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS) $(AD_RPATH)

# ======================================================================
### Local Variables:
### eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
### End:
