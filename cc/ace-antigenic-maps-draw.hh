#pragma once

#include "acmacs-chart/chart.hh"
#include "acmacs-map-draw/draw.hh"
#include "antigenic-maps-draw.hh"
#include "chart-draw.hh"
#include "antigenic-maps-layout.hh"

// ----------------------------------------------------------------------

class ChartDrawInterface : public ChartDrawBase
{
 public:
    inline ChartDrawInterface(Chart* aChart) : mChart(aChart), mChartDraw(*aChart, 0) {}

    virtual void init_settings();
    virtual void prepare(const AntigenicMapsDrawSettings& aSettings);
    virtual const Viewport& viewport(const Transformation* aSettingsTransformation);
    virtual inline std::string lab() const { return mChart->chart_info().lab(); }
    virtual inline void draw(Surface& aSurface) const { /* mChartDraw.draw(aSurface); */ }

      // returns ChartDrawBase::AntigenNotFound if not found
    virtual inline size_t find_antigen(std::string aName) const { return mChart->antigens().find_by_name_for_exact_matching(aName); }

 private:
    std::unique_ptr<Chart> mChart;
    ChartDraw mChartDraw;

}; // class ChartDrawInterface

// ----------------------------------------------------------------------

class AntigenicMapsDraw : public AntigenicMapsDrawBase
{
 public:
    inline AntigenicMapsDraw(Surface& aSurface, Tree& aTree, Chart* aChart, HzSections& aHzSections, SignaturePageDrawSettings& aSignaturePageDrawSettings, AntigenicMapsDrawSettings& aSettings)
        : AntigenicMapsDrawBase(aSurface, aTree, aHzSections, aSignaturePageDrawSettings, aSettings), mChartDraw(aChart) {}

    virtual void make_layout();
    virtual inline const ChartDrawBase& chart() const { return mChartDraw; }
    virtual inline ChartDrawBase& chart() { return mChartDraw; }

 private:
    ChartDrawInterface mChartDraw;

}; // class AntigenicMapsDraw

// ----------------------------------------------------------------------

class AntigenicMapsLayoutDrawAce : public AntigenicMapsLayoutDraw
{
 public:
    inline AntigenicMapsLayoutDrawAce(AntigenicMapsDrawBase& aAntigenicMapsDraw) : AntigenicMapsLayoutDraw(aAntigenicMapsDraw) {}

    virtual void draw_chart(Surface& aSurface, size_t aSectionIndex);

 protected:
    virtual void reset();

}; // class AntigenicMapsLayoutDrawAce

// ----------------------------------------------------------------------

class LabelledGrid : public LabelledGridBase
{
 public:
    inline LabelledGrid(AntigenicMapsDrawBase& aAntigenicMapsDraw) : mLayoutDraw(aAntigenicMapsDraw) {}

    virtual inline AntigenicMapsLayoutDraw& layout_draw() { return mLayoutDraw; }

 private:
    AntigenicMapsLayoutDrawAce mLayoutDraw;

}; // class LabelledGrid

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
