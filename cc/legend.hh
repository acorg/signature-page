#pragma once

#include "acmacs-base/settings.hh"
#include "acmacs-draw/surface.hh"

// ----------------------------------------------------------------------

class LegendSettings : public acmacs::settings::object
{
 public:
    using acmacs::settings::object::object;

    acmacs::settings::field<acmacs::Offset>    offset{this, "offset", {-30, 950}};  // in enclosing surface
    acmacs::settings::field<double>            width{this, "width", 100}; // in enclosing surface scale
    acmacs::settings::field<acmacs::TextStyle> title_style{this, "title_style", {"sans_serif"}};
    acmacs::settings::field<double>            title_size{this, "title_size", 10};
    acmacs::settings::field<acmacs::TextStyle> text_style{this, "text_style", {"monospace"}};
    acmacs::settings::field<double>            text_size{this, "text_size", 10};
    acmacs::settings::field<double>            interline{this, "interline", 1.5};

}; // class LegendSettings

// ----------------------------------------------------------------------

class Legend
{
 public:
    inline Legend() {}
    virtual inline ~Legend() = default;

    virtual void draw(acmacs::surface::Surface& aSurface, const LegendSettings& aSettings) const = 0;
    virtual acmacs::Size size() const = 0;

}; // class Legend

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
