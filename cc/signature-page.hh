#pragma once

#include <memory>

#include "seqdb/seqdb.hh"
#include "antigenic-maps-draw.hh"
#include "chart-draw.hh"

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
class SettingsInitializer;

// ----------------------------------------------------------------------

enum class SignaturePageLayout { Auto, TreeTSClades, TreeTSCladesWide, TreeCladesTSMaps, TreeAATSClades };

namespace acmacs::settings
{
    inline namespace v1
    {
        template <> inline void field<SignaturePageLayout>::assign(rjson::value& to, const SignaturePageLayout& from)
        {
            switch (from) {
              case SignaturePageLayout::Auto:
                  to = "auto";
                  break;
              case SignaturePageLayout::TreeTSClades:
                  to = "tree-ts-clades";
                  break;
              case SignaturePageLayout::TreeTSCladesWide:
                  to = "tree-ts-clades-wide";
                  break;
              case SignaturePageLayout::TreeCladesTSMaps:
                  to = "tree-clades-ts-maps";
                  break;
              case SignaturePageLayout::TreeAATSClades:
                  to = "tree-aa-ts-clades";
                  break;
            }
        }

        template <> inline SignaturePageLayout field<SignaturePageLayout>::extract(const rjson::value& from) const
        {
            if (from == "auto")
                return SignaturePageLayout::Auto;
            else if (from == "tree-ts-clades")
                return SignaturePageLayout::TreeTSClades;
            else if (from == "tree-ts-clades-wide")
                return SignaturePageLayout::TreeTSCladesWide;
            else if (from == "tree-aa-ts-clades")
                return SignaturePageLayout::TreeAATSClades;
            else if (from == "tree-clades-ts-maps")
                return SignaturePageLayout::TreeCladesTSMaps;
            else
                throw std::runtime_error("Unrecognized layout: " + rjson::to_string(from));
        }
    }
}

// ----------------------------------------------------------------------

class SignaturePageDrawSettings : public acmacs::settings::object
{
 public:
    using acmacs::settings::object::object;

    acmacs::settings::field<SignaturePageLayout> layout{this, "layout", SignaturePageLayout::Auto};
    acmacs::settings::field<double>
        top{this, "top", 60},
        bottom{this, "bottom", 60},
        left{this, "left", 50},
        right{this, "right", 20},
        tree_margin_right{this, "tree_margin_right", 0},
        mapped_antigens_margin_right{this, "mapped_antigens_margin_right", 10},
        time_series_width{this, "time_series_width", 400},
        clades_width{this, "clades_width", 100},
        antigenic_maps_width{this, "antigenic_maps_width"}; // , 300};

}; // class SignaturePageDrawSettings

// ----------------------------------------------------------------------

class SignaturePageDraw
{
 public:
    SignaturePageDraw();
    ~SignaturePageDraw();

    void load_settings(std::string_view aFilename);
    void make_surface(std::string_view aFilename, bool init_settings, bool show_aa_at_pos, bool draw_map);
    void init_settings(bool show_aa_at_pos, bool whocc_support);
    void write_initialized_settings(std::string_view aFilename); // removes redundant settings entries depending on layout!
    Settings& settings() { return *mSettings; }
    void tree(std::string_view aTreeFilename, seqdb::Seqdb::ignore_not_found ignore = seqdb::Seqdb::ignore_not_found::no);
    Tree& tree() { return *mTree; }
    const TreeDraw& tree_draw() const { return *mTreeDraw; }
    void chart(std::string_view aChartFilename) { mChartFilename = aChartFilename; }
    bool has_antigenic_maps_draw() const noexcept { return bool{mMappedAntigensDraw}; }
    const AntigenicMapsDrawBase& antigenic_maps_draw() const { return *mAntigenicMapsDraw; }

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

    SignaturePageLayout detect_layout(bool init_settings, bool show_aa_at_pos) const;
    void make_layout_tree_ts_clades();
    void make_layout_tree_clades_ts_maps();
    void draw_mods();

}; // class SignaturePageDraw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
