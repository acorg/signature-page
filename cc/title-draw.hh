#pragma once

#include <iostream>

#include "acmacs-draw/surface.hh"

// ----------------------------------------------------------------------

class Tree;
class Chart;

// ----------------------------------------------------------------------

class TitleDrawSettings
{
 public:
    TitleDrawSettings();
    ~TitleDrawSettings();

    std::string title;
    Color color;
    double size;
    TextStyle style;
    Size offset;

}; // class TitleDrawSettings

// ----------------------------------------------------------------------

class TitleDraw
{
 public:
    inline TitleDraw(Surface& aSurface, const Tree& aTree, const Chart* aChart, TitleDrawSettings& aSettings)
        : mSurface(aSurface), mTree(aTree), mChart(aChart), mSettings(aSettings) {}

    void init_settings();
    void prepare();
    void draw();

    inline Surface& surface() { return mSurface; }

 private:
    Surface& mSurface;
    const Tree& mTree;
    const Chart* mChart;
    TitleDrawSettings& mSettings;

}; // class TitleDraw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
