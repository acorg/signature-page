#pragma once

#include <iostream>

#include "acmacs-base/settings.hh"
#include "acmacs-draw/surface.hh"

// ----------------------------------------------------------------------

class Tree;
class TreeDraw;
class ChartDrawBase;

// ----------------------------------------------------------------------

class MappedAntigensDrawSettings : public acmacs::settings::object
{
 public:
    MappedAntigensDrawSettings(rjson::field_container_parent& aParent, std::string aFieldName);

    acmacs::settings::field<double> width;
    acmacs::settings::field<double> line_width;
    acmacs::settings::field<Color> line_color;
    acmacs::settings::field<double> line_length;         // fraction of the surface width

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
