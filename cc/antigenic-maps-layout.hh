#pragma once

#include <map>

#include "acmacs-draw/viewport.hh"
#include "antigenic-maps-draw.hh"

// ----------------------------------------------------------------------

class AntigenicMapsLayoutDraw
{
 public:
    inline AntigenicMapsLayoutDraw(AntigenicMapsDrawBase& aAntigenicMapsDraw) : mAntigenicMapsDraw(aAntigenicMapsDraw) {}
    virtual ~AntigenicMapsLayoutDraw();

    virtual void prepare();
    virtual void prepare_apply_mods() = 0;
    virtual void prepare_chart_for_all_sections() = 0;
    virtual void prepare_drawing_chart(size_t aSectionIndex, bool report_antigens_in_hz_sections) = 0;
    virtual void draw_chart(Surface& aSurface, size_t aSectionIndex) = 0;

    inline AntigenicMapsDrawBase& antigenic_maps_draw() { return mAntigenicMapsDraw; }
    inline const AntigenicMapsDrawBase& antigenic_maps_draw() const { return mAntigenicMapsDraw; }
    inline AntigenicMapsDrawSettings& settings() { return mAntigenicMapsDraw.settings(); }
    inline const AntigenicMapsDrawSettings& settings() const { return mAntigenicMapsDraw.settings(); }
    inline auto& chart() { return mAntigenicMapsDraw.chart(); }
    inline const auto& chart() const { return mAntigenicMapsDraw.chart(); }
    inline Surface& surface() const { return mAntigenicMapsDraw.surface(); }
    inline const auto& hz_sections() const { return mAntigenicMapsDraw.hz_sections(); }
    inline const auto& signature_page_settings() const { return mAntigenicMapsDraw.signature_page_settings(); }
    inline auto& signature_page_settings() { return mAntigenicMapsDraw.signature_page_settings(); }
    const Viewport& viewport() const;
    inline const auto& sequenced_antigens() const { return mSequencedAntigens; }

 protected:
    virtual void apply_mods_before(Surface& aSurface);
    virtual void apply_mods_after(Surface& aSurface);

    virtual void find_sequenced_antigens();

 private:
    AntigenicMapsDrawBase& mAntigenicMapsDraw;
    std::map<size_t, size_t> mSequencedAntigens; // antigen_no to section_no

}; // class AntigenicMapsLayoutDraw

// ----------------------------------------------------------------------

class AntigenicMapsLayout
{
 public:
    virtual ~AntigenicMapsLayout();

    virtual AntigenicMapsLayoutDraw& layout_draw() = 0;
    virtual inline void prepare() { layout_draw().prepare(); }
    virtual void draw(Surface& aMappedAntigensDrawSurface, bool report_antigens_in_hz_sections) = 0;

}; // class AntigenicMapsLayout

// ----------------------------------------------------------------------

class LabelledGridBase : public AntigenicMapsLayout
{
 public:
    void draw(Surface& aMappedAntigensDrawSurface, bool report_antigens_in_hz_sections) override;

 private:
    void draw_mapped_antigens_section(size_t aSectionIndex, Surface& aMappedAntigensDrawSurface);

}; // class LabelledGridBase

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
