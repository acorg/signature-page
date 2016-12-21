#pragma once

#include "surface.hh"

// ----------------------------------------------------------------------

class LegendSettings
{
 public:
    inline LegendSettings()
        : offset{0, 0}, width{100}, title_style{"sans_serif", TextStyle::Slant::Normal, TextStyle::Weight::Bold}, title_size{10}, text_style{"monospace"}, text_size{10}, interline{1.5} {}

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
