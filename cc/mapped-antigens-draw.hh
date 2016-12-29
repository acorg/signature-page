#pragma once

#include <iostream>

#include "surface.hh"

// ----------------------------------------------------------------------

class Tree;
class TreeDraw;
class Chart;

// ----------------------------------------------------------------------

class MappedAntigensDrawSettings
{
 public:
    inline MappedAntigensDrawSettings() : width(15), line_width(1), line_color("red"), line_length(0.5) {}
    ~MappedAntigensDrawSettings();

    double width;
    double line_width;
    Color line_color;
    double line_length;         // fraction of the surface width

}; // class MappedAntigensDrawSettings

// ----------------------------------------------------------------------

class MappedAntigensDraw
{
 public:
    inline MappedAntigensDraw(Surface& aSurface, Tree& aTree, const TreeDraw& aTreeDraw, const Chart& aChart, MappedAntigensDrawSettings& aSettings)
        : mSurface(aSurface), mTree(aTree), mTreeDraw(aTreeDraw), mChart(aChart), mSettings(aSettings) {}

    void prepare();
    void draw();

    inline Surface& surface() { return mSurface; }

 private:
    Surface& mSurface;
    Tree& mTree;
    const TreeDraw& mTreeDraw;
    const Chart& mChart;
    MappedAntigensDrawSettings& mSettings;

}; // class MappedAntigensDraw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
