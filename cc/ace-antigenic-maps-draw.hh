#pragma once

#include <cstdlib>

#include "acmacs-chart-2/chart-modify.hh"
#include "hidb-5/hidb.hh"
#include "acmacs-map-draw/draw.hh"
#include "antigenic-maps-draw.hh"
#include "chart-draw.hh"
#include "antigenic-maps-layout.hh"

// ----------------------------------------------------------------------

class ChartDrawInterface : public ChartDrawBase
{
 public:
    ChartDrawInterface(acmacs::chart::ChartModifyP aChart) : mChartDraw(aChart, 0) {}

    void init_settings() override;
    const acmacs::Viewport& viewport() const override { return mChartDraw.viewport(); }
    virtual void viewport(const acmacs::Viewport& aViewport) { mChartDraw.viewport(aViewport); }
    std::string lab() const override { return chart().info()->lab(); }
    void draw(acmacs::surface::Surface& aSurface) const override { mChartDraw.draw(aSurface); }

    std::optional<size_t> find_antigen(std::string aName) const override { return chart().antigens()->find_by_full_name(aName); }

    void apply_mods(const std::vector<AntigenicMapMod>& aMods);
    // inline void calculate_viewport() { mChartDraw.calculate_viewport(); }
    const acmacs::chart::Chart& chart() const override { return mChartDraw.chart(); }
    ChartDraw& chart_draw() { return mChartDraw; }
    const ChartDraw& chart_draw() const { return mChartDraw; }

 private:
    ChartDraw mChartDraw;

}; // class ChartDrawInterface

// ----------------------------------------------------------------------

class AntigenicMapsDraw : public AntigenicMapsDrawBase
{
 public:
    AntigenicMapsDraw(acmacs::surface::Surface& aSurface, Tree& aTree, acmacs::chart::ChartModifyP aChart, HzSections& aHzSections, SignaturePageDrawSettings& aSignaturePageDrawSettings, AntigenicMapsDrawSettings& aSettings)
        : AntigenicMapsDrawBase(aSurface, aTree, aHzSections, aSignaturePageDrawSettings, aSettings), mChartDraw(aChart) {}

    void make_layout() override;
    const ChartDrawBase& chart() const override { return mChartDraw; }
    ChartDrawBase& chart() override { return mChartDraw; }

 private:
    ChartDrawInterface mChartDraw;

}; // class AntigenicMapsDraw

// ----------------------------------------------------------------------

class VaccineMatcherLabel;

class AntigenicMapsLayoutDrawAce : public AntigenicMapsLayoutDraw
{
 public:
    AntigenicMapsLayoutDrawAce(AntigenicMapsDrawBase& aAntigenicMapsDraw)
        : AntigenicMapsLayoutDraw(aAntigenicMapsDraw), mHomologousAntigenForSeraFound(false) {}

    void draw_chart(acmacs::surface::Surface& aSurface, size_t aSectionIndex) override;
    void prepare_apply_mods() override;
    void prepare_chart_for_all_sections() override;
    void prepare_drawing_chart(size_t aSectionIndex, bool report_antigens_in_hz_sections) override;

 private:
    mutable bool mHomologousAntigenForSeraFound;

    const ChartDrawInterface& chart_draw_interface() const { return dynamic_cast<const ChartDrawInterface&>(antigenic_maps_draw().chart()); }
    ChartDrawInterface& chart_draw_interface() { return dynamic_cast<ChartDrawInterface&>(antigenic_maps_draw().chart()); }
    const acmacs::chart::Chart& chart() const { return chart_draw_interface().chart(); }
    const ChartDraw& chart_draw() const { return chart_draw_interface().chart_draw(); }
    ChartDraw& chart_draw() { return chart_draw_interface().chart_draw(); }

    acmacs::chart::PointIndexList tracked_antigens(size_t aSectionIndex, bool report_antigens_in_hz_sections) const;
    std::map<size_t, acmacs::chart::PointIndexList> tracked_sera(size_t aSectionIndex) const;
    void tracked_serum_circles(const AntigenicMapMod& mod, size_t aSectionIndex);
    void mark_vaccines(const AntigenicMapMod& mod);
    void mark_antigens(const AntigenicMapMod& mod);
    void mark_antigens_old(const AntigenicMapMod& mod);
    void add_label(std::shared_ptr<VaccineMatcherLabel> label, const rjson::value& data);

}; // class AntigenicMapsLayoutDrawAce

// ----------------------------------------------------------------------

class LabelledGrid : public LabelledGridBase
{
 public:
    LabelledGrid(AntigenicMapsDrawBase& aAntigenicMapsDraw) : mLayoutDraw(aAntigenicMapsDraw) {}

    AntigenicMapsLayoutDraw& layout_draw() override { return mLayoutDraw; }

 private:
    AntigenicMapsLayoutDrawAce mLayoutDraw;

}; // class LabelledGrid

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
