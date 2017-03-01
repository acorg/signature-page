#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>

#include "acmacs-base/transformation.hh"
#include "acmacs-base/color.hh"
#include "acmacs-draw/viewport.hh"

// ----------------------------------------------------------------------

class AntigenicMapsDrawSettings;

// ----------------------------------------------------------------------

namespace sdb
{

// class VaccineData
// {
//  public:
//     inline VaccineData() : enabled(false), fill_color(0xFFC0CB), outline_color(0), aspect(0.5) {}

//     bool enabled;
//     Color fill_color;
//     Color outline_color;
//     double aspect;
// };

// ----------------------------------------------------------------------

    class PointAttributes
    {
     public:
        inline PointAttributes() : antigen(true), egg(false), reassortant(false), reference(false), vaccine(false), homologous_antigen(-1), serum_circle_radius(-1) {}

        bool antigen;
        bool egg;
        bool reassortant;
        bool reference;
          // v1 VaccineData vaccine;
        bool vaccine;
        int homologous_antigen;
        std::string homologous_titer;
        double serum_circle_radius;
    };

// ----------------------------------------------------------------------

    class Point
    {
     public:
        inline Point() : section_for_serum_circle(-1) {}

        std::string name;
        Location coordinates;
        std::string lab_id;
        PointAttributes attributes;
        int section_for_serum_circle; // -1 means null

    }; // class Point

// ----------------------------------------------------------------------

    class PointStyle
    {
     public:
        inline PointStyle()
            : fill_color("transparent"), outline_color("black"), aspect(1), size(1), rotation(0) {}

        Color fill_color;
        Color outline_color;
        double aspect;
        std::string shape;
        double size;
        double rotation;

    }; // class PointStyle

// ----------------------------------------------------------------------

    class PlotStyle
    {
     public:
        inline PlotStyle() {}

        std::vector<size_t> points;
        std::vector<PointStyle> styles;

        inline const PointStyle& style(size_t aPointNo) const { return styles[points[aPointNo]]; }

    }; // class PlotStyle

// ----------------------------------------------------------------------

    class ChartInfo
    {
     public:
        inline ChartInfo() {}

        std::string date;
        std::string lab;
        std::string virus_type;
        std::string lineage;
        std::string name;
        std::string rbc_species;

    }; // class ChartInfo

// ----------------------------------------------------------------------

    class Chart
    {
     public:
        inline Chart() : mStress(-1) {}
        ~Chart();

        void init_settings();
        void prepare(const AntigenicMapsDrawSettings& aSettings);

          // void preprocess(const SettingsAntigenicMaps& aSettings);
          // void draw_points_reset(const SettingsAntigenicMaps& aSettings) const;

        constexpr static const size_t AntigenNotFound = static_cast<size_t>(-1);
          // returns AntigenNotFound if not found
        inline size_t find_antigen(std::string aName) const
            {
                make_point_by_name();
                const auto p = mPointByName.find(aName);
                  // std::cerr << "Chart::find_antigen " << aName << " " << (p != mPointByName.end()) << std::endl;
                return p == mPointByName.end() ? AntigenNotFound : p->second;
            }

          //   // returns number of antigens from aNames list found in the chart
          // size_t tracked_antigens(const std::vector<std::string>& aNames, Color aFillColor, const SettingsAntigenicMaps& aSettings) const;
          // size_t tracked_antigens_colored_by_clade(const std::vector<std::string>& aNames, const std::map<std::string, const Node*>& aNodeByName, const SettingsAntigenicMaps& aSettings) const;
          //   // returns line_no for each antigen from aLeaves found in the chart
          // std::vector<size_t> sequenced_antigens(const std::vector<const Node*>& aLeaves);
          // size_t marked_antigens(const SettingsMarkAntigens& aData, const std::vector<std::string>& aTrackedNames, size_t aSectionNo, const SettingsAntigenicMaps& aSettings) const;
          // void tracked_sera(size_t aSectionNo, const SettingsAntigenicMaps& aSettings) const;

          // // const Viewport& viewport() const { return mViewport; }
          // void draw(Surface& aSurface, double aObjectScale, const SettingsAntigenicMaps& aSettings) const;

        inline const std::vector<Point>& points() const { return mPoints; }
        inline std::vector<Point>& points() { return mPoints; }
        inline const std::vector<size_t>& drawing_order() const { return mDrawingOrder; }
        inline std::vector<size_t>& drawing_order() { return mDrawingOrder; }
        inline const ChartInfo& chart_info() const { return mInfo; }
        inline ChartInfo& chart_info() { return mInfo; }
        inline const std::string minimum_column_basis() const { return mMinimumColumnBasis; }
        inline std::string& minimum_column_basis() { return mMinimumColumnBasis; }
        inline const PlotStyle& plot_style() const { return mPlot; }
        inline PlotStyle& plot_style() { return mPlot; }
        inline double stress() const { return mStress; }
        inline void stress(double aStress) { mStress = aStress; }
        inline const Transformation& transformation() const { return mTransformation; }
        inline Transformation& transformation() { return mTransformation; }
        inline const std::vector<double>& column_bases() const { return mColumnBases; }
        inline std::vector<double>& column_bases() { return mColumnBases; }

        inline const Viewport& viewport(const Transformation* aSettingsTransformation) { calculate_viewport(aSettingsTransformation); return mViewport; }
        inline const Viewport& viewport(const Transformation* aSettingsTransformation) const { return const_cast<Chart*>(this)->viewport(aSettingsTransformation); }

     private:
        double mStress;
        ChartInfo mInfo;
        std::vector<Point> mPoints;
        std::string mMinimumColumnBasis;
        std::vector<double> mColumnBases;
        Transformation mTransformation;
        std::vector<size_t> mDrawingOrder;
        PlotStyle mPlot;

        mutable std::map<std::string, size_t> mPointByName;
        Viewport mViewport;

        void make_point_by_name() const;
        void apply_transformation(const Transformation* aSettingsTransformation);
        void calculate_viewport(const Transformation* aSettingsTransformation);
        void bounding_rectangle(Viewport& aViewport) const;

          //std::set<size_t> mSequencedAntigens;
          // DrawSerum mDrawSerum;
          //   // DrawTrackedSerum mDrawTrackedSerum;
          // DrawReferenceAntigen mDrawReferenceAntigen;
          // DrawTestAntigen mDrawTestAntigen;
          // DrawSequencedAntigen mDrawSequencedAntigen;
          // mutable DrawTrackedAntigen mDrawTrackedAntigen;
          // mutable std::vector<DrawTrackedAntigen> mDrawTrackedAntigensColoredByClade;
          // DrawVaccineAntigen mDrawVaccineAntigen;
          // mutable std::vector<DrawMarkedAntigen> mDrawMarkedAntigens;
          // mutable std::vector<DrawTrackedSerum> mDrawTrackedSera;


          // std::set<std::string> mPrefixName;


          // void init_tracked_sera(size_t aSize, const SettingsAntigenicMaps& aSettings) const;
          // void add_tracked_serum(size_t aSectionNo, size_t aAntigenNo, const SettingsAntigenicMaps& aSettings) const;

    }; // class Chart

// ----------------------------------------------------------------------

    void read_chart_from_sdb(Chart& aChart, std::string aFilename);

} // namespace sdb

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
