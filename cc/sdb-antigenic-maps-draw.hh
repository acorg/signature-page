#pragma once

#include <iostream>

#include "antigenic-maps-draw.hh"
#include "antigenic-maps-layout.hh"
#include "sdb-chart.hh"

// ----------------------------------------------------------------------

namespace sdb
{

    class DrawPointSettings
    {
     public:
        DrawPointSettings();

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
    };

    class DrawPoint
    {
     public:
        inline DrawPoint() = default;
        inline DrawPoint(const DrawPoint&) = default;
        inline DrawPoint(DrawPoint&&) = default;
        inline virtual ~DrawPoint() = default;
        inline DrawPoint& operator=(const DrawPoint&) = default;
        virtual void draw(Surface& aSurface, const sdb::Point& aPoint, const sdb::PointStyle& aStyle, const DrawPointSettings& aSettings) const = 0;
        virtual size_t level() const = 0;
        virtual inline Aspect aspect(const sdb::Point&, const sdb::PointStyle& aStyle, const DrawPointSettings&) const { return Aspect{aStyle.aspect}; }
        virtual inline Rotation rotation(const sdb::Point& aPoint, const sdb::PointStyle& aStyle, const DrawPointSettings& aSettings) const { return Rotation{aPoint.attributes.reassortant ? aSettings.reassortant_rotation : aStyle.rotation}; }
        virtual inline Color fill_color(const sdb::Point&, const sdb::PointStyle& aStyle, const DrawPointSettings&) const { return aStyle.fill_color; }
        virtual inline Color outline_color(const sdb::Point&, const sdb::PointStyle& aStyle, const DrawPointSettings&) const { return aStyle.outline_color; }
    };

    class DrawSerum : public DrawPoint
    {
     public:
        virtual void draw(Surface& aSurface, const sdb::Point& aPoint, const sdb::PointStyle& aStyle, const DrawPointSettings& aSettings) const;
        virtual inline size_t level() const { return 1; }
        virtual inline Color outline_color(const sdb::Point&, const sdb::PointStyle&, const DrawPointSettings& aSettings) const { return aSettings.serum_outline_color; }
        virtual inline double outline_width(const sdb::Point&, const sdb::PointStyle&, const DrawPointSettings& aSettings) const { return aSettings.serum_outline_width; }
    };

    class DrawAntigen : public DrawPoint
    {
     public:
        virtual inline Aspect aspect(const sdb::Point& aPoint, const sdb::PointStyle& aStyle, const DrawPointSettings& aSettings) const { return Aspect{aPoint.attributes.egg ? aSettings.egg_antigen_aspect : aStyle.aspect}; }
    };

    class DrawReferenceAntigen : public DrawAntigen
    {
     public:
        virtual void draw(Surface& aSurface, const sdb::Point& aPoint, const sdb::PointStyle& aStyle, const DrawPointSettings& aSettings) const;
        virtual inline size_t level() const { return 2; }
    };

    class DrawTestAntigen : public DrawAntigen
    {
     public:
        virtual void draw(Surface& aSurface, const sdb::Point& aPoint, const sdb::PointStyle& aStyle, const DrawPointSettings& aSettings) const;
        virtual inline size_t level() const { return 3; }
    };

    class DrawSequencedAntigen : public DrawAntigen
    {
     public:
        virtual void draw(Surface& aSurface, const sdb::Point& aPoint, const sdb::PointStyle& aStyle, const DrawPointSettings& aSettings) const;
        virtual inline size_t level() const { return 4; }
    };

    class DrawTrackedAntigen : public DrawAntigen
    {
     public:
        inline DrawTrackedAntigen(Color aFillColor = "green3") : mColor(aFillColor) {}

        virtual void draw(Surface& aSurface, const sdb::Point& aPoint, const sdb::PointStyle& aStyle, const DrawPointSettings& aSettings) const;
        virtual inline size_t level() const { return 5; }
        inline void color(Color aColor) { mColor = aColor; }

     private:
        Color mColor;
    };

    class DrawTrackedSerum : public DrawSerum
    {
     public:
        inline DrawTrackedSerum(Color aOutlineColor = 0, double aOutlineWidth = 1) : mOutlineColor(aOutlineColor), mOutlineWidth(aOutlineWidth) {}

