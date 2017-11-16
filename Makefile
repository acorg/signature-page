# -*- Makefile -*-
# Eugene Skepner 2017
# ======================================================================

MAKEFLAGS = -w

# ----------------------------------------------------------------------

SIGNATURE_PAGE_SOURCES = tree.cc tree-export.cc \
			 signature-page.cc tree-draw.cc time-series-draw.cc clades-draw.cc \
			 mapped-antigens-draw.cc antigenic-maps-layout.cc \
			 antigenic-maps-draw.cc ace-antigenic-maps-draw.cc \
			 title-draw.cc coloring.cc settings.cc
SIGNATURE_PAGE_PY_SOURCES = py.cc $(SIGNATURE_PAGE_SOURCES)
SIGP_SOURCES = sigp.cc $(SIGNATURE_PAGE_SOURCES)
SETTINGS_CREATE_SOURCES = settings-create.cc settings.cc
TEST_SETTINGS_COPY_SOURCES = test-settings-copy.cc settings.cc
# TEST_DRAW_CHART_SOURCES = test-draw-chart.cc $(SIGNATURE_PAGE_SOURCES)

# ----------------------------------------------------------------------

include $(ACMACSD_ROOT)/share/makefiles/Makefile.g++
include $(ACMACSD_ROOT)/share/makefiles/Makefile.dist-build.vars

CXXFLAGS = -MMD -g $(OPTIMIZATION) $(PROFILE) -fPIC -std=$(STD) $(WARNINGS) -Icc -I$(AD_INCLUDE) $(PKG_INCLUDES)
LDFLAGS = $(OPTIMIZATION) $(PROFILE)

PYTHON_VERSION = $(shell python3 -c 'import sys; print("{0.major}.{0.minor}".format(sys.version_info))')
PYTHON_CONFIG = python$(PYTHON_VERSION)-config
PYTHON_MODULE_SUFFIX = $(shell $(PYTHON_CONFIG) --extension-suffix)
PYTHON_LD_LIB = $(shell $(PYTHON_CONFIG) --ldflags | sed -E 's/-Wl,-stack_size,[0-9]+//')

PKG_INCLUDES = $(shell pkg-config --cflags cairo) $(shell pkg-config --cflags liblzma) $(shell $(PYTHON_CONFIG) --includes)

ACMACSD_LIBS = \
	$(AD_LIB)/$(call shared_lib_name,libacmacsbase,1,0) \
	$(AD_LIB)/$(call shared_lib_name,liblocationdb,1,0) \
	$(AD_LIB)/$(call shared_lib_name,libacmacschart,1,0) \
	$(AD_LIB)/$(call shared_lib_name,libhidb,1,0) \
	-L$(AD_LIB) -lacmacsdraw -lacmacsmapdraw -lseqdb -lboost_date_time -lboost_program_options $(FS_LIB)

SETTINGS_CREATE_LDLIBS = $(ACMACSD_LIBS) $(shell pkg-config --libs liblzma)
SIGP_LDLIBS = $(ACMACSD_LIBS) $(shell pkg-config --libs cairo) $(shell pkg-config --libs liblzma)

# ----------------------------------------------------------------------

all: $(DIST)/signature_page_backend$(PYTHON_MODULE_SUFFIX) $(DIST)/sigp-settings-create $(DIST)/test-settings-copy $(DIST)/sigp

# ----------------------------------------------------------------------

# $(DIST)/: $(patsubst %.cc,$(BUILD)/%.o,$(_SOURCES)) | $(DIST) check-acmacsd-root
#	$(CXX) $(LDFLAGS) -o $@ $^

$(DIST)/signature_page_backend$(PYTHON_MODULE_SUFFIX):  $(patsubst %.cc,$(BUILD)/%.o,$(SIGNATURE_PAGE_PY_SOURCES)) | $(DIST) check-acmacsd-root
	@printf "%-16s %s\n" "SHARED" $@
	@$(CXX) -shared $(LDFLAGS) -o $@ $^ $(SIGP_LDLIBS) $(PYTHON_LD_LIB)

$(DIST)/sigp-settings-create: $(patsubst %.cc,$(BUILD)/%.o,$(SETTINGS_CREATE_SOURCES)) | $(DIST)
	@printf "%-16s %s\n" "LINK" $@
	@$(CXX) $(LDFLAGS) -o $@ $^ $(SETTINGS_CREATE_LDLIBS)

$(DIST)/test-settings-copy: $(patsubst %.cc,$(BUILD)/%.o,$(TEST_SETTINGS_COPY_SOURCES)) | $(DIST)
	@printf "%-16s %s\n" "LINK" $@
	@$(CXX) $(LDFLAGS) -o $@ $^ $(SETTINGS_CREATE_LDLIBS)

# $(DIST)/test-draw-chart: $(patsubst %.cc,$(BUILD)/%.o,$(TEST_DRAW_CHART_SOURCES)) | $(DIST)
#	@printf "%-16s %s\n" "LINK" $@
#	@$(CXX) $(LDFLAGS) -o $@ $^ $(SIGP_LDLIBS)

$(DIST)/sigp: $(patsubst %.cc,$(BUILD)/%.o,$(SIGP_SOURCES)) | $(DIST)
	@printf "%-16s %s\n" "LINK" $@
	@$(CXX) $(LDFLAGS) -o $@ $^ $(SIGP_LDLIBS)

# ----------------------------------------------------------------------

install: $(DIST)/signature_page_backend$(PYTHON_MODULE_SUFFIX) $(DIST)/sigp | check-acmacsd-root
	@#ln -sf $(abspath bin)/sigp-* $(AD_BIN)
	ln -sf $(DIST)/sigp $(AD_BIN)
	ln -sf $(abspath py)/* $(AD_PY)
	ln -sf $(DIST)/signature_page_backend$(PYTHON_MODULE_SUFFIX) $(AD_PY)

test: install $(DIST)/sigp
	test/test

# ----------------------------------------------------------------------

-include $(BUILD)/*.d
include $(ACMACSD_ROOT)/share/makefiles/Makefile.dist-build.rules
include $(ACMACSD_ROOT)/share/makefiles/Makefile.rtags

# ======================================================================
### Local Variables:
### eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
### End:
