#pragma once

#include <iostream>

#include "acmacs-base/settings-v1.hh"
#include "acmacs-draw/surface.hh"

// ----------------------------------------------------------------------

class Tree;
class TreeDraw;
class ChartDrawBase;

// ----------------------------------------------------------------------

class MappedAntigensDrawSettings : public acmacs::settings::v1::object
{
 public:
    using acmacs::settings::v1::object::object;

    acmacs::settings::v1::field<double> width{this, "width", 10};
    acmacs::settings::v1::field<double> line_width{this, "line_width", 0.5};
    acmacs::settings::v1::field<Color>  line_color{this, "line_color", "grey56"};
    acmacs::settings::v1::field<double> line_length{this, "line_length", 0.5};         // fraction of the surface width

}; // class MappedAntigensDrawSettings

// ----------------------------------------------------------------------

class MappedAntigensDraw
{
 public:
    MappedAntigensDraw(acmacs::surface::Surface& aSurface, Tree& aTree, /* const TreeDraw& aTreeDraw, */ const ChartDrawBase& aChart, MappedAntigensDrawSettings& aSettings)
        : mSurface(aSurface), mTree(aTree), /* mTreeDraw(aTreeDraw), */ mChart(aChart), mSettings(aSettings) {}

    void prepare();
    void draw();

    acmacs::surface::Surface& surface() { return mSurface; }

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
