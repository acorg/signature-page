#include <iostream>
#include <cstdlib>

#include "signature-page.hh"
#include "locationdb/locdb.hh"
#include "seqdb/seqdb.hh"
#include "tree.hh"
#include "tree-export.hh"
#include "tree-draw.hh"
#include "time-series-draw.hh"
#include "settings.hh"
#include "surface-cairo.hh"

// ----------------------------------------------------------------------

SignaturePageDraw::SignaturePageDraw()
    : mSettings{new Settings{}}, mLocdb{new LocDb{}}, mSeqdb{new seqdb::Seqdb{}},
      mTree{new Tree{}} //, mTreeDraw{new TreeDraw{}}, mTimeSeriesDraw{new TimeSeriesDraw{}}
{
    mLocdb->importFrom(std::getenv("ACMACSD_ROOT") + std::string("/data/locationdb.json.xz"));

} // SignaturePageDraw::SignaturePageDraw

// ----------------------------------------------------------------------

// must be declared here to allow proper destruction of unique_ptr
SignaturePageDraw::~SignaturePageDraw()
{
}

// ----------------------------------------------------------------------

void SignaturePageDraw::load_settings(std::string aFilename)
{
    if (!aFilename.empty())
        read_settings(*mSettings, aFilename);

} // SignaturePageDraw::load_settings

// ----------------------------------------------------------------------

Settings& SignaturePageDraw::init_settings(bool layout_tree)
{
    double ratio;
    if (layout_tree) {
        ratio = 1.0 / 0.6;
        mSettings->signature_page.top = 80;
        mSettings->signature_page.left = 50;
        mSettings->signature_page.set_layot("tree-ts-clades");
        mSettings->signature_page.time_series_width = 400;
        mSettings->signature_page.clades_width = 100;
        mSettings->signature_page.tree_margin_right = 50;
        mSettings->tree_draw.aa_transition.per_branch.size = mSettings->signature_page.time_series_width * 0.1;
    }
    else {
        ratio = 1.0 / 1.6;
        mSettings->signature_page.top = 30;
        mSettings->signature_page.left = 20;
        mSettings->signature_page.set_layot("tree-clades-ts-maps");
        mSettings->signature_page.time_series_width = 100;
        mSettings->signature_page.clades_width = 50;
        mSettings->signature_page.tree_margin_right = 10;
        mSettings->tree_draw.aa_transition.per_branch.size = mSettings->signature_page.time_series_width * 0.075;
    }
    mSettings->signature_page.bottom = mSettings->signature_page.top;

    if (mTreeDraw)
        mTreeDraw->init_settings();
    if (mTimeSeriesDraw)
        mTimeSeriesDraw->init_settings();
    if (mCladesDraw)
        mCladesDraw->init_settings();

    mSettings->tree_draw.vaccines[0].label_size = mSettings->signature_page.time_series_width * 0.1;

    mSettings->hz_sections.ts_label_size = mSettings->time_series.label_size;

    const double ts_base = mSettings->signature_page.time_series_width * 0.01;
    mSettings->time_series.label_size = ts_base * 9;
    mSettings->time_series.month_separator_width = ts_base * 0.1;
    mSettings->time_series.month_year_to_timeseries_gap = ts_base * 3;
    mSettings->time_series.dash_line_width = ts_base * 1;

    const double clade_base = mSettings->signature_page.clades_width * 0.1 * ratio;
    mSettings->clades.slot_width = clade_base * 2;
    for (auto& clade: mSettings->clades.clades) {
        clade.label_size = clade_base * 3;
        clade.line_width = clade_base * 0.2;
        clade.arrow_width = clade.line_width * 5;
        clade.label_offset.set(clade_base * 1, 0);
    }

    return *mSettings;

} // SignaturePageDraw::init_settings

// ----------------------------------------------------------------------

void SignaturePageDraw::make_surface(std::string aFilename)
{
    double width = 300, height = 300;
    switch (mSettings->signature_page.layout) {
      case SignaturePageDrawSettings::Layout::TreeTSClades:
          width = 500;
          height = 850;
          break;
      case SignaturePageDrawSettings::Layout::TreeCladesTSMaps:
          width = 1360;         // ratio 1.6
          height = 850;
          break;
    }
    mSurface = std::make_unique<PdfCairo>(aFilename, width, height);

} // SignaturePageDraw::make_surface

// ----------------------------------------------------------------------

void SignaturePageDraw::tree(std::string aTreeFilename, std::string aSeqdbFilename)
{
    tree_import(aTreeFilename, *mTree);
    if (!aSeqdbFilename.empty()) {
        mSeqdb->load(aSeqdbFilename);
        mTree->match_seqdb(*mSeqdb);
    }
    mTree->set_continents(*mLocdb);
      // tree.report_cumulative_edge_length(std::cout);
    mTree->ladderize(Tree::LadderizeMethod::NumberOfLeaves);           // must be before clade_setup
    mTree->make_aa_transitions();

    mTree->add_vaccine("A/SOUTH%20AUCKLAND/17/2016__MDCK%3F/SIAT1", "A/SOUTH AUCKLAND/17/2016 C");

} // SignaturePageDraw::tree

