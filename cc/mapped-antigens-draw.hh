#pragma once

#include <iostream>

#include "acmacs-draw/surface.hh"
#include "rjson-serialize.hh"

// ----------------------------------------------------------------------

class Tree;
class TreeDraw;
class ChartDrawBase;

// ----------------------------------------------------------------------

class MappedAntigensDrawSettings : public rjson::v1::field_container_child
{
 public:
    MappedAntigensDrawSettings(rjson::v1::field_container_parent& aParent, std::string aFieldName);

    rjson::v1::field_get_set<double> width;
    rjson::v1::field_get_set<double> line_width;
    rjson::v1::field_get_set<Color> line_color;
    rjson::v1::field_get_set<double> line_length;         // fraction of the surface width

}; // class MappedAntigensDrawSettings

// ----------------------------------------------------------------------

class MappedAntigensDraw
{
 public:
    inline MappedAntigensDraw(acmacs::surface::Surface& aSurface, Tree& aTree, /* const TreeDraw& aTreeDraw, */ const ChartDrawBase& aChart, MappedAntigensDrawSettings& aSettings)
        : mSurface(aSurface), mTree(aTree), /* mTreeDraw(aTreeDraw), */ mChart(aChart), mSettings(aSettings) {}

    void prepare();
    void draw();

    inline acmacs::surface::Surface& surface() { return mSurface; }

 private:
    acmacs::surface::Surface& mSurface;
    Tree& mTree;
    // const TreeDraw& mTreeDraw;
    const ChartDrawBase& mChart;
    MappedAntigensDrawSettings& mSettings;

}; // class MappedAntigensDraw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
