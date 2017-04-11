#pragma once

#include <iostream>

#include "antigenic-maps-draw.hh"
#include "antigenic-maps-layout.hh"
#include "sdb-chart.hh"

// ----------------------------------------------------------------------

namespace sdb
{

    class DrawPoint
    {
     public:
        inline DrawPoint() = default;
        inline DrawPoint(const DrawPoint&) = default;
        inline DrawPoint(DrawPoint&&) = default;
        inline virtual ~DrawPoint() = default;
        inline DrawPoint& operator=(const DrawPoint&) = default;
        virtual void draw(Surface& aSurface, const sdb::Point& aPoint, const sdb::PointStyle& aStyle, const AntigenicMapsDrawSettings& aSettings) const = 0;
        virtual size_t level() const = 0;
        virtual inline Aspect aspect(const sdb::Point&, const sdb::PointStyle& aStyle, const AntigenicMapsDrawSettings&) const { return Aspect{aStyle.aspect}; }
        virtual inline Rotation rotation(const sdb::Point& aPoint, const sdb::PointStyle& aStyle, const AntigenicMapsDrawSettings& aSettings) const { return Rotation{aPoint.attributes.reassortant ? aSettings.reassortant_rotation : aStyle.rotation}; }
        virtual inline Color fill_color(const sdb::Point&, const sdb::PointStyle& aStyle, const AntigenicMapsDrawSettings&) const { return aStyle.fill_color; }
        virtual inline Color outline_color(const sdb::Point&, const sdb::PointStyle& aStyle, const AntigenicMapsDrawSettings&) const { return aStyle.outline_color; }
    };

    class DrawSerum : public DrawPoint
    {
     public:
        virtual void draw(Surface& aSurface, const sdb::Point& aPoint, const sdb::PointStyle& aStyle, const AntigenicMapsDrawSettings& aSettings) const;
        virtual inline size_t level() const { return 1; }
        virtual inline Color outline_color(const sdb::Point&, const sdb::PointStyle&, const AntigenicMapsDrawSettings& aSettings) const { return aSettings.serum_outline_color; }
        virtual inline double outline_width(const sdb::Point&, const sdb::PointStyle&, const AntigenicMapsDrawSettings& aSettings) const { return aSettings.serum_outline_width; }
    };

    class DrawAntigen : public DrawPoint
    {
     public:
        virtual inline Aspect aspect(const sdb::Point& aPoint, const sdb::PointStyle& aStyle, const AntigenicMapsDrawSettings& aSettings) const { return Aspect{aPoint.attributes.egg ? aSettings.egg_antigen_aspect : aStyle.aspect}; }
    };

    class DrawReferenceAntigen : public DrawAntigen
    {
     public:
        virtual void draw(Surface& aSurface, const sdb::Point& aPoint, const sdb::PointStyle& aStyle, const AntigenicMapsDrawSettings& aSettings) const;
        virtual inline size_t level() const { return 2; }
    };

    class DrawTestAntigen : public DrawAntigen
    {
     public:
        virtual void draw(Surface& aSurface, const sdb::Point& aPoint, const sdb::PointStyle& aStyle, const AntigenicMapsDrawSettings& aSettings) const;
        virtual inline size_t level() const { return 3; }
    };

    class DrawSequencedAntigen : public DrawAntigen
    {
     public:
        virtual void draw(Surface& aSurface, const sdb::Point& aPoint, const sdb::PointStyle& aStyle, const AntigenicMapsDrawSettings& aSettings) const;
        virtual inline size_t level() const { return 4; }
    };

    class DrawTrackedAntigen : public DrawAntigen
    {
     public:
        inline DrawTrackedAntigen(Color aFillColor = "pink") : mColor(aFillColor) {}

        virtual void draw(Surface& aSurface, const sdb::Point& aPoint, const sdb::PointStyle& aStyle, const AntigenicMapsDrawSettings& aSettings) const;
        virtual inline size_t level() const { return 5; }
        inline void color(Color aColor) { mColor = aColor; }

     private:
        Color mColor;
    };

    class DrawTrackedSerum : public DrawSerum
    {
     public:
        inline DrawTrackedSerum(Color aOutlineColor = 0, double aOutlineWidth = 1) : mOutlineColor(aOutlineColor), mOutlineWidth(aOutlineWidth) {}

        virtual void draw(Surface& aSurface, const sdb::Point& aPoint, const sdb::PointStyle& aStyle, const AntigenicMapsDrawSettings& aSettings) const;
        virtual inline size_t level() const { return 6; }
        virtual inline Color outline_color(const sdb::Point& /*aPoint*/, const sdb::PointStyle& /*aStyle*/, const AntigenicMapsDrawSettings& /*aSettings*/) const { return mOutlineColor; }
        virtual double outline_width(const sdb::Point& /*aPoint*/, const sdb::PointStyle& /*aStyle*/, const AntigenicMapsDrawSettings& /*aSettings*/) const { return mOutlineWidth; }

     private:
        Color mOutlineColor;
        double mOutlineWidth;
    };

    class DrawVaccineAntigen : public DrawAntigen
    {
     public:
        virtual void draw(Surface& aSurface, const sdb::Point& aPoint, const sdb::PointStyle& aStyle, const AntigenicMapsDrawSettings& aSettings) const;
        virtual inline size_t level() const { return 9; }
    };

    class DrawMarkedAntigen : public DrawAntigen
    {
     public:
        inline DrawMarkedAntigen(const MarkAntigenSettings& aData) : mData(aData) {}

        virtual void draw(Surface& aSurface, const sdb::Point& aPoint, const sdb::PointStyle& aStyle, const AntigenicMapsDrawSettings& aSettings) const;
        virtual inline size_t level() const { return 7; }

     private:
        MarkAntigenSettings mData;
    };

// ----------------------------------------------------------------------

    class AntigenicMapsLayoutDraw : public ::AntigenicMapsLayoutDraw
    {
     public:
        inline AntigenicMapsLayoutDraw(AntigenicMapsDrawBase& aAntigenicMapsDraw) : ::AntigenicMapsLayoutDraw(aAntigenicMapsDraw) {}

        virtual void draw_chart(Surface& aSurface, size_t aSectionIndex);

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
        virtual void reset();
        virtual void mark_tracked_sera(size_t aSectionIndex);
        virtual void draw_map_title(Surface& aSurface, size_t aSectionIndex);
        virtual void mark_tracked_antigens(size_t aSectionIndex);
        virtual void mark_marked_antigens();

          // tracked_antigens_colored_by_clade
          // tracked_sera

    }; // class AntigenicMapsLayoutDraw

// ----------------------------------------------------------------------

    class AntigenicMapsDraw : public AntigenicMapsDrawBase
    {
     public:
        inline AntigenicMapsDraw(Surface& aSurface, Tree& aTree, ChartDrawBase* aChart, HzSections& aHzSections, SignaturePageDrawSettings& aSignaturePageDrawSettings, AntigenicMapsDrawSettings& aSettings)
            : AntigenicMapsDrawBase(aSurface, aTree, aHzSections, aSignaturePageDrawSettings, aSettings), mChart(aChart) {}

        virtual void init_settings();
        virtual void draw(Surface& aMappedAntigensDrawSurface);
        virtual void make_layout();
        virtual inline const ChartDrawBase& chart() const { return *mChart; }
        virtual inline ChartDrawBase& chart() { return *mChart; }

        inline const sdb::Chart& chart_sdb() const { return dynamic_cast<const sdb::Chart&>(*mChart); }

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
