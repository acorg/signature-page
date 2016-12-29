#pragma once

#include <iostream>

#include "surface.hh"

// ----------------------------------------------------------------------

class Tree;
class TreeDraw;
class Chart;
class HzSections;

// ----------------------------------------------------------------------

class AntigenicMapsDrawSettings
{
 public:
    inline AntigenicMapsDrawSettings() : width(500), columns(3), border_width(1), border_color("black"), gap(20) {}
    ~AntigenicMapsDrawSettings();

    double width;
    size_t columns;
    double border_width;
    Color border_color;
    double gap;

}; // class AntigenicMapsDrawSettings

// ----------------------------------------------------------------------

class AntigenicMapsDraw
{
 public:
    inline AntigenicMapsDraw(Surface& aSurface, Tree& aTree, const Chart& aChart, AntigenicMapsDrawSettings& aSettings)
        : mSurface(aSurface), mTree(aTree), mChart(aChart), mSettings(aSettings) {}

    void init_settings(const HzSections& aHzSections);
    void prepare();
    void draw();

    inline Surface& surface() { return mSurface; }

 private:
    Surface& mSurface;
    Tree& mTree;
    const Chart& mChart;
    AntigenicMapsDrawSettings& mSettings;

}; // class AntigenicMapsDraw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
