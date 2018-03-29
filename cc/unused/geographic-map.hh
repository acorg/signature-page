#pragma once

#include "legend.hh"

// ----------------------------------------------------------------------

class ColoringByContinent;

// ----------------------------------------------------------------------

class ColoringByGeographyMapLegend : public Legend
{

 public:
    inline ColoringByGeographyMapLegend(const ColoringByContinent& aColoring) : Legend(), mColoring(aColoring) {}

    virtual void draw(acmacs::surface::Surface& aSurface) const;
    virtual Size size() const;

 private:
    const ColoringByContinent& mColoring;

      // cairo_path_t* outline(acmacs::surface::Surface& aSurface, const std::vector<GeographicMapPathElement>& aPath) const;

}; // class ColoringByGeographyMapLegend

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
