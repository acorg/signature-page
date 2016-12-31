#pragma once

#include <iostream>

#include "surface.hh"
#include "chart.hh"

// ----------------------------------------------------------------------

class Tree;
class TreeDraw;
class HzSections;

// ----------------------------------------------------------------------

class AntigenicMapsDrawSettings
{
 public:
    inline AntigenicMapsDrawSettings()
        : width(500), columns(3), border_width(0.05), border_color("black"), gap(20),
          serum_scale(5), reference_antigen_scale(5), test_antigen_scale(3), vaccine_antigen_scale(8), tracked_antigen_scale(5),
          serum_outline_width(0.5), reference_antigen_outline_width(0.5), test_antigen_outline_width(0.5), vaccine_antigen_outline_width(0.5),
          sequenced_antigen_outline_width(0.5), tracked_antigen_outline_width(0.5),
          serum_outline_color("grey88"), reference_antigen_outline_color("grey88"), test_antigen_outline_color("grey88"),
          test_antigen_fill_color("grey88"), vaccine_antigen_outline_color("white"), sequenced_antigen_outline_color("white"), sequenced_antigen_fill_color("grey63"),
          tracked_antigen_outline_color("white"), tracked_antigen_colored_by_clade(false),
          reassortant_rotation(0.5 /* M_PI / 6.0 */), egg_antigen_aspect(0.75), serum_circle_color("black"), serum_circle_thickness(1) {}
    ~AntigenicMapsDrawSettings();

    double width;
    size_t columns;
    double border_width;
    Color border_color;
    double gap;
    Transformation transformation;
    Viewport viewport;

    double serum_scale, reference_antigen_scale, test_antigen_scale, vaccine_antigen_scale, tracked_antigen_scale;
    double serum_outline_width, reference_antigen_outline_width, test_antigen_outline_width, vaccine_antigen_outline_width, sequenced_antigen_outline_width, tracked_antigen_outline_width;
    Color serum_outline_color, reference_antigen_outline_color, test_antigen_outline_color, test_antigen_fill_color, vaccine_antigen_outline_color, sequenced_antigen_outline_color, sequenced_antigen_fill_color, tracked_antigen_outline_color;
    bool tracked_antigen_colored_by_clade;
    double reassortant_rotation, egg_antigen_aspect;
    Color serum_circle_color;
    double serum_circle_thickness;

}; // class AntigenicMapsDrawSettings

// ----------------------------------------------------------------------

class DrawPoint
{
 public:
    inline DrawPoint() = default;
    inline DrawPoint(const DrawPoint&) = default;
    inline DrawPoint(DrawPoint&&) = default;
    inline virtual ~DrawPoint() = default;
    inline DrawPoint& operator=(const DrawPoint&) = default;
    virtual void draw(Surface& aSurface, const Point& aPoint, const PointStyle& aStyle, double aObjectScale, const AntigenicMapsDrawSettings& aSettings) const = 0;
    virtual size_t level() const = 0;
    virtual inline double aspect(const Point&, const PointStyle& aStyle, const AntigenicMapsDrawSettings&) const { return aStyle.aspect; }
    virtual inline double rotation(const Point& aPoint, const PointStyle& aStyle, const AntigenicMapsDrawSettings& aSettings) const { return aPoint.attributes.reassortant ? aSettings.reassortant_rotation : aStyle.rotation; }
    virtual inline Color fill_color(const Point&, const PointStyle& aStyle, const AntigenicMapsDrawSettings&) const { return aStyle.fill_color; }
    virtual inline Color outline_color(const Point&, const PointStyle& aStyle, const AntigenicMapsDrawSettings&) const { return aStyle.outline_color; }
};

class DrawSerum : public DrawPoint
{
 public:
    virtual void draw(Surface& aSurface, const Point& aPoint, const PointStyle& aStyle, double aObjectScale, const AntigenicMapsDrawSettings& aSettings) const;
    virtual inline size_t level() const { return 1; }
    virtual inline Color outline_color(const Point&, const PointStyle&, const AntigenicMapsDrawSettings& aSettings) const { return aSettings.serum_outline_color; }
    virtual inline double outline_width(const Point&, const PointStyle&, const AntigenicMapsDrawSettings& aSettings) const { return aSettings.serum_outline_width; }
};

class DrawAntigen : public DrawPoint
{
 public:
    virtual inline double aspect(const Point& aPoint, const PointStyle& aStyle, const AntigenicMapsDrawSettings& aSettings) const { return aPoint.attributes.egg ? aSettings.egg_antigen_aspect : aStyle.aspect; }
};

