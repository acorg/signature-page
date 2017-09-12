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

void SignaturePageDraw::load_settings(const std::vector<std::string>& aFilenames)
{
    for (const auto& filename: aFilenames)
        load_settings(filename);

} // SignaturePageDraw::load_settings

// ----------------------------------------------------------------------

void SignaturePageDraw::load_settings(std::string aFilename)
{
    if (!aFilename.empty())
        read_settings(*mSettings, aFilename);

} // SignaturePageDraw::load_settings

// ----------------------------------------------------------------------

SignaturePageDrawSettings::Layout SignaturePageDraw::detect_layout(bool init_settings) const
{
    const auto layout = mSettings->signature_page.get_layout();
    return (layout == SignaturePageDrawSettings::Layout::Auto || init_settings) ? (mChartFilename.empty() ? SignaturePageDrawSettings::Layout::TreeTSClades : SignaturePageDrawSettings::Layout::TreeCladesTSMaps) : layout;

} // SignaturePageDraw::detect_layout

// ----------------------------------------------------------------------

void SignaturePageDraw::make_surface(std::string aFilename, bool init_settings, bool draw_map)
{
    double width = 300, height = 300;
    switch (detect_layout(init_settings)) {
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
    mSurface = std::make_unique<PdfCairo>(draw_map ? aFilename : std::string{}, width, height);
    std::cerr << "INFO: Surface: " << width << " x " << height << std::endl;

    mTreeDraw = std::make_unique<TreeDraw>(mSurface->subsurface(false), *mTree, mSettings->tree_draw, mSettings->hz_sections);
    mTimeSeriesDraw = std::make_unique<TimeSeriesDraw>(mSurface->subsurface(false), *mTree, *mTreeDraw, mSettings->hz_sections, mSettings->time_series);
    mCladesDraw = std::make_unique<CladesDraw>(mSurface->subsurface(false), *mTree, *mTreeDraw, *mTimeSeriesDraw, mSettings->clades);

    if (!mChartFilename.empty()) {
        mAntigenicMapsDraw = std::unique_ptr<AntigenicMapsDrawBase>(make_antigenic_maps_draw(mChartFilename, mSurface->subsurface(false), *mTree, mSettings->hz_sections, mSettings->signature_page, mSettings->antigenic_maps));
        mMappedAntigensDraw = std::make_unique<MappedAntigensDraw>(mSurface->subsurface(false), *mTree, mAntigenicMapsDraw->chart(), mSettings->mapped_antigens);
        mTitleDraw = std::make_unique<TitleDraw>(mSurface->subsurface(false), *mTree, &mAntigenicMapsDraw->chart(), mSettings->title);
    }
    else {
        mTitleDraw = std::make_unique<TitleDraw>(mSurface->subsurface(false), *mTree, nullptr, mSettings->title);
    }

} // SignaturePageDraw::make_surface

// ----------------------------------------------------------------------

void SignaturePageDraw::init_layout()
{
    if (mChartFilename.empty()) {
        mSettings->signature_page.top = 60;
        mSettings->signature_page.bottom = 60;
        mSettings->signature_page.left = 50;
        mSettings->signature_page.right = 0;
        mSettings->signature_page.layout = "tree-ts-clades";
        mSettings->signature_page.time_series_width = 300;
        mSettings->signature_page.clades_width = 100;
        mSettings->signature_page.tree_margin_right = 10;
    }
    else {
        mSettings->signature_page.top = 23;
        mSettings->signature_page.bottom = 23;
        mSettings->signature_page.left = 10;
        mSettings->signature_page.layout = "tree-clades-ts-maps";
        mSettings->signature_page.time_series_width = 140;
        mSettings->signature_page.clades_width = 20;
        mSettings->signature_page.tree_margin_right = 10;
        mSettings->signature_page.mapped_antigens_margin_right = 10;
    }

    // std::cerr << "init_layout signature_page " << mSettings->signature_page.to_json() << '\n';

} // SignaturePageDraw::init_layout

// ----------------------------------------------------------------------

void SignaturePageDraw::init_settings()
{
    std::cerr << std::endl << "INFO: INIT:" << std::endl;
    std::cerr << "signature_page.top " << mSettings->signature_page.top << '\n';
    mSettings->signature_page.bottom = mSettings->signature_page.top;
    std::cerr << "signature_page.bottom " << mSettings->signature_page.bottom << '\n';

    if (mTitleDraw)
        mTitleDraw->init_settings();
    if (mTreeDraw)
        mTreeDraw->ladderize(); // ladderize and set_line_no before init clades!
    if (mCladesDraw)
        mCladesDraw->init_settings();
    if (mTreeDraw)
        mTreeDraw->init_settings(mCladesDraw ? mCladesDraw->clades() : nullptr);
    if (mTimeSeriesDraw)
        mTimeSeriesDraw->init_settings();

    if (mSurface->aspect() > 1) { // with maps
        mSettings->tree_draw.legend.width = 100;
        mSettings->hz_sections.vertical_gap = 15;
        mSettings->clades.slot_width = 10;
        for (auto clade: mSettings->clades.clades) {
            clade.label_offset = Size{1, 0};
        }

        if (mAntigenicMapsDraw)
            mAntigenicMapsDraw->init_settings();
    }
    else {                      // just tree
        mSettings->tree_draw.legend.width = 180;
        mSettings->hz_sections.vertical_gap = 15;
        // for (auto& section: mSettings->hz_sections.sections) {
        //     section.show_label_in_time_series = false;
        // }

        mSettings->clades.slot_width = 10;
        for (auto clade: mSettings->clades.clades) {
            clade.label_offset = Size{10, 0};
        }
        if (mTreeDraw)
            mTreeDraw->detect_hz_lines_for_clades(mCladesDraw ? mCladesDraw->clades() : nullptr, true);
    }

} // SignaturePageDraw::init_settings

// ----------------------------------------------------------------------

void SignaturePageDraw::write_initialized_settings(std::string aFilename)
{
    auto layout = settings().signature_page.get_layout();
    if (layout == SignaturePageDrawSettings::Layout::Auto)
        layout = mSurface->aspect() > 1 ? SignaturePageDrawSettings::Layout::TreeCladesTSMaps : SignaturePageDrawSettings::Layout::TreeTSClades;
    switch (layout) {
      case SignaturePageDrawSettings::Layout::Auto:
          break;
      case SignaturePageDrawSettings::Layout::TreeCladesTSMaps:
          settings().tree_draw.remove_for_signature_page_settings();
          settings().time_series.remove();
          settings().clades.remove();
          settings().hz_sections.remove();
          break;
      case SignaturePageDrawSettings::Layout::TreeTSClades:
          settings().antigenic_maps.remove();
          settings().mapped_antigens.remove();
          break;
    }
    write_settings(settings(), aFilename);

} // SignaturePageDraw::write_initialized_settings

// ----------------------------------------------------------------------

void SignaturePageDraw::tree(std::string aTreeFilename, std::string aSeqdbFilename)
{
    tree_import(aTreeFilename, *mTree);
    if (!aSeqdbFilename.empty()) {
        mSeqdb->load(aSeqdbFilename);
        mTree->match_seqdb(*mSeqdb);
    }

    mTree->add_vaccine("A/SOUTH%20AUCKLAND/17/2016__MDCK%3F/SIAT1", "A/SOUTH AUCKLAND/17/2016 C");

} // SignaturePageDraw::tree

// ----------------------------------------------------------------------

void SignaturePageDraw::prepare()
{
    std::cout << std::endl << "PREPARE:" << std::endl;
    switch (detect_layout(false)) {
      case SignaturePageDrawSettings::Layout::TreeTSClades:
      case SignaturePageDrawSettings::Layout::Auto:
          make_layout_tree_ts_clades();
          if (mTimeSeriesDraw)
              mTimeSeriesDraw->tree_mode(true);
          //   // hide hz section lables in time series
          // if (mTimeSeriesDraw)
          //     mTimeSeriesDraw->hide_hz_section_labels_in_time_series();
          break;
      case SignaturePageDrawSettings::Layout::TreeCladesTSMaps:
          if (mChartFilename.empty())
              throw std::runtime_error("Cannot generate page in the TreeCladesTSMaps layout: no chart provided (--chart)");
          make_layout_tree_clades_ts_maps();
          if (mTimeSeriesDraw)
              mTimeSeriesDraw->tree_mode(false);
          break;
    }

    if (mMappedAntigensDraw)
        mMappedAntigensDraw->prepare();
    if (mTitleDraw)
        mTitleDraw->prepare();
    if (mTreeDraw)
        mTreeDraw->prepare(*mLocdb);
    if (mTimeSeriesDraw)
        mTimeSeriesDraw->prepare();
    if (mCladesDraw)
        mCladesDraw->prepare();
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

    mTreeDraw->surface().move_resize_viewport({mSettings->signature_page.left, mSettings->signature_page.top}, tree_width, Size{1000 * tree_width / section_height, 1000});
    mTimeSeriesDraw->surface().move_resize_viewport({ts_left, mSettings->signature_page.top}, ts_width, Size{1000 * ts_width / section_height, 1000});
    mCladesDraw->surface().move_resize_viewport({clades_left, mSettings->signature_page.top}, clades_width, Size{1000 * clades_width / section_height, 1000});
    mTitleDraw->surface().move_resize_viewport(Location{}, page_size.width, page_size);

    std::cout << "Tree   " << mTreeDraw->surface() << std::endl;
    std::cout << "TS     " << mTimeSeriesDraw->surface() << std::endl;
    std::cout << "Clades " << mCladesDraw->surface() << std::endl;

} // SignaturePageDraw::make_layout_tree_ts_clades

// ----------------------------------------------------------------------

void SignaturePageDraw::make_layout_tree_clades_ts_maps()
{
    const Size& page_size = mSurface->viewport().size;
    const double section_height = page_size.height - (mSettings->signature_page.top + mSettings->signature_page.bottom);

    const double antigic_maps_width = mSettings->signature_page.antigenic_maps_width;
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

    mTreeDraw->surface().move_resize_viewport({mSettings->signature_page.left, mSettings->signature_page.top}, tree_width, Size{1000 * tree_width / section_height, 1000});
    mTimeSeriesDraw->surface().move_resize_viewport({ts_left, mSettings->signature_page.top}, ts_width, Size{1000 * ts_width / section_height, 1000});
    mCladesDraw->surface().move_resize_viewport({clades_left, mSettings->signature_page.top}, clades_width, Size{1000 * clades_width / section_height, 1000});
    mMappedAntigensDraw->surface().move_resize_viewport({mapped_antigens_left, mSettings->signature_page.top}, mapped_antigens_width, Size{1000 * mapped_antigens_width / section_height, 1000});
    mAntigenicMapsDraw->surface().move_resize_viewport({antigic_maps_left, mSettings->signature_page.top}, antigic_maps_width, Size{1000 * antigic_maps_width / section_height, 1000});
    mTitleDraw->surface().move_resize_viewport(Location{}, page_size.width, page_size);

    std::cout << "Tree   " << mTreeDraw->surface() << std::endl;
    std::cout << "TS     " << mTimeSeriesDraw->surface() << std::endl;
    std::cout << "Clades " << mCladesDraw->surface() << std::endl;
    std::cout << "Mapped " << mMappedAntigensDraw->surface() << std::endl;
    std::cout << "Maps   " << mAntigenicMapsDraw->surface() << std::endl;

} // SignaturePageDraw::make_layout_tree_clades_ts_maps

// ----------------------------------------------------------------------

void SignaturePageDraw::draw()
{
    std::cout << std::endl << "INFO: DRAW signature page" << std::endl;
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
        mAntigenicMapsDraw->draw(mMappedAntigensDraw->surface());
    if (mTitleDraw)
        mTitleDraw->draw();

} // SignaturePageDraw::draw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
