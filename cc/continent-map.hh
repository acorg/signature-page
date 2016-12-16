#pragma once

#include "coloring.hh"
#include "legend.hh"

// ----------------------------------------------------------------------

class ColoringByContinentMapLegend : public Legend
{

 public:
    inline ColoringByContinentMapLegend(const ColoringByContinent& aColoring) : Legend(), mColoring(aColoring) {}

    virtual void draw(Surface& aSurface) const;
    virtual Size size() const;

 private:
    const ColoringByContinent& mColoring;

}; // class ColoringByContinentMapLegend

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
