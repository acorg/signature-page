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
    virtual void draw_chart(Surface& aSurface, size_t aSectionIndex) = 0;

    inline AntigenicMapsDrawBase& antigenic_maps_draw() { return mAntigenicMapsDraw; }
    inline const AntigenicMapsDrawSettings& settings() const { return mAntigenicMapsDraw.settings(); }
    inline const auto& chart() const { return mAntigenicMapsDraw.chart(); }
    inline Surface& surface() const { return mAntigenicMapsDraw.surface(); }
    inline const auto& hz_sections() const { return mAntigenicMapsDraw.hz_sections(); }
    inline const auto& signature_page_settings() const { return mAntigenicMapsDraw.signature_page_settings(); }
    inline const Viewport& viewport() const { return mMapViewport; }
    inline const auto& sequenced_antigens() const { return mSequencedAntigens; }

 protected:
    virtual void reset() = 0;
    virtual void apply_mods(const std::vector<AntigenicMapMod>& aMods);
    virtual void apply_mods_before(const std::vector<AntigenicMapMod>& aMods, Surface& aSurface);
    virtual void apply_mods_after(const std::vector<AntigenicMapMod>& aMods, Surface& aSurface);

    virtual void find_sequenced_antigens();

 private:
    AntigenicMapsDrawBase& mAntigenicMapsDraw;
    Viewport mMapViewport;
    std::map<size_t, size_t> mSequencedAntigens; // antigen_no to section_no

}; // class AntigenicMapsLayoutDraw

// ----------------------------------------------------------------------

class AntigenicMapsLayout
{
 public:
    virtual ~AntigenicMapsLayout();

    virtual AntigenicMapsLayoutDraw& layout_draw() = 0;
    inline void prepare() { layout_draw().prepare(); }
    virtual void draw(Surface& aMappedAntigensDrawSurface) = 0;

}; // class AntigenicMapsLayout

// ----------------------------------------------------------------------

class LabelledGridBase : public AntigenicMapsLayout
{
 public:
    virtual void draw(Surface& aMappedAntigensDrawSurface);

 private:
    void draw_mapped_antigens_section(size_t aSectionIndex, Surface& aMappedAntigensDrawSurface);

}; // class LabelledGridBase

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
