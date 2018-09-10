#pragma once

#include "acmacs-draw/surface.hh"
#include "rjson-serialize.hh"

// ----------------------------------------------------------------------

class LegendSettings : public rjson::v1::field_container_child
{
 public:
    LegendSettings(rjson::v1::field_container_parent& aParent, std::string aFieldName);

    rjson::v1::field_get_set<acmacs::Offset> offset;  // in enclosing surface
    rjson::v1::field_get_set<double> width; // in enclosing surface scale
    rjson::v1::field_get_set<acmacs::TextStyle> title_style;
    rjson::v1::field_get_set<double> title_size;
    rjson::v1::field_get_set<acmacs::TextStyle> text_style;
    rjson::v1::field_get_set<double> text_size;
    rjson::v1::field_get_set<double> interline;

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
