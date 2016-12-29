#pragma once

// #include <string>
// #include <limits>
// #include <utility>
#include <memory>

#include "surface.hh"

// ----------------------------------------------------------------------

class Settings;
class Tree;
namespace seqdb { class Seqdb; }
class LocDb;
class TreeDraw;
class TimeSeriesDraw;
class CladesDraw;
class Chart;
class MappedAntigensDraw;
class AntigenicMapsDraw;

// ----------------------------------------------------------------------

class SignaturePageDrawSettings
{
 public:
    enum class Layout { Auto, TreeTSClades, TreeCladesTSMaps };

    inline SignaturePageDrawSettings()
        : layout(Layout::Auto), top(80), bottom(80), left(50), right(0),
          tree_margin_right(30), mapped_antigens_margin_right(30), time_series_width(400), clades_width(100)
        {}

    Layout layout;
    double top, bottom, left, right;
    double tree_margin_right, mapped_antigens_margin_right;
    double time_series_width;
    double clades_width;

    inline std::string layout_to_string() const
        {
            switch (layout) {
              case Layout::Auto:
                  return "auto";
              case Layout::TreeTSClades:
                  return "tree-ts-clades";
              case Layout::TreeCladesTSMaps:
                  return "tree-clades-ts-maps";
            }
            return "tree-ts-clades";
        }

    inline void set_layot(std::string s)
        {
            if (s == "auto")
                layout = Layout::Auto;
            else if (s == "tree-ts-clades")
                layout = Layout::TreeTSClades;
            else if (s == "tree-clades-ts-maps")
                layout = Layout::TreeCladesTSMaps;
            else
                throw std::runtime_error("Unrecognized layout: " + s);
        }

}; // class SignaturePageDrawSettings

// ----------------------------------------------------------------------

class SignaturePageDraw
{
 public:
    SignaturePageDraw();
    ~SignaturePageDraw();

    void load_settings(std::string aFilename);
    void make_surface(std::string aFilename);
    void init_layout();
    void init_settings();
    inline Settings& settings() { return *mSettings; }
    void tree(std::string aTreeFilename, std::string aSeqdbFilename);
    void chart(std::string aChartFilename);

    void prepare();
    void draw();

 private:
    std::unique_ptr<Surface> mSurface;
    std::unique_ptr<Settings> mSettings;
    std::unique_ptr<LocDb> mLocdb;
    std::unique_ptr<seqdb::Seqdb> mSeqdb;
    std::unique_ptr<Tree> mTree;
    std::unique_ptr<TreeDraw> mTreeDraw;
    std::unique_ptr<TimeSeriesDraw> mTimeSeriesDraw;
    std::unique_ptr<CladesDraw> mCladesDraw;
    std::unique_ptr<Chart> mChart;
    std::unique_ptr<MappedAntigensDraw> mMappedAntigensDraw;
    std::unique_ptr<AntigenicMapsDraw> mAntigenicMapsDraw;

    SignaturePageDrawSettings::Layout detect_layout() const;
    void make_layout_tree_ts_clades();
    void make_layout_tree_clades_ts_maps();

}; // class SignaturePageDraw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
