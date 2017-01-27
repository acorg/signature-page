#pragma once

#include <iostream>

#include "acmacs-draw/surface.hh"
#include "chart.hh"

// ----------------------------------------------------------------------

class Tree;
class TreeDraw;
class HzSections;

// ----------------------------------------------------------------------

class MarkAntigenSettings
{
 public:
    MarkAntigenSettings(bool aShow = true, std::string aName = std::string{});

    bool show;
    std::string name, label;
    double scale, aspect, rotation, outline_width, label_line_width;
    Color fill_color, outline_color, label_color, label_line_color;
    Size label_offset;
    double label_size;

}; // class MarkAntigenSettings

// ----------------------------------------------------------------------

class AntigenicMapsDrawSettings
{
 public:
    AntigenicMapsDrawSettings();
    ~AntigenicMapsDrawSettings();

    std::string layout;
    double width;
    size_t columns;
    double gap;
    Transformation transformation;
    Viewport viewport;

    double border_width, grid_line_width;
    Color border_color, grid_line_color, background_color;

    double point_scale;
    double serum_scale, reference_antigen_scale, test_antigen_scale, vaccine_antigen_scale, tracked_antigen_scale;
    double serum_outline_width, reference_antigen_outline_width, test_antigen_outline_width, vaccine_antigen_outline_width, sequenced_antigen_outline_width, tracked_antigen_outline_width;
    Color serum_outline_color, reference_antigen_outline_color, test_antigen_outline_color, test_antigen_fill_color, vaccine_antigen_outline_color, sequenced_antigen_outline_color, sequenced_antigen_fill_color, tracked_antigen_outline_color;
    bool tracked_antigen_colored_by_clade;
    Color tracked_antigen_color;
    double reassortant_rotation, egg_antigen_aspect;
    bool show_tracked_sera;
    Color serum_circle_color, tracked_serum_outline_color;
    double serum_circle_thickness, tracked_serum_outline_width;
    Color map_title_color;
    Size map_title_offset;      // offset of the top left corner of the text in letter W fraction
    double map_title_size;
    Color mapped_antigens_section_line_color;
    double mapped_antigens_section_line_width;

    std::vector<MarkAntigenSettings> mark_antigens;

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
    virtual void draw(Surface& aSurface, const Point& aPoint, const PointStyle& aStyle, const AntigenicMapsDrawSettings& aSettings) const = 0;
    virtual size_t level() const = 0;
    virtual inline double aspect(const Point&, const PointStyle& aStyle, const AntigenicMapsDrawSettings&) const { return aStyle.aspect; }
    virtual inline double rotation(const Point& aPoint, const PointStyle& aStyle, const AntigenicMapsDrawSettings& aSettings) const { return aPoint.attributes.reassortant ? aSettings.reassortant_rotation : aStyle.rotation; }
    virtual inline Color fill_color(const Point&, const PointStyle& aStyle, const AntigenicMapsDrawSettings&) const { return aStyle.fill_color; }
    virtual inline Color outline_color(const Point&, const PointStyle& aStyle, const AntigenicMapsDrawSettings&) const { return aStyle.outline_color; }
};

class DrawSerum : public DrawPoint
{
 public:
    virtual void draw(Surface& aSurface, const Point& aPoint, const PointStyle& aStyle, const AntigenicMapsDrawSettings& aSettings) const;
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
    virtual void draw(Surface& aSurface, const Point& aPoint, const PointStyle& aStyle, const AntigenicMapsDrawSettings& aSettings) const;
    virtual inline size_t level() const { return 2; }
};

class DrawTestAntigen : public DrawAntigen
{
 public:
    virtual void draw(Surface& aSurface, const Point& aPoint, const PointStyle& aStyle, const AntigenicMapsDrawSettings& aSettings) const;
    virtual inline size_t level() const { return 3; }
};

class DrawSequencedAntigen : public DrawAntigen
{
 public:
    virtual void draw(Surface& aSurface, const Point& aPoint, const PointStyle& aStyle, const AntigenicMapsDrawSettings& aSettings) const;
    virtual inline size_t level() const { return 4; }
};

class DrawTrackedAntigen : public DrawAntigen
{
 public:
    inline DrawTrackedAntigen(Color aFillColor = "pink") : mColor(aFillColor) {}

    virtual void draw(Surface& aSurface, const Point& aPoint, const PointStyle& aStyle, const AntigenicMapsDrawSettings& aSettings) const;
    virtual inline size_t level() const { return 5; }
    inline void color(Color aColor) { mColor = aColor; }

 private:
    Color mColor;
};

