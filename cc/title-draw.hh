#pragma once

#include <iostream>

#include "acmacs-draw/surface.hh"
#include "rjson-serialize.hh"

// ----------------------------------------------------------------------

class Tree;
class ChartDrawBase;

// ----------------------------------------------------------------------

class TitleDrawSettings : public rjson::field_container_child
{
 public:
    TitleDrawSettings(rjson::field_container_parent& aParent, std::string aFieldName);

    rjson::field_get_set<std::string> title;
    rjson::field_get_set<Color> color; // Color
    rjson::field_get_set<double> size;
    TextStyle style;
    rjson::field_get_set<Size> offset;

}; // class TitleDrawSettings

// ----------------------------------------------------------------------

class TitleDraw
{
 public:
    inline TitleDraw(Surface& aSurface, const Tree& aTree, const ChartDrawBase* aChart, TitleDrawSettings& aSettings)
        : mSurface(aSurface), mTree(aTree), mChart(aChart), mSettings(aSettings) {}

    void init_settings();
    void prepare();
    void draw();

    inline Surface& surface() { return mSurface; }

 private:
    Surface& mSurface;
    const Tree& mTree;
    const ChartDrawBase* mChart;
    TitleDrawSettings& mSettings;

}; // class TitleDraw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
