#pragma once

#include "acmacs-draw/surface.hh"

// ----------------------------------------------------------------------

class LegendSettings
{
 public:
    LegendSettings();

    Size offset;                // in enclosing surface
    double width;                // in enclosing surface scale
    TextStyle title_style;
    double title_size;
    TextStyle text_style;
    double text_size;
    double interline;

}; // class LegendSettings

// ----------------------------------------------------------------------

class Legend
{
 public:
    inline Legend() {}
    virtual inline ~Legend() = default;

    virtual void draw(Surface& aSurface, const LegendSettings& aSettings) const = 0;
    virtual Size size() const = 0;

}; // class Legend

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