class DrawTrackedSerum : public DrawSerum
{
 public:
    inline DrawTrackedSerum(Color aOutlineColor = 0, double aOutlineWidth = 1) : mOutlineColor(aOutlineColor), mOutlineWidth(aOutlineWidth) {}

    virtual void draw(Surface& aSurface, const Point& aPoint, const PointStyle& aStyle, const AntigenicMapsDrawSettings& aSettings) const;
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
    virtual void draw(Surface& aSurface, const Point& aPoint, const PointStyle& aStyle, const AntigenicMapsDrawSettings& aSettings) const;
    virtual inline size_t level() const { return 9; }
};

class DrawMarkedAntigen : public DrawAntigen
{
 public:
    inline DrawMarkedAntigen(const MarkAntigenSettings& aData) : mData(aData) {}

    virtual void draw(Surface& aSurface, const Point& aPoint, const PointStyle& aStyle, const AntigenicMapsDrawSettings& aSettings) const;
    virtual inline size_t level() const { return 7; }

 private:
    MarkAntigenSettings mData;
};

// ----------------------------------------------------------------------

class AntigenicMapsDraw;
class HzSection;
class MappedAntigensDraw;

class AntigenicMapsLayout
{
 public:
    inline AntigenicMapsLayout(AntigenicMapsDraw& aAntigenicMapsDraw) : mAntigenicMapsDraw(aAntigenicMapsDraw) {}
    virtual ~AntigenicMapsLayout();

    virtual void prepare();
    virtual void draw() = 0;

 protected:
    AntigenicMapsDraw& mAntigenicMapsDraw;
    Viewport mMapViewport;

    std::vector<const DrawPoint*> mDrawPoints;
    std::map<size_t, size_t> mSequencedAntigens; // antigen_no to section_no

    DrawSerum mDrawSerum;
    DrawReferenceAntigen mDrawReferenceAntigen;
    DrawTestAntigen mDrawTestAntigen;
    DrawSequencedAntigen mDrawSequencedAntigen;
    DrawTrackedAntigen mDrawTrackedAntigen;
    std::vector<DrawTrackedAntigen> mDrawTrackedAntigensColoredByClade;
    DrawVaccineAntigen mDrawVaccineAntigen;
    std::vector<DrawMarkedAntigen> mDrawMarkedAntigens;
    std::vector<DrawTrackedSerum> mDrawTrackedSera;

    virtual void find_sequenced_antigens();
    virtual void draw_points_reset();
    virtual void draw_chart(Surface& aSurface, size_t aSectionIndex);
    virtual void mark_tracked_sera(size_t aSectionIndex);
    virtual void draw_map_title(Surface& aSurface, size_t aSectionIndex);
    virtual void mark_tracked_antigens(size_t aSectionIndex);
    virtual void mark_marked_antigens();

      // tracked_antigens_colored_by_clade
      // tracked_sera

}; // class AntigenicMapsLayout

// ----------------------------------------------------------------------

class AntigenicMapsDraw
{
 public:
    inline AntigenicMapsDraw(Surface& aSurface, Tree& aTree, Chart& aChart, HzSections& aHzSections, MappedAntigensDraw& aMappedAntigensDraw, AntigenicMapsDrawSettings& aSettings)
        : mSurface(aSurface), mTree(aTree), mChart(aChart), mHzSections(aHzSections), mMappedAntigensDraw(aMappedAntigensDraw), mSettings(aSettings) {}

    void init_settings();
    void prepare();
    void draw();

    inline Surface& surface() { return mSurface; }
    inline const Tree& tree() const { return mTree; }
    inline const Chart& chart() const { return mChart; }
    inline const HzSections& hz_sections() const { return mHzSections; }
    inline MappedAntigensDraw& mapped_antigens_draw() { return mMappedAntigensDraw; }
    inline const AntigenicMapsDrawSettings& settings() const { return mSettings; }

 private:
    Surface& mSurface;
    Tree& mTree;
    Chart& mChart;
    HzSections& mHzSections;
    MappedAntigensDraw& mMappedAntigensDraw;
    AntigenicMapsDrawSettings& mSettings;
    std::unique_ptr<AntigenicMapsLayout> mLayout;

}; // class AntigenicMapsDraw

// ----------------------------------------------------------------------

class LabelledGrid : public AntigenicMapsLayout
{
 public:
    inline LabelledGrid(AntigenicMapsDraw& aAntigenicMapsDraw) : AntigenicMapsLayout(aAntigenicMapsDraw) {}

    virtual void prepare();
    virtual void draw();

 protected:
    virtual void draw_chart(Surface& aSurface, size_t aSectionIndex);
    virtual void draw_mapped_antigens_section(size_t aSectionIndex);

}; // class LabelledGrid

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
