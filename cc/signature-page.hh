#pragma once

#include <memory>

#include "antigenic-maps-draw.hh"
#include "chart-draw.hh"
#include "rjson-serialize.hh"

// ----------------------------------------------------------------------

class Settings;
class Tree;
namespace seqdb { class Seqdb; }
class LocDb;
class TreeDraw;
class TimeSeriesDraw;
class CladesDraw;
class MappedAntigensDraw;
class TitleDraw;

// ----------------------------------------------------------------------

class SignaturePageDrawSettings : public rjson::field_container_child
{
 public:
    enum class Layout { Auto, TreeTSClades, TreeCladesTSMaps };

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

    inline Layout get_layout() const
        {
            const std::string layout_s = layout;
            if (layout_s == "auto")
                return Layout::Auto;
            else if (layout_s == "tree-ts-clades")
                return Layout::TreeTSClades;
            else if (layout_s == "tree-clades-ts-maps")
                return Layout::TreeCladesTSMaps;
            else
                throw std::runtime_error("Unrecognized layout: " + layout_s);
        }

    // inline rjson::value default_layout() const { return rjson::string{"auto"}; }
    // inline std::string layout_to_string() const { return std::get<rjson::string>(mData->get_ref("layout", default_layout())); }
    // inline void set_layot(std::string s) { std::get<rjson::string>(mData->get_ref("layout", default_layout())) = s; }

    // inline double top() const { return mData->get_field("top", 60.0); }
    // inline void top(double aValue) { mData->set_field("top", aValue); }
    // inline double bottom() const { return mData->get_field("bottom", 60.0); }
    // inline void bottom(double aValue) { mData->set_field("bottom", aValue); }
    // inline double left() const { return mData->get_field("left", 50.0); }
    // inline void left(double aValue) { mData->set_field("left", aValue); }
    // inline double right() const { return mData->get_field("right", 20.0); }
    // inline void right(double aValue) { mData->set_field("right", aValue); }
    // inline double tree_margin_right() const { return mData->get_field("tree_margin_right", 0.0); }
    // inline void tree_margin_right(double aValue) { mData->set_field("tree_margin_right", aValue); }
    // inline double mapped_antigens_margin_right() const { return mData->get_field("mapped_antigens_margin_right", 30.0); }
    // inline void mapped_antigens_margin_right(double aValue) { mData->set_field("mapped_antigens_margin_right", aValue); }
    // inline double time_series_width() const { return mData->get_field("time_series_width", 400.0); }
    // inline void time_series_width(double aValue) { mData->set_field("time_series_width", aValue); }
    // inline double clades_width() const { return mData->get_field("clades_width", 100.0); }
    // inline void clades_width(double aValue) { mData->set_field("clades_width", aValue); }
    // inline double antigenic_maps_width() const { return mData->get_field("antigenic_maps_width", 500.0); }
    // inline void antigenic_maps_width(double aValue) { mData->set_field("antigenic_maps_width", aValue); }

 // private:
 //    mutable rjson::value* mData = nullptr;

}; // class SignaturePageDrawSettings

// ----------------------------------------------------------------------

class SignaturePageDraw
{
 public:
    SignaturePageDraw();
    ~SignaturePageDraw();

    void load_settings(std::string aFilename);
    void load_settings(const std::vector<std::string>& aFilenames);
    void make_surface(std::string aFilename, bool init_settings, bool draw_map);
    void init_layout();
    void init_settings();
    inline Settings& settings() { return *mSettings; }
    void tree(std::string aTreeFilename, std::string aSeqdbFilename);
    inline Tree& tree() { return *mTree; }
    inline void chart(std::string aChartFilename) { mChartFilename = aChartFilename; }

    void prepare();
    void draw();

 private:
    std::string mChartFilename;
    std::unique_ptr<Surface> mSurface;
    std::unique_ptr<Settings> mSettings;
    std::unique_ptr<LocDb> mLocdb;
    std::unique_ptr<seqdb::Seqdb> mSeqdb;
    std::unique_ptr<Tree> mTree;
    std::unique_ptr<TreeDraw> mTreeDraw;
    std::unique_ptr<TimeSeriesDraw> mTimeSeriesDraw;
    std::unique_ptr<CladesDraw> mCladesDraw;
    std::unique_ptr<MappedAntigensDraw> mMappedAntigensDraw;
    std::unique_ptr<AntigenicMapsDrawBase> mAntigenicMapsDraw;
    std::unique_ptr<TitleDraw> mTitleDraw;

    SignaturePageDrawSettings::Layout detect_layout(bool init_settings) const;
    void make_layout_tree_ts_clades();
    void make_layout_tree_clades_ts_maps();

}; // class SignaturePageDraw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
