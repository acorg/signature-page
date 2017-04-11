#pragma once

#include <map>

#include "acmacs-draw/viewport.hh"
#include "antigenic-maps-draw.hh"

// ----------------------------------------------------------------------

class AntigenicMapsLayout
{
 public:
    inline AntigenicMapsLayout(AntigenicMapsDrawBase& aAntigenicMapsDraw) : mAntigenicMapsDraw(aAntigenicMapsDraw) {}
    virtual ~AntigenicMapsLayout();

    virtual void prepare();
    virtual void draw(Surface& aMappedAntigensDrawSurface) = 0;
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

    // virtual void mark_tracked_sera(size_t aSectionIndex) = 0;
    // virtual void draw_map_title(Surface& aSurface, size_t aSectionIndex) = 0;
    // virtual void mark_tracked_antigens(size_t aSectionIndex) = 0;
    // virtual void mark_marked_antigens() = 0;

}; // class AntigenicMapsLayout

// ----------------------------------------------------------------------

class LabelledGrid
{
 public:
    inline LabelledGrid(AntigenicMapsLayout& aLayout) : mLayout(aLayout) {}

    inline AntigenicMapsLayout& layout() { return mLayout; }
    void prepare();
    void draw(Surface& aMappedAntigensDrawSurface);

 private:
    AntigenicMapsLayout& mLayout;

      // virtual void draw_chart(Surface& aSurface, size_t aSectionIndex);
    void draw_mapped_antigens_section(size_t aSectionIndex, Surface& aMappedAntigensDrawSurface);

}; // class LabelledGrid

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
