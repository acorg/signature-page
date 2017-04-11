#pragma once

#include <map>

#include "acmacs-draw/viewport.hh"

// ----------------------------------------------------------------------

class AntigenicMapsDrawBase;
class Surface;

class AntigenicMapsLayout
{
 public:
    inline AntigenicMapsLayout(AntigenicMapsDrawBase& aAntigenicMapsDraw) : mAntigenicMapsDraw(aAntigenicMapsDraw) {}
    virtual ~AntigenicMapsLayout();

    virtual void prepare();
    virtual void draw(Surface& aMappedAntigensDrawSurface) = 0;

 protected:
    AntigenicMapsDrawBase& mAntigenicMapsDraw;
    Viewport mMapViewport;

    std::map<size_t, size_t> mSequencedAntigens; // antigen_no to section_no

    virtual void reset() = 0;

    virtual void find_sequenced_antigens();

    virtual void draw_chart(Surface& aSurface, size_t aSectionIndex) = 0;
    virtual void mark_tracked_sera(size_t aSectionIndex) = 0;
    virtual void draw_map_title(Surface& aSurface, size_t aSectionIndex) = 0;
    virtual void mark_tracked_antigens(size_t aSectionIndex) = 0;
    virtual void mark_marked_antigens() = 0;

}; // class AntigenicMapsLayout

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