class DrawReferenceAntigen : public DrawAntigen
{
 public:
    virtual void draw(Surface& aSurface, const Point& aPoint, const PointStyle& aStyle, double aObjectScale, const AntigenicMapsDrawSettings& aSettings) const;
    virtual inline size_t level() const { return 2; }
};

class DrawTestAntigen : public DrawAntigen
{
 public:
    virtual void draw(Surface& aSurface, const Point& aPoint, const PointStyle& aStyle, double aObjectScale, const AntigenicMapsDrawSettings& aSettings) const;
    virtual inline size_t level() const { return 3; }
};

class DrawSequencedAntigen : public DrawAntigen
{
 public:
    virtual void draw(Surface& aSurface, const Point& aPoint, const PointStyle& aStyle, double aObjectScale, const AntigenicMapsDrawSettings& aSettings) const;
    virtual inline size_t level() const { return 4; }
};

class DrawTrackedAntigen : public DrawAntigen
{
 public:
    inline DrawTrackedAntigen(Color aFillColor = "pink") : mColor(aFillColor) {}

    virtual void draw(Surface& aSurface, const Point& aPoint, const PointStyle& aStyle, double aObjectScale, const AntigenicMapsDrawSettings& aSettings) const;
    virtual inline size_t level() const { return 5; }
    inline void color(Color aColor) { mColor = aColor; }

 private:
    Color mColor;
};

class DrawTrackedSerum : public DrawSerum
{
 public:
    inline DrawTrackedSerum(Color aOutlineColor = 0, double aOutlineWidth = 1) : mOutlineColor(aOutlineColor), mOutlineWidth(aOutlineWidth) {}

    virtual void draw(Surface& aSurface, const Point& aPoint, const PointStyle& aStyle, double aObjectScale, const AntigenicMapsDrawSettings& aSettings) const;
    virtual inline size_t level() const { return 6; }
    virtual inline Color outline_color(const Point& /*aPoint*/, const PointStyle& /*aStyle*/, const AntigenicMapsDrawSettings& /*aSettings*/) const { return mOutlineColor; }
    virtual double outline_width(const Point& /*aPoint*/, const PointStyle& /*aStyle*/, const AntigenicMapsDrawSettings& /*aSettings*/) const { return mOutlineWidth; }

 private:
    Color mOutlineColor;
    double mOutlineWidth;
};

class DrawVaccineAntigen : public DrawAntigen
{
 public:
    virtual void draw(Surface& aSurface, const Point& aPoint, const PointStyle& aStyle, double aObjectScale, const AntigenicMapsDrawSettings& aSettings) const;
    virtual inline size_t level() const { return 9; }
};

// class DrawMarkedAntigen : public DrawAntigen
// {
//  public:
//     inline DrawMarkedAntigen(const SettingsMarkAntigen& aData) : mData(aData) {}

//     virtual void draw(Surface& aSurface, const Point& aPoint, const PointStyle& aStyle, double aObjectScale, const AntigenicMapsDrawSettings& aSettings) const;
//     virtual inline size_t level() const { return 7; }

//  private:
//     SettingsMarkAntigen mData;
// };

// ----------------------------------------------------------------------

class AntigenicMapsDraw
{
 public:
    inline AntigenicMapsDraw(Surface& aSurface, Tree& aTree, Chart& aChart, AntigenicMapsDrawSettings& aSettings)
        : mSurface(aSurface), mTree(aTree), mChart(aChart), mSettings(aSettings) {}

    void init_settings(const HzSections& aHzSections);
    void prepare();
    void draw();

    inline Surface& surface() { return mSurface; }

 private:
    Surface& mSurface;
    Tree& mTree;
    Chart& mChart;
    AntigenicMapsDrawSettings& mSettings;
    Viewport mMapViewport;

    std::vector<const DrawPoint*> mDrawPoints;

    DrawSerum mDrawSerum;
      // DrawTrackedSerum mDrawTrackedSerum;
    DrawReferenceAntigen mDrawReferenceAntigen;
    DrawTestAntigen mDrawTestAntigen;
    DrawSequencedAntigen mDrawSequencedAntigen;
    DrawTrackedAntigen mDrawTrackedAntigen;
    std::vector<DrawTrackedAntigen> mDrawTrackedAntigensColoredByClade;
    DrawVaccineAntigen mDrawVaccineAntigen;
      // std::vector<DrawMarkedAntigen> mDrawMarkedAntigens;
    std::vector<DrawTrackedSerum> mDrawTrackedSera;

    void draw_points_reset();

}; // class AntigenicMapsDraw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
