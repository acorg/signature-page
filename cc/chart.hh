#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>

#include "acmacs-base/color.hh"
#include "size.hh"

// ----------------------------------------------------------------------

// class VaccineData
// {
//  public:
//     inline VaccineData() : enabled(false), fill_color(0xFFC0CB), outline_color(0), aspect(0.5) {}

//     bool enabled;
//     Color fill_color;
//     Color outline_color;
//     double aspect;
// };

class Transformation : public std::vector<double>
{
  public:
    inline Transformation() : std::vector<double>{1, 0, 0, 1} {}
    inline Transformation(const Transformation&) = default;
    inline Transformation(double a11, double a12, double a21, double a22) : std::vector<double>{a11, a12, a21, a22} {}
    inline Transformation& operator=(const Transformation& t) = default;

    inline void multiplyBy(const Transformation& t)
    {
        (*this)[0] = (*this)[0] * t[0] + (*this)[1] * t[2];
        (*this)[1] = (*this)[0] * t[1] + (*this)[1] * t[3];
        (*this)[2] = (*this)[2] * t[0] + (*this)[3] * t[2];
        (*this)[3] = (*this)[2] * t[1] + (*this)[3] * t[3];
    }

    friend inline std::ostream& operator << (std::ostream& out, const Transformation& t)
    {
        return out << "[[" << t[0] << ", " << t[1] << "], [" << t[2] << ", " << t[3] << "]]";
    }
};

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

    // void preprocess(const SettingsAntigenicMaps& aSettings);
    // void draw_points_reset(const SettingsAntigenicMaps& aSettings) const;

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

 private:
    double mStress;
    ChartInfo mInfo;
    std::vector<Point> mPoints;
    std::map<std::string, size_t> mPointByName;
    std::string mMinimumColumnBasis;
    std::vector<double> mColumnBases;
    Transformation mTransformation;
    std::vector<size_t> mDrawingOrder;
    PlotStyle mPlot;

    // std::set<size_t> mSequencedAntigens;
    // mutable std::vector<const DrawPoint*> mDrawPoints;
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

    // Viewport mViewport;

    // std::set<std::string> mPrefixName;


    // void apply_transformation(const SettingsAntigenicMaps& aSettings);
    // Viewport bounding_rectangle() const;
    // void init_tracked_sera(size_t aSize, const SettingsAntigenicMaps& aSettings) const;
    // void add_tracked_serum(size_t aSectionNo, size_t aAntigenNo, const SettingsAntigenicMaps& aSettings) const;

}; // class Chart

// ----------------------------------------------------------------------

void read_chart_from_sdb(Chart& aChart, std::string aFilename);

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
