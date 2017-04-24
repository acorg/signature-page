#pragma once

#include <cstdlib>

#include "acmacs-chart/chart.hh"
#include "hidb/hidb.hh"
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
    virtual inline const Viewport& viewport() const { return mChartDraw.viewport(); }
    virtual inline void viewport(const Viewport& aViewport) { mChartDraw.viewport(aViewport); }
    virtual inline std::string lab() const { return mChart->chart_info().lab(); }
    virtual inline void draw(Surface& aSurface) const { mChartDraw.draw(aSurface); }

      // returns ChartDrawBase::AntigenNotFound if not found
    virtual inline size_t find_antigen(std::string aName) const { return mChart->antigens().find_by_name_for_exact_matching(aName); }

    void apply_mods(const std::vector<AntigenicMapMod>& aMods);
    // inline void calculate_viewport() { mChartDraw.calculate_viewport(); }
    inline const Chart& chart() const { return *mChart; }
    inline ChartDraw& chart_draw() { return mChartDraw; }
    inline const ChartDraw& chart_draw() const { return mChartDraw; }

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

class VaccineMatcherLabel;

class AntigenicMapsLayoutDrawAce : public AntigenicMapsLayoutDraw
{
 public:
    inline AntigenicMapsLayoutDrawAce(AntigenicMapsDrawBase& aAntigenicMapsDraw)
        : AntigenicMapsLayoutDraw(aAntigenicMapsDraw), mHiDbSet(std::string{std::getenv("HOME")} + "/AD/data"), mHomologousAntigenForSeraFound(false) {}

    virtual void draw_chart(Surface& aSurface, size_t aSectionIndex);
    virtual void prepare_apply_mods();
    virtual void prepare_chart_for_all_sections();
    virtual void prepare_drawing_chart(size_t aSectionIndex);

 private:
    hidb::HiDbSet mHiDbSet;
    mutable bool mHomologousAntigenForSeraFound;

    inline const ChartDrawInterface& chart_draw_interface() const { return dynamic_cast<const ChartDrawInterface&>(antigenic_maps_draw().chart()); }
    inline ChartDrawInterface& chart_draw_interface() { return dynamic_cast<ChartDrawInterface&>(antigenic_maps_draw().chart()); }
    inline const Chart& chart() const { return chart_draw_interface().chart(); }
    inline const ChartDraw& chart_draw() const { return chart_draw_interface().chart_draw(); }
    inline ChartDraw& chart_draw() { return chart_draw_interface().chart_draw(); }

    void tracked_antigens(std::vector<size_t>& tracked_indices, size_t aSectionIndex) const;
    void tracked_sera(std::map<size_t, std::vector<size_t>>& tracked_indices, size_t aSectionIndex) const;
    void tracked_serum_circles(const AntigenicMapMod& mod, size_t aSectionIndex);
    void mark_vaccines(const AntigenicMapMod& mod);
    void add_label(std::shared_ptr<VaccineMatcherLabel> label, const SettingDict& data);

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
