#pragma once

#include <iostream>

#include "surface.hh"

// ----------------------------------------------------------------------

class Tree;
class TreeDraw;
class Chart;

// ----------------------------------------------------------------------

class TitleDrawSettings
{
 public:
    inline TitleDrawSettings() {}
    ~TitleDrawSettings();

}; // class TitleDrawSettings

// ----------------------------------------------------------------------

class TitleDraw
{
 public:
    inline TitleDraw(Surface& aSurface, TitleDrawSettings& aSettings)
        : mSurface(aSurface), mSettings(aSettings) {}

    void init_settings();
    void prepare();
    void draw();

    inline Surface& surface() { return mSurface; }

 private:
    Surface& mSurface;
    TitleDrawSettings& mSettings;

}; // class TitleDraw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
