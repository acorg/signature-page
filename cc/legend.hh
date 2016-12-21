#pragma once

#include "surface.hh"

// ----------------------------------------------------------------------

class Legend
{
 public:
    inline Legend() {}
    virtual inline ~Legend() = default;

    virtual void draw(Surface& aSurface) const = 0;
    virtual Size size() const = 0;

}; // class Legend

// ----------------------------------------------------------------------

class LegendSettings
{
 public:
    inline LegendSettings()
        : offset{0, 0}, width{100}, title_size{10}, text_size{10}, interline{1.2} {}

    Size offset;                // in enclosing surface
    double width;                // in enclosing surface scale
    TextStyle title_style;
    double title_size;
    TextStyle text_style;
    double text_size;
    double interline;

}; // class LegendSettings

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
