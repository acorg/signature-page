#include <iostream>
#include <cstdlib>
#include <cmath>

#include "signature-page.hh"
#include "locationdb/locdb.hh"
#include "seqdb/seqdb.hh"
#include "tree.hh"
#include "tree-export.hh"
#include "tree-draw.hh"
#include "time-series-draw.hh"
#include "settings.hh"
#include "acmacs-draw/surface-cairo.hh"
#include "chart.hh"
#include "mapped-antigens-draw.hh"
#include "antigenic-maps-draw.hh"
#include "title-draw.hh"

// ----------------------------------------------------------------------

SignaturePageDraw::SignaturePageDraw()
    : mSettings{new Settings{}}, mLocdb{new LocDb{}}, mSeqdb{new seqdb::Seqdb{}},
      mTree{new Tree{}}
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

SignaturePageDrawSettings::Layout SignaturePageDraw::detect_layout() const
{
    return mSettings->signature_page.layout == SignaturePageDrawSettings::Layout::Auto ? (mChart ? SignaturePageDrawSettings::Layout::TreeCladesTSMaps : SignaturePageDrawSettings::Layout::TreeTSClades) : mSettings->signature_page.layout;

} // SignaturePageDraw::detect_layout

// ----------------------------------------------------------------------

void SignaturePageDraw::make_surface(std::string aFilename)
{
    double width = 300, height = 300;
    switch (detect_layout()) {
      case SignaturePageDrawSettings::Layout::TreeCladesTSMaps:
          width = 1360;         // ratio 1.6
          height = 850;
          break;
      case SignaturePageDrawSettings::Layout::TreeTSClades:
      case SignaturePageDrawSettings::Layout::Auto:
          height = 800;
          width = std::floor(height * (210.0 / 297.0));
          break;
    }
    mSurface = std::make_unique<PdfCairo>(aFilename, width, height);

    mTitleDraw = std::make_unique<TitleDraw>(mSurface->subsurface(false), *mTree, mChart.get(), mSettings->title);
    mTreeDraw = std::make_unique<TreeDraw>(mSurface->subsurface(false), *mTree, mSettings->tree_draw, mSettings->hz_sections);
    mTimeSeriesDraw = std::make_unique<TimeSeriesDraw>(mSurface->subsurface(false), *mTree, *mTreeDraw, mSettings->hz_sections, mSettings->time_series);
    mCladesDraw = std::make_unique<CladesDraw>(mSurface->subsurface(false), *mTree, *mTreeDraw, *mTimeSeriesDraw, mSettings->clades);

    if (mChart) {
        mMappedAntigensDraw = std::make_unique<MappedAntigensDraw>(mSurface->subsurface(false), *mTree, *mChart, mSettings->mapped_antigens);
        mAntigenicMapsDraw = std::make_unique<AntigenicMapsDraw>(mSurface->subsurface(false), *mTree, *mChart, mSettings->hz_sections, *mMappedAntigensDraw, mSettings->antigenic_maps);
    }

} // SignaturePageDraw::make_surface

// ----------------------------------------------------------------------

void SignaturePageDraw::init_layout()
{
    if (!mChart) {
        mSettings->signature_page.top = 60;
        mSettings->signature_page.bottom = 60;
        mSettings->signature_page.left = 50;
        mSettings->signature_page.right = 0;
        mSettings->signature_page.set_layot("tree-ts-clades");
        mSettings->signature_page.time_series_width = 300;
        mSettings->signature_page.clades_width = 60;
        mSettings->signature_page.tree_margin_right = 10;
        // mSettings->tree_draw.aa_transition.per_branch.size = mSettings->signature_page.time_series_width * 0.1;
    }
    else {
        mSettings->signature_page.top = 30;
        mSettings->signature_page.bottom = 30;
        mSettings->signature_page.left = 10;
        mSettings->signature_page.set_layot("tree-clades-ts-maps");
        mSettings->signature_page.time_series_width = 140;
        mSettings->signature_page.clades_width = 20;
        mSettings->signature_page.tree_margin_right = 10;
        mSettings->signature_page.mapped_antigens_margin_right = 10;
        // mSettings->tree_draw.aa_transition.per_branch.size = mSettings->signature_page.time_series_width * 0.075;
    }

} // SignaturePageDraw::init_layout

// ----------------------------------------------------------------------

