#pragma once

#include <iostream>

#include "acmacs-base/settings.hh"
#include "acmacs-draw/surface.hh"

// ----------------------------------------------------------------------

class Tree;
class ChartDrawBase;

// ----------------------------------------------------------------------

class TitleDrawSettings : public acmacs::settings::object
{
 public:
    TitleDrawSettings(rjson::field_container_parent& aParent, std::string aFieldName);

    acmacs::settings::field<std::string> title;
    acmacs::settings::field<Color> color; // Color
    acmacs::settings::field<double> size;
    acmacs::settings::field<acmacs::TextStyle> style;
    acmacs::settings::field<acmacs::Offset> offset;

}; // class TitleDrawSettings

// ----------------------------------------------------------------------

class TitleDraw
{
 public:
    inline TitleDraw(acmacs::surface::Surface& aSurface, const Tree& aTree, const ChartDrawBase* aChart, TitleDrawSettings& aSettings)
        : mSurface(aSurface), mTree(aTree), mChart(aChart), mSettings(aSettings) {}

    void init_settings();
    void prepare();
    void draw();

    inline acmacs::surface::Surface& surface() { return mSurface; }

 private:
    acmacs::surface::Surface& mSurface;
    const Tree& mTree;
    const ChartDrawBase* mChart;
    TitleDrawSettings& mSettings;

}; // class TitleDraw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
