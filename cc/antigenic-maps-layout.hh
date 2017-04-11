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

    inline const AntigenicMapsDrawSettings& settings() const { return mAntigenicMapsDraw.settings(); }
    inline Surface& surface() const { return mAntigenicMapsDraw.surface(); }
    inline const auto& hz_sections() const { return mAntigenicMapsDraw.hz_sections(); }
    inline const auto& signature_page_settings() const { return mAntigenicMapsDraw.signature_page_settings(); }
    inline const Viewport& viewport() const { return mMapViewport; }

 protected:
    AntigenicMapsDrawBase& mAntigenicMapsDraw;
    Viewport mMapViewport;

    std::map<size_t, size_t> mSequencedAntigens; // antigen_no to section_no

    virtual void reset() = 0;

    virtual void find_sequenced_antigens();

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
