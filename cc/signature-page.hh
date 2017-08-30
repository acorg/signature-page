#pragma once

#include <memory>

#include "acmacs-base/rjson.hh"
#include "antigenic-maps-draw.hh"
#include "chart-draw.hh"

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

class SignaturePageDrawSettings
{
 public:
    enum class Layout { Auto, TreeTSClades, TreeCladesTSMaps };

    SignaturePageDrawSettings() : mData{nullptr} {}

      //Layout layout;
    double top, bottom, left, right;
    double tree_margin_right, mapped_antigens_margin_right;
    double time_series_width;
    double clades_width;
    double antigenic_maps_width;

    inline void use_json(rjson::value& aData) { mData = &aData; std::cerr << "SignaturePageDrawSettings::use_json " << aData << '\n'; }

    inline Layout layout() const
        {
            const auto layout_s = layout_to_string();
            if (layout_s == "auto")
                return Layout::Auto;
            else if (layout_s == "tree-ts-clades")
                return Layout::TreeTSClades;
            else if (layout_s == "tree-clades-ts-maps")
                return Layout::TreeCladesTSMaps;
            else
                throw std::runtime_error("Unrecognized layout: " + layout_s);
        }

    inline rjson::value default_layout() const { using namespace std::literals; return rjson::string{"auto"s}; }
    inline std::string layout_to_string() const { std::cerr << "SignaturePageDrawSettings::layout_to_string " << *mData << '\n'; return std::get<rjson::string>(mData->get_ref("layout", default_layout())); }
    inline void set_layot(std::string s) { std::get<rjson::string>(mData->get_ref("layout", default_layout())) = s; }

//     inline std::string layout_to_string() const
//         {
//             switch (layout) {
//               case Layout::Auto:
//                   return "auto";
//               case Layout::TreeTSClades:
//                   return "tree-ts-clades";
//               case Layout::TreeCladesTSMaps:
//                   return "tree-clades-ts-maps";
//             }
//             return "tree-ts-clades";
//         }
//
//     inline void set_layot(std::string s)
//         {
//             if (s == "auto")
//                 layout = Layout::Auto;
//             else if (s == "tree-ts-clades")
//                 layout = Layout::TreeTSClades;
//             else if (s == "tree-clades-ts-maps")
//                 layout = Layout::TreeCladesTSMaps;
//             else
//                 throw std::runtime_error("Unrecognized layout: " + s);
//         }

 private:
    rjson::value* mData;

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