        virtual void draw(Surface& aSurface, const sdb::Point& aPoint, const sdb::PointStyle& aStyle, const DrawPointSettings& aSettings) const;
        virtual inline size_t level() const { return 6; }
        virtual inline Color outline_color(const sdb::Point& /*aPoint*/, const sdb::PointStyle& /*aStyle*/, const DrawPointSettings& /*aSettings*/) const { return mOutlineColor; }
        virtual double outline_width(const sdb::Point& /*aPoint*/, const sdb::PointStyle& /*aStyle*/, const DrawPointSettings& /*aSettings*/) const { return mOutlineWidth; }

     private:
        Color mOutlineColor;
        double mOutlineWidth;
    };

    class DrawVaccineAntigen : public DrawAntigen
    {
     public:
        virtual void draw(Surface& aSurface, const sdb::Point& aPoint, const sdb::PointStyle& aStyle, const DrawPointSettings& aSettings) const;
        virtual inline size_t level() const { return 9; }
    };

    class DrawMarkedAntigen : public DrawAntigen
    {
     public:
        inline DrawMarkedAntigen(const MarkAntigenSettings& aData) : mData(aData) {}

        virtual void draw(Surface& aSurface, const sdb::Point& aPoint, const sdb::PointStyle& aStyle, const DrawPointSettings& aSettings) const;
        virtual inline size_t level() const { return 7; }

     private:
        MarkAntigenSettings mData;
    };

// ----------------------------------------------------------------------

    class AntigenicMapsLayoutDraw : public ::AntigenicMapsLayoutDraw
    {
     public:
        inline AntigenicMapsLayoutDraw(AntigenicMapsDrawBase& aAntigenicMapsDraw)
            : ::AntigenicMapsLayoutDraw(aAntigenicMapsDraw) {}

        virtual void draw_chart(Surface& aSurface, size_t aSectionIndex);
        virtual void prepare_apply_mods();
        void prepare_drawing_char();

     protected:
        std::vector<const DrawPoint*> mDrawPoints;

        DrawSerum mDrawSerum;
        DrawReferenceAntigen mDrawReferenceAntigen;
        DrawTestAntigen mDrawTestAntigen;
        DrawSequencedAntigen mDrawSequencedAntigen;
        DrawTrackedAntigen mDrawTrackedAntigen;
        std::vector<DrawTrackedAntigen> mDrawTrackedAntigensColoredByClade;
        DrawVaccineAntigen mDrawVaccineAntigen;
        std::vector<DrawMarkedAntigen> mDrawMarkedAntigens;
        std::vector<DrawTrackedSerum> mDrawTrackedSera;

        // virtual void find_sequenced_antigens();

          // tracked_antigens_colored_by_clade
          // tracked_sera

     private:
        void apply_mods_to_settings(DrawPointSettings& aSettings);
        void mark_tracked_antigens(size_t aSectionIndex, const DrawPointSettings& aSettings);
        void mark_tracked_sera(size_t aSectionIndex, const DrawPointSettings& aSettings);
        void draw_map_title(Surface& aSurface, size_t aSectionIndex, const DrawPointSettings& aSettings);
        void mark_marked_antigens();

    }; // class AntigenicMapsLayoutDraw

// ----------------------------------------------------------------------

    class AntigenicMapsDraw : public AntigenicMapsDrawBase
    {
     public:
        inline AntigenicMapsDraw(Surface& aSurface, Tree& aTree, ChartDrawBase* aChart, HzSections& aHzSections, SignaturePageDrawSettings& aSignaturePageDrawSettings, AntigenicMapsDrawSettings& aSettings)
            : AntigenicMapsDrawBase(aSurface, aTree, aHzSections, aSignaturePageDrawSettings, aSettings), mChart(aChart) {}

        virtual void make_layout();
        virtual inline const ChartDrawBase& chart() const { return *mChart; }
        virtual inline ChartDrawBase& chart() { return *mChart; }

        inline sdb::Chart& chart_sdb() { return dynamic_cast<sdb::Chart&>(*mChart); }

     private:
        std::unique_ptr<ChartDrawBase> mChart;

    }; // class AntigenicMapsDraw

// ----------------------------------------------------------------------

    class LabelledGrid : public LabelledGridBase
    {
     public:
        inline LabelledGrid(AntigenicMapsDrawBase& aAntigenicMapsDraw) : mLayoutDraw(aAntigenicMapsDraw) {}

        virtual inline ::AntigenicMapsLayoutDraw& layout_draw() { return mLayoutDraw; }

     private:
        AntigenicMapsLayoutDraw mLayoutDraw;

    }; // class LabelledGrid

// ----------------------------------------------------------------------

} // namespace sdb

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
