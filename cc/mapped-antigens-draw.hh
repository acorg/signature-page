#pragma once

#include <iostream>

#include "acmacs-draw/surface.hh"

// ----------------------------------------------------------------------

class Tree;
class TreeDraw;
namespace sdb {
    class Chart;
}

// ----------------------------------------------------------------------

class MappedAntigensDrawSettings
{
 public:
    MappedAntigensDrawSettings();
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
    inline MappedAntigensDraw(Surface& aSurface, Tree& aTree, /* const TreeDraw& aTreeDraw, */ const sdb::Chart& aChart, MappedAntigensDrawSettings& aSettings)
        : mSurface(aSurface), mTree(aTree), /* mTreeDraw(aTreeDraw), */ mChart(aChart), mSettings(aSettings) {}

    void prepare();
    void draw();

    inline Surface& surface() { return mSurface; }

 private:
    Surface& mSurface;
    Tree& mTree;
    // const TreeDraw& mTreeDraw;
    const sdb::Chart& mChart;
    MappedAntigensDrawSettings& mSettings;

}; // class MappedAntigensDraw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
