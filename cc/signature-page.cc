#include <iostream>
#include <cstdlib>
#include <cmath>

#include "acmacs-base/read-file.hh"
#include "seqdb/seqdb.hh"
#include "acmacs-draw/surface-cairo.hh"

#include "signature-page.hh"
#include "tree.hh"
#include "tree-export.hh"
#include "tree-draw.hh"
#include "time-series-draw.hh"
#include "settings.hh"
#include "mapped-antigens-draw.hh"
#include "antigenic-maps-draw.hh"
#include "title-draw.hh"
#include "settings-initializer.hh"

// ----------------------------------------------------------------------

SignaturePageDraw::SignaturePageDraw()
    : mSettings{new Settings{}}, mTree{new Tree{}}
{
    try {
        mSeqdb = &seqdb::get();
    }
    catch (seqdb::import_error&) {
    }

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

SignaturePageLayout SignaturePageDraw::detect_layout(bool init_settings, bool show_aa_at_pos) const
{
    const auto layout = *mSettings->signature_page->layout;
    if (layout == SignaturePageLayout::Auto || init_settings) {
        if (mChartFilename.empty()) {
            if (show_aa_at_pos)
                return SignaturePageLayout::TreeAATSClades;
            else
                return SignaturePageLayout::TreeTSClades;
        }
        else
            return SignaturePageLayout::TreeCladesTSMaps;
    }
    else
        return layout;

} // SignaturePageDraw::detect_layout

// ----------------------------------------------------------------------

void SignaturePageDraw::make_surface(std::string aFilename, bool init_settings, bool show_aa_at_pos, bool draw_map)
{
    double width = 300, height = 300;
    switch (detect_layout(init_settings, show_aa_at_pos)) {
      case SignaturePageLayout::TreeCladesTSMaps:
      case SignaturePageLayout::TreeAATSClades:
          width = 1360;         // ratio 1.6
          height = 850;
          break;
      case SignaturePageLayout::TreeTSClades:
      case SignaturePageLayout::Auto:
          height = 800;
          width = std::floor(height * (210.0 / 297.0));
          break;
    }
    mSurface = std::make_unique<acmacs::surface::PdfCairo>(draw_map ? aFilename : std::string{}, width, height);
    std::cout << "INFO: Surface: " << width << " x " << height << '\n';

    mTreeDraw = std::make_unique<TreeDraw>(*this, mSurface->subsurface(false), *mTree, *mSettings->tree_draw, *mSettings->hz_sections);
    mTimeSeriesDraw = std::make_unique<TimeSeriesDraw>(mSurface->subsurface(false), *mTree, *mTreeDraw, *mSettings->hz_sections, *mSettings->time_series);
    mCladesDraw = std::make_unique<CladesDraw>(mSurface->subsurface(false), *mTree, *mTreeDraw, *mTimeSeriesDraw, *mSettings->clades);
    mAAAtPosDraw = std::make_unique<AAAtPosDraw>(mSurface->subsurface(false), *mTree, *mSettings->hz_sections, *mSettings->aa_at_pos);

    if (!mChartFilename.empty()) {
        mAntigenicMapsDraw = std::unique_ptr<AntigenicMapsDrawBase>(make_antigenic_maps_draw(mChartFilename, mSurface->subsurface(false), *mTree, *mSettings->hz_sections, *mSettings->signature_page, *mSettings->antigenic_maps));
        mMappedAntigensDraw = std::make_unique<MappedAntigensDraw>(mSurface->subsurface(false), *mTree, mAntigenicMapsDraw->chart(), *mSettings->mapped_antigens);
        mTitleDraw = std::make_unique<TitleDraw>(mSurface->subsurface(false), *mSettings->title);
    }
    else {
        mTitleDraw = std::make_unique<TitleDraw>(mSurface->subsurface(false), *mSettings->title);
    }

} // SignaturePageDraw::make_surface

// ----------------------------------------------------------------------

void SignaturePageDraw::init_settings(bool show_aa_at_pos)
{
    mSettings->inject_default();

    const auto virus_type = mTree->virus_type();
    const auto lab = mAntigenicMapsDraw ? mAntigenicMapsDraw->chart().lab() : std::string{};
    const auto assay = mAntigenicMapsDraw ? mAntigenicMapsDraw->chart().assay() : std::string{};
    auto settings_initilizer = settings_initilizer_factory(lab, virus_type, assay, show_aa_at_pos);

    settings_initilizer->update(*mSettings->signature_page);

    if (!mChartFilename.empty()) {
        mSettings->signature_page->layout = SignaturePageLayout::TreeCladesTSMaps;
        if (mAntigenicMapsDraw)
            mAntigenicMapsDraw->init_settings(*settings_initilizer);
    }
    else if (settings_initilizer->show_aa_at_pos()) {
        mSettings->signature_page->layout = SignaturePageLayout::TreeAATSClades;
        mSettings->aa_at_pos->width = 500;
    }
    else {
        mSettings->signature_page->layout = SignaturePageLayout::TreeTSClades;
    }

    if (mTitleDraw)
        mTitleDraw->init_settings(*settings_initilizer);
    if (mTreeDraw)
        mTreeDraw->ladderize(); // ladderize and set_line_no before init clades!
    if (mCladesDraw)
        mCladesDraw->init_settings(*settings_initilizer);
    if (mTreeDraw) {
        auto clades = mCladesDraw ? mCladesDraw->clades() : nullptr;
        if (mSettings->signature_page->layout == SignaturePageLayout::TreeTSClades)
            mTreeDraw->detect_hz_lines_for_clades(clades, true);
        mTreeDraw->init_settings(clades, *settings_initilizer);
    }
    if (mTimeSeriesDraw)
        mTimeSeriesDraw->init_settings(*settings_initilizer);

    // std::cerr << "DEBUG: " << mSettings->hz_sections->pretty() << DEBUG_LINE_FUNC << '\n';

} // SignaturePageDraw::init_settings

// ----------------------------------------------------------------------

void SignaturePageDraw::write_initialized_settings(std::string aFilename)
{
    SignaturePageLayout layout = settings().signature_page->layout;
    if (layout == SignaturePageLayout::Auto)
        layout = mSurface->aspect() > 1 ? SignaturePageLayout::TreeCladesTSMaps : SignaturePageLayout::TreeTSClades;
    switch (layout) {
        case SignaturePageLayout::Auto:
            break;
        case SignaturePageLayout::TreeCladesTSMaps:
            settings().tree_draw->remove_for_signature_page_settings();
            settings().time_series.remove();
            settings().clades.remove();
            settings().hz_sections.remove();
            break;
        case SignaturePageLayout::TreeTSClades:
        case SignaturePageLayout::TreeAATSClades:
            settings().tree_draw->remove_for_tree_settings();
            settings().time_series->remove_for_tree_settings();
            settings().title->remove_for_tree_settings();
            settings().antigenic_maps.remove();
            settings().mapped_antigens.remove();
            break;
    }
    write_settings(settings(), aFilename);

} // SignaturePageDraw::write_initialized_settings

// ----------------------------------------------------------------------

void SignaturePageDraw::tree(std::string aTreeFilename, seqdb::Seqdb::ignore_not_found ignore)
{
    tree::tree_import(aTreeFilename, *mTree);
    if (mSeqdb)
        mTree->match_seqdb(*mSeqdb, ignore);

} // SignaturePageDraw::tree

// ----------------------------------------------------------------------

void SignaturePageDraw::prepare(bool show_hz_sections)
{
    std::cout << "\nINFO: PREPARE:\n";
    settings().hz_sections->show = show_hz_sections;
    switch (detect_layout(false, false)) {
      case SignaturePageLayout::TreeTSClades:
      case SignaturePageLayout::TreeAATSClades:
      case SignaturePageLayout::Auto:
          make_layout_tree_ts_clades();
          if (mTimeSeriesDraw)
              mTimeSeriesDraw->tree_mode(true);
          //   // hide hz section lables in time series
          // if (mTimeSeriesDraw)
          //     mTimeSeriesDraw->hide_hz_section_labels_in_time_series();
          break;
      case SignaturePageLayout::TreeCladesTSMaps:
          if (mChartFilename.empty())
              throw std::runtime_error("Cannot generate page in the TreeCladesTSMaps layout: no chart provided (--chart)");
          make_layout_tree_clades_ts_maps();
          if (mTimeSeriesDraw)
              mTimeSeriesDraw->tree_mode(false);
          break;
    }

    if (mMappedAntigensDraw)
        mMappedAntigensDraw->prepare();
    if (mAAAtPosDraw)
        mAAAtPosDraw->prepare();
    if (mTitleDraw)
        mTitleDraw->prepare();
    if (mTreeDraw)
        mTreeDraw->prepare();
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
    const acmacs::Size& page_size = mSurface->viewport().size;
    const double section_height = page_size.height - (mSettings->signature_page->top + mSettings->signature_page->bottom);

    const double aa_at_pos_width = mSettings->aa_at_pos->width + mSettings->aa_at_pos->right_margin;
    const double clades_width = mSettings->signature_page->clades_width;
    const double ts_width = mSettings->signature_page->time_series_width;
    const double tree_width =
        page_size.width - (mSettings->signature_page->left + mSettings->signature_page->tree_margin_right + ts_width + clades_width + aa_at_pos_width + mSettings->signature_page->right);

    const double aa_at_pos_left = mSettings->signature_page->left + tree_width + mSettings->signature_page->tree_margin_right;
    const double ts_left = aa_at_pos_left + aa_at_pos_width;
    const double clades_left = ts_left + ts_width;

    mTreeDraw->surface().move_resize_viewport({mSettings->signature_page->left, mSettings->signature_page->top}, tree_width, acmacs::Size{1000 * tree_width / section_height, 1000});
    if (mSettings->aa_at_pos->width > 0.0)
        mAAAtPosDraw->surface().move_resize_viewport({aa_at_pos_left, mSettings->signature_page->top}, mSettings->aa_at_pos->width,
                                                     acmacs::Size{1000 * mSettings->aa_at_pos->width / section_height, 1000});
    mTimeSeriesDraw->surface().move_resize_viewport({ts_left, mSettings->signature_page->top}, ts_width, acmacs::Size{1000 * ts_width / section_height, 1000});
    mCladesDraw->surface().move_resize_viewport({clades_left, mSettings->signature_page->top}, clades_width, acmacs::Size{1000 * clades_width / section_height, 1000});
    mTitleDraw->surface().move_resize_viewport(acmacs::Location2D{}, page_size.width, page_size);

    std::cout << "INFO: Tree    " << mTreeDraw->surface() << '\n'
              << "INFO: AAatPos " << mAAAtPosDraw->surface() << '\n'
              << "INFO: TS      " << mTimeSeriesDraw->surface() << '\n'
              << "INFO: Clades  " << mCladesDraw->surface() << '\n';

} // SignaturePageDraw::make_layout_tree_ts_clades

// ----------------------------------------------------------------------

void SignaturePageDraw::make_layout_tree_clades_ts_maps()
{
    const acmacs::Size& page_size = mSurface->viewport().size;
    const double section_height = page_size.height - (mSettings->signature_page->top + mSettings->signature_page->bottom);

    const double antigic_maps_width = mSettings->signature_page->antigenic_maps_width;
    const double mapped_antigens_width = mSettings->mapped_antigens->width;
    const double clades_width = mSettings->signature_page->clades_width;
    const double ts_width = mSettings->signature_page->time_series_width;
    const double tree_width = page_size.width - (mSettings->signature_page->left + mSettings->signature_page->tree_margin_right + ts_width + clades_width + mapped_antigens_width +
                                                 mSettings->signature_page->mapped_antigens_margin_right + antigic_maps_width + mSettings->signature_page->right);

    const double clades_left = mSettings->signature_page->left + tree_width + mSettings->signature_page->tree_margin_right;
    const double ts_left = clades_left + clades_width;
    const double mapped_antigens_left = ts_left + ts_width;
    const double antigic_maps_left = mapped_antigens_left + mapped_antigens_width + mSettings->signature_page->mapped_antigens_margin_right;

    mTreeDraw->surface().move_resize_viewport({mSettings->signature_page->left, mSettings->signature_page->top}, tree_width, acmacs::Size{1000 * tree_width / section_height, 1000});
    mTimeSeriesDraw->surface().move_resize_viewport({ts_left, mSettings->signature_page->top}, ts_width, acmacs::Size{1000 * ts_width / section_height, 1000});
    mCladesDraw->surface().move_resize_viewport({clades_left, mSettings->signature_page->top}, clades_width, acmacs::Size{1000 * clades_width / section_height, 1000});
    mMappedAntigensDraw->surface().move_resize_viewport({mapped_antigens_left, mSettings->signature_page->top}, mapped_antigens_width,
                                                        acmacs::Size{1000 * mapped_antigens_width / section_height, 1000});
    mAntigenicMapsDraw->surface().move_resize_viewport({antigic_maps_left, mSettings->signature_page->top}, antigic_maps_width, acmacs::Size{1000 * antigic_maps_width / section_height, 1000});
    mTitleDraw->surface().move_resize_viewport(acmacs::Location2D{}, page_size.width, page_size);

    std::cout << "INFO: Tree   " << mTreeDraw->surface() << '\n'
              << "INFO: TS     " << mTimeSeriesDraw->surface() << '\n'
              << "INFO: Clades " << mCladesDraw->surface() << '\n'
              << "INFO: Mapped " << mMappedAntigensDraw->surface() << '\n'
              << "INFO: Maps   " << mAntigenicMapsDraw->surface() << '\n';

} // SignaturePageDraw::make_layout_tree_clades_ts_maps

// ----------------------------------------------------------------------

void SignaturePageDraw::draw(bool report_antigens_in_hz_sections, bool init_settings, size_t hz_section_threshold, size_t aa_small_section_threshold)
{
    std::cout << "\nINFO: DRAW signature page\n";
    const auto& v = mSurface->viewport();
    mSurface->rectangle_filled(v.origin, v.size, WHITE, Pixels{0}, WHITE);

    if (init_settings)
        mSettings->aa_at_pos->small_section_threshold = aa_small_section_threshold;

    if (mTreeDraw)
        mTreeDraw->draw();
    if (mAAAtPosDraw)
        mAAAtPosDraw->draw(init_settings, hz_section_threshold);
    if (mTimeSeriesDraw)
        mTimeSeriesDraw->draw();
    if (mCladesDraw)
        mCladesDraw->draw();
    if (mMappedAntigensDraw)
        mMappedAntigensDraw->draw();
    if (mAntigenicMapsDraw)
        mAntigenicMapsDraw->draw(mMappedAntigensDraw->surface(), report_antigens_in_hz_sections);
    if (mTitleDraw)
        mTitleDraw->draw();

    draw_mods();

} // SignaturePageDraw::draw

// ----------------------------------------------------------------------

void SignaturePageDraw::draw_mods()
{
    try {
        mSettings->mods.for_each([this](const SettingsMod& mod) {
            if (mod.name.is_set_or_has_default()) {
                if (mod.name == "text") {
                    mSurface->text(mod.offset.get_or(acmacs::Offset{}), mod.text.get_or(""), mod.color.get_or(BLACK),
                                   Pixels{mod.size.get_or(14.0)}, mod.style.get_or(acmacs::TextStyle{}));
                }
                else {
                    std::cerr << "WARNING: unrecognized mod: " << mod << '\n';
                }
            }
        });
    }
    catch (std::exception& err) {
        std::cerr << "WARNING: cannot draw mods: " << err.what() << '\n';
    }

} // SignaturePageDraw::draw_mods

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