void SignaturePageDraw::init_settings()
{
    std::cout << std::endl << "INIT:" << std::endl;
    mSettings->signature_page.bottom = mSettings->signature_page.top;

    if (mTitleDraw)
        mTitleDraw->init_settings();
    if (mTreeDraw)
        mTreeDraw->set_line_no(true, true); // hides leaves too
    if (mCladesDraw)
        mCladesDraw->init_settings();
    if (mTreeDraw)
        mTreeDraw->init_settings(mCladesDraw ? mCladesDraw->clades() : nullptr);
    if (mTimeSeriesDraw)
        mTimeSeriesDraw->init_settings();

    if (mSurface->aspect() > 1) { // with maps
        // mSettings->tree_draw.vaccines[0].label_size = 10;
        // mSettings->tree_draw.aa_transition.per_branch.size = 15;
        // mSettings->tree_draw.legend.offset.set(0, 1100);
        mSettings->tree_draw.legend.width = 300;

        // mSettings->time_series.label_size = 18;
        // mSettings->time_series.month_separator_width = 1;
        // mSettings->time_series.month_year_to_timeseries_gap = 1;
        // mSettings->time_series.dash_line_width = 0.8;

        mSettings->hz_sections.vertical_gap = 15;
        // mSettings->hz_sections.ts_label_size = 20;
        // mSettings->hz_sections.line_width = 1;

        mSettings->clades.slot_width = 10;
        for (auto& clade: mSettings->clades.clades) {
            // clade.label_size = 20;
            // clade.line_width = 1;
            // clade.arrow_width = clade.line_width * 5;
            clade.label_offset.set(1, 0);
        }

        if (mAntigenicMapsDraw)
            mAntigenicMapsDraw->init_settings();
    }
    else {                      // just tree
        // mSettings->tree_draw.vaccines[0].label_size = 40;
        // mSettings->tree_draw.aa_transition.per_branch.size = 20;
        // mSettings->tree_draw.legend.offset.set(500, 1900);
        mSettings->tree_draw.legend.width = 300;

        // mSettings->time_series.label_size = 20;
        // mSettings->time_series.month_separator_width = 1;
        // mSettings->time_series.month_year_to_timeseries_gap = 5;
        // mSettings->time_series.dash_line_width = 3;

        mSettings->hz_sections.vertical_gap = 40;
        // mSettings->hz_sections.ts_label_size = 35;
        // mSettings->hz_sections.line_width = 2;
        for (auto& section: mSettings->hz_sections.sections) {
            section.show_label_in_time_series = false;
        }

        mSettings->clades.slot_width = 20;
        for (auto& clade: mSettings->clades.clades) {
            // clade.label_size = 33;
            // clade.line_width = 2;
            // clade.arrow_width = clade.line_width * 5;
            clade.label_offset.set(10, 0);
        }
    }

} // SignaturePageDraw::init_settings

// ----------------------------------------------------------------------

void SignaturePageDraw::tree(std::string aTreeFilename, std::string aSeqdbFilename)
{
    tree_import(aTreeFilename, *mTree);
    if (!aSeqdbFilename.empty()) {
        mSeqdb->load(aSeqdbFilename);
        mTree->match_seqdb(*mSeqdb);
    }
    mTree->set_continents(*mLocdb);
    if (!mSettings->tree_draw.root.empty())
        mTree->re_root(mSettings->tree_draw.root);
      // tree.report_cumulative_edge_length(std::cout);
    mTree->ladderize(Tree::LadderizeMethod::NumberOfLeaves);           // must be before clade_setup
    mTree->make_aa_transitions();

    mTree->add_vaccine("A/SOUTH%20AUCKLAND/17/2016__MDCK%3F/SIAT1", "A/SOUTH AUCKLAND/17/2016 C");

} // SignaturePageDraw::tree

// ----------------------------------------------------------------------

void SignaturePageDraw::chart(std::string aChartFilename)
{
    mChart = std::make_unique<Chart>();
    read_chart_from_sdb(*mChart, aChartFilename);

} // SignaturePageDraw::chart

// ----------------------------------------------------------------------

void SignaturePageDraw::prepare()
{
    std::cout << std::endl << "PREPARE:" << std::endl;
    switch (detect_layout()) {
      case SignaturePageDrawSettings::Layout::TreeTSClades:
      case SignaturePageDrawSettings::Layout::Auto:
          make_layout_tree_ts_clades();
          break;
      case SignaturePageDrawSettings::Layout::TreeCladesTSMaps:
          make_layout_tree_clades_ts_maps();
          break;
    }

    if (mMappedAntigensDraw)
        mMappedAntigensDraw->prepare();
    if (mTitleDraw)
        mTitleDraw->prepare();
    if (mTreeDraw)
        mTreeDraw->prepare();
    if (mTimeSeriesDraw)
        mTimeSeriesDraw->prepare();
    if (mCladesDraw)
        mCladesDraw->prepare();
    if (mChart)
        mChart->prepare(mSettings->antigenic_maps);
    if (mAntigenicMapsDraw)
        mAntigenicMapsDraw->prepare();

} // SignaturePageDraw::prepare

// ----------------------------------------------------------------------

