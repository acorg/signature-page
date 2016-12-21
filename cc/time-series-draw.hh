#pragma once

#include <string>
#include <limits>
// #include <utility>
// #include <memory>

#include "surface.hh"

// ----------------------------------------------------------------------

class Node;
class Tree;
class Coloring;
class TreeDraw;

// ----------------------------------------------------------------------

class TimeSeriesDrawSettings
{
 public:
    inline TimeSeriesDrawSettings()
        {}


}; // class TimeSeriesDrawSettings

// ----------------------------------------------------------------------

class TimeSeriesDraw
{
 public:
    inline TimeSeriesDraw(Surface& aSurface, Tree& aTree, const TreeDraw& aTreeDraw, const TimeSeriesDrawSettings& aSettings)
        : mSurface(aSurface), mTree(aTree), mTreeDraw(aTreeDraw), mSettings(aSettings) {}

    void prepare();
    void draw();

 private:
    Surface& mSurface;
    Tree& mTree;
    const TreeDraw& mTreeDraw;
    const TimeSeriesDrawSettings& mSettings;

}; // class TimeSeriesDraw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
