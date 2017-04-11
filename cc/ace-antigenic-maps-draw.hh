#pragma once

#include "antigenic-maps-draw.hh"
#include "chart-draw.hh"
#include "acmacs-chart/chart.hh"

// ----------------------------------------------------------------------

class ChartDraw : public ChartDrawBase
{
 public:
    inline ChartDraw(Chart* aChart) : mChart(aChart) {}

    virtual void init_settings();
    virtual void prepare(const AntigenicMapsDrawSettings& aSettings);
    virtual const Viewport& viewport(const Transformation* aSettingsTransformation);
    virtual inline std::string lab() const { return mChart->chart_info().lab(); }

      // returns ChartDrawBase::AntigenNotFound if not found
    virtual inline size_t find_antigen(std::string aName) const { return mChart->antigens().find_by_name_for_exact_matching(aName); }

 private:
    std::unique_ptr<Chart> mChart;

}; // class ChartDrawBase

// ----------------------------------------------------------------------

class AntigenicMapsDraw : public AntigenicMapsDrawBase
{
 public:
    inline AntigenicMapsDraw(Surface& aSurface, Tree& aTree, Chart* aChart, HzSections& aHzSections, SignaturePageDrawSettings& aSignaturePageDrawSettings, AntigenicMapsDrawSettings& aSettings)
        : AntigenicMapsDrawBase(aSurface, aTree, aHzSections, aSignaturePageDrawSettings, aSettings), mChartDraw(aChart) {}

    virtual void init_settings();
    virtual void prepare();
    virtual void draw(Surface& aMappedAntigensDrawSurface);
    virtual inline const ChartDrawBase& chart() const { return mChartDraw; }
    virtual inline ChartDrawBase& chart() { return mChartDraw; }

 private:
    ChartDraw mChartDraw;
    // std::unique_ptr<AntigenicMapsLayout> mLayout;

}; // class AntigenicMapsDraw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
