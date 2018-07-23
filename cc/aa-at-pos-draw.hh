#pragma once

#include <map>

#include "acmacs-base/color.hh"
#include "acmacs-draw/surface.hh"
#include "rjson-serialize.hh"

// ----------------------------------------------------------------------

class Tree;
class TreeDraw;
class ChartDrawBase;

// ----------------------------------------------------------------------

class AAAtPosDrawSettings : public rjson::field_container_child
{
 public:
    AAAtPosDrawSettings(rjson::field_container_parent& aParent, std::string aFieldName);

    rjson::field_get_set<double> width;
    rjson::field_get_set<double> right_margin;
    rjson::field_get_set<double> line_width;
    rjson::field_get_set<double> line_length;         // fraction of the surface width
    rjson::array_field_container_child<size_t> positions;

}; // class AAAtPosDrawSettings

// ----------------------------------------------------------------------

class AAAtPosDraw
{
 public:
    inline AAAtPosDraw(acmacs::surface::Surface& aSurface, Tree& aTree, AAAtPosDrawSettings& aSettings)
        : mSurface(aSurface), mTree(aTree), mSettings(aSettings) {}

    void prepare();
    void draw();

    acmacs::surface::Surface& surface() { return mSurface; }

 private:
    acmacs::surface::Surface& mSurface;
    Tree& mTree;
    AAAtPosDrawSettings& mSettings;
    std::map<size_t, std::map<char, Color>> mColors;

}; // class AAAtPosDraw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