void SignaturePageDraw::make_layout_tree_ts_clades()
{
    const Size& page_size = mSurface->viewport().size;
    const double section_height = page_size.height - (mSettings->signature_page.top + mSettings->signature_page.bottom);

    const double clades_width = mSettings->signature_page.clades_width;
    const double ts_width = mSettings->signature_page.time_series_width;
    const double tree_width = page_size.width - (mSettings->signature_page.left + mSettings->signature_page.tree_margin_right + ts_width + clades_width + mSettings->signature_page.right);

    const double ts_left = mSettings->signature_page.left + tree_width + mSettings->signature_page.tree_margin_right;
    const double clades_left = ts_left + ts_width;

    mTreeDraw->surface().move_resize_viewport({mSettings->signature_page.left, mSettings->signature_page.top}, tree_width, Size{1000, 1000 * section_height / tree_width});
    mTimeSeriesDraw->surface().move_resize_viewport({ts_left, mSettings->signature_page.top}, ts_width, Size{1000, 1000 * section_height / ts_width});
    mCladesDraw->surface().move_resize_viewport({clades_left, mSettings->signature_page.top}, clades_width, Size{1000, 1000 * section_height / clades_width});
    mTitleDraw->surface().move_resize_viewport(Location{}, page_size.width, page_size);

} // SignaturePageDraw::make_layout_tree_ts_clades

// ----------------------------------------------------------------------

void SignaturePageDraw::make_layout_tree_clades_ts_maps()
{
    const Size& page_size = mSurface->viewport().size;
    const double section_height = page_size.height - (mSettings->signature_page.top + mSettings->signature_page.bottom);

    const double antigic_maps_width = mSettings->antigenic_maps.width;
    const double mapped_antigens_width = mSettings->mapped_antigens.width;
    const double clades_width = mSettings->signature_page.clades_width;
    const double ts_width = mSettings->signature_page.time_series_width;
    const double tree_width = page_size.width - (mSettings->signature_page.left + mSettings->signature_page.tree_margin_right + ts_width +
                                                 clades_width + mapped_antigens_width + mSettings->signature_page.mapped_antigens_margin_right +
                                                 antigic_maps_width + mSettings->signature_page.right);

    const double clades_left = mSettings->signature_page.left + tree_width + mSettings->signature_page.tree_margin_right;
    const double ts_left = clades_left + clades_width;
    const double mapped_antigens_left = ts_left + ts_width;
    const double antigic_maps_left = mapped_antigens_left + mapped_antigens_width + mSettings->signature_page.mapped_antigens_margin_right;

    // std::cerr << "page_size " << page_size << std::endl;
    // std::cerr << "tree " << Size{tree_width, section_height} << std::endl;
    // std::cerr << "time series " << Size{ts_width, section_height} << std::endl;
    // std::cerr << "clades " << Size{clades_width, section_height} << std::endl;

    // mTreeDraw->surface().move_resize_viewport({mSettings->signature_page.left, mSettings->signature_page.top}, page_size.width, Size{tree_width, section_height});
    // mTimeSeriesDraw->surface().move_resize_viewport({ts_left, mSettings->signature_page.top}, page_size.width * ts_width / tree_width, Size{ts_width, section_height});
    // mCladesDraw->surface().move_resize_viewport({clades_left, mSettings->signature_page.top}, page_size.width * clades_width / tree_width, Size{clades_width, section_height});
    // mMappedAntigensDraw->surface().move_resize_viewport({mapped_antigens_left, mSettings->signature_page.top}, page_size.width * mapped_antigens_width / tree_width, Size{mapped_antigens_width, section_height});
    // mAntigenicMapsDraw->surface().move_resize_viewport({antigic_maps_left, mSettings->signature_page.top}, page_size.width * antigic_maps_width / tree_width, Size{antigic_maps_width, section_height});
    // mTitleDraw->surface().move_resize_viewport({0, 0}, page_size.width, page_size);

    mTreeDraw->surface().move_resize_viewport({mSettings->signature_page.left, mSettings->signature_page.top}, tree_width, Size{1000, 1000 * section_height / tree_width});
    mTimeSeriesDraw->surface().move_resize_viewport({ts_left, mSettings->signature_page.top}, ts_width, Size{1000, 1000 * section_height / ts_width});
    mCladesDraw->surface().move_resize_viewport({clades_left, mSettings->signature_page.top}, clades_width, Size{1000, 1000 * section_height / clades_width});
    mMappedAntigensDraw->surface().move_resize_viewport({mapped_antigens_left, mSettings->signature_page.top}, mapped_antigens_width, Size{1000, 1000 * section_height / mapped_antigens_width});
    mAntigenicMapsDraw->surface().move_resize_viewport({antigic_maps_left, mSettings->signature_page.top}, antigic_maps_width, Size{1000, 1000 * section_height / antigic_maps_width});
    mTitleDraw->surface().move_resize_viewport(Location{}, page_size.width, page_size);

} // SignaturePageDraw::make_layout_tree_clades_ts_maps

// ----------------------------------------------------------------------

void SignaturePageDraw::draw()
{
    std::cout << std::endl << "DRAW:" << std::endl;
    mSurface->background("white");

    if (mTreeDraw)
        mTreeDraw->draw();
    if (mTimeSeriesDraw)
        mTimeSeriesDraw->draw();
    if (mCladesDraw)
        mCladesDraw->draw();
    if (mMappedAntigensDraw)
        mMappedAntigensDraw->draw();
    if (mAntigenicMapsDraw)
        mAntigenicMapsDraw->draw();
    if (mTitleDraw)
        mTitleDraw->draw();

} // SignaturePageDraw::draw

// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
