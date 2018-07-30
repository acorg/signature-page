#pragma once

#include <memory>

#include "antigenic-maps-draw.hh"
#include "chart-draw.hh"
#include "rjson-serialize.hh"

// ----------------------------------------------------------------------

class Settings;
class Tree;
namespace seqdb { class Seqdb; }
class TreeDraw;
class TimeSeriesDraw;
class CladesDraw;
class MappedAntigensDraw;
class AAAtPosDraw;
class TitleDraw;

// ----------------------------------------------------------------------

class SignaturePageDrawSettings : public rjson::field_container_child
{
 public:
    enum class Layout { Auto, TreeTSClades, TreeCladesTSMaps, TreeAATSClades };

    SignaturePageDrawSettings(rjson::field_container_parent& aParent, std::string aFieldName);

    rjson::field_get_set<std::string> layout;
    rjson::field_get_set<double>
        top,
        bottom,
        left,
        right,
        tree_margin_right,
        mapped_antigens_margin_right,
        time_series_width,
        clades_width,
        antigenic_maps_width;
    rjson::field_get_set<bool> _force_pp; // hidden field to force pretty printing this rjson object

    Layout get_layout() const
        {
            const auto layout_s = layout.get_value_ref().strv();
            if (layout_s == "auto")
                return Layout::Auto;
            else if (layout_s == "tree-ts-clades")
                return Layout::TreeTSClades;
            else if (layout_s == "tree-aa-ts-clades")
                return Layout::TreeAATSClades;
            else if (layout_s == "tree-clades-ts-maps")
                return Layout::TreeCladesTSMaps;
            else
                throw std::runtime_error("Unrecognized layout: " + std::string(layout_s));
        }

}; // class SignaturePageDrawSettings

// ----------------------------------------------------------------------

class SignaturePageDraw
{
 public:
    SignaturePageDraw();
    ~SignaturePageDraw();

    void load_settings(std::string aFilename);
    void make_surface(std::string aFilename, bool init_settings, bool show_aa_at_pos, bool draw_map);
    void init_layout(bool show_aa_at_pos);
    void init_settings();
    void write_initialized_settings(std::string aFilename); // removes redundant settings entries depending on layout!
    Settings& settings() { return *mSettings; }
    void tree(std::string aTreeFilename);
    Tree& tree() { return *mTree; }
    const TreeDraw& tree_draw() const { return *mTreeDraw; }
    void chart(std::string aChartFilename) { mChartFilename = aChartFilename; }

    void prepare(bool show_hz_sections);
    void draw(bool report_antigens_in_hz_sections, bool init_settings, size_t hz_section_threshold, size_t aa_small_section_threshold);

 private:
    std::string mChartFilename;
    std::unique_ptr<acmacs::surface::Surface> mSurface;
    std::unique_ptr<Settings> mSettings;
    const seqdb::Seqdb* mSeqdb = nullptr;
    std::unique_ptr<Tree> mTree;
    std::unique_ptr<TreeDraw> mTreeDraw;
    std::unique_ptr<TimeSeriesDraw> mTimeSeriesDraw;
    std::unique_ptr<CladesDraw> mCladesDraw;
    std::unique_ptr<MappedAntigensDraw> mMappedAntigensDraw;
    std::unique_ptr<AAAtPosDraw> mAAAtPosDraw;
    std::unique_ptr<AntigenicMapsDrawBase> mAntigenicMapsDraw;
    std::unique_ptr<TitleDraw> mTitleDraw;

    SignaturePageDrawSettings::Layout detect_layout(bool init_settings, bool show_aa_at_pos) const;
    void make_layout_tree_ts_clades();
    void make_layout_tree_clades_ts_maps();
    void draw_mods();

}; // class SignaturePageDraw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