// ----------------------------------------------------------------------

void SignaturePageDraw::prepare()
{
    switch (mSettings->signature_page.layout) {
      case SignaturePageDrawSettings::Layout::TreeTSClades:
          make_layout_tree_ts_clades();
          break;
      case SignaturePageDrawSettings::Layout::TreeCladesTSMaps:
          make_layout_tree_clades_ts_maps();
          break;
    }

    if (mTreeDraw)
        mTreeDraw->prepare();
    if (mTimeSeriesDraw)
        mTimeSeriesDraw->prepare();
    if (mCladesDraw)
        mCladesDraw->prepare();

} // SignaturePageDraw::prepare

// ----------------------------------------------------------------------

void SignaturePageDraw::make_layout_tree_ts_clades()
{
    const Size page_size = mSurface->size();
    const double section_height = page_size.height - (mSettings->signature_page.top + mSettings->signature_page.bottom);

    const double clades_width = mSettings->signature_page.clades_width;
    const double ts_width = mSettings->signature_page.time_series_width;
    const double tree_width = page_size.width - (mSettings->signature_page.left + mSettings->signature_page.tree_margin_right + ts_width + clades_width + mSettings->signature_page.right);

    const double ts_left = mSettings->signature_page.left + tree_width + mSettings->signature_page.tree_margin_right;
    const double clades_left = ts_left + ts_width;

    Surface& tree_draw_surface = mSurface->subsurface({mSettings->signature_page.left, mSettings->signature_page.top}, {tree_width, section_height}, page_size.width, false);
    mTreeDraw = std::make_unique<TreeDraw>(tree_draw_surface, *mTree, mSettings->tree_draw, mSettings->hz_sections);

    Surface& ts_surface = mSurface->subsurface({ts_left, mSettings->signature_page.top}, {ts_width, section_height}, page_size.width * ts_width / tree_width, false);
    mTimeSeriesDraw = std::make_unique<TimeSeriesDraw>(ts_surface, *mTree, *mTreeDraw, mSettings->hz_sections, mSettings->time_series);

    Surface& clades_surface = mSurface->subsurface({clades_left, mSettings->signature_page.top}, {clades_width, section_height}, page_size.width * clades_width / tree_width, false);
    mCladesDraw = std::make_unique<CladesDraw>(clades_surface, *mTree, *mTreeDraw, *mTimeSeriesDraw, mSettings->clades);

} // SignaturePageDraw::make_layout_tree_ts_clades

// ----------------------------------------------------------------------

void SignaturePageDraw::make_layout_tree_clades_ts_maps()
{
    const Size page_size = mSurface->size();
    const double section_height = page_size.height - (mSettings->signature_page.top + mSettings->signature_page.bottom);

    const double clades_width = mSettings->signature_page.clades_width;
    const double ts_width = mSettings->signature_page.time_series_width;
    const double tree_width = page_size.width - (mSettings->signature_page.left + mSettings->signature_page.tree_margin_right + ts_width + clades_width + mSettings->signature_page.right);

    const double clades_left = mSettings->signature_page.left + tree_width + mSettings->signature_page.tree_margin_right;
    const double ts_left = clades_left + clades_width;

    Surface& tree_draw_surface = mSurface->subsurface({mSettings->signature_page.left, mSettings->signature_page.top}, {tree_width, section_height}, page_size.width, false);
    mTreeDraw = std::make_unique<TreeDraw>(tree_draw_surface, *mTree, mSettings->tree_draw, mSettings->hz_sections);

    Surface& ts_surface = mSurface->subsurface({ts_left, mSettings->signature_page.top}, {ts_width, section_height}, page_size.width * ts_width / tree_width, false);
    mTimeSeriesDraw = std::make_unique<TimeSeriesDraw>(ts_surface, *mTree, *mTreeDraw, mSettings->hz_sections, mSettings->time_series);

    Surface& clades_surface = mSurface->subsurface({clades_left, mSettings->signature_page.top}, {clades_width, section_height}, page_size.width * clades_width / tree_width, false);
    mCladesDraw = std::make_unique<CladesDraw>(clades_surface, *mTree, *mTreeDraw, *mTimeSeriesDraw, mSettings->clades);

} // SignaturePageDraw::make_layout_tree_clades_ts_maps

// ----------------------------------------------------------------------

void SignaturePageDraw::draw()
{
    mSurface->background("white");

    if (mTreeDraw)
        mTreeDraw->draw();
    if (mTimeSeriesDraw)
        mTimeSeriesDraw->draw();
    if (mCladesDraw)
        mCladesDraw->draw();

} // SignaturePageDraw::draw

// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
