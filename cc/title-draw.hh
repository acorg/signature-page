#pragma once

#include <iostream>

#include "acmacs-base/settings.hh"
#include "acmacs-draw/surface.hh"

// ----------------------------------------------------------------------

class Tree;
class ChartDrawBase;
class SettingsInitializer;

// ----------------------------------------------------------------------

class TitleDrawSettings : public acmacs::settings::object
{
 public:
    using acmacs::settings::object::object;

    acmacs::settings::field<std::string>       title{this, "title", ""};
    acmacs::settings::field<Color>             color{this, "color", BLACK};
    acmacs::settings::field<double>            size{this, "size", 12};
    acmacs::settings::field<acmacs::TextStyle> style{this, "style", {}};
    acmacs::settings::field<acmacs::Offset>    offset{this, "offset", {10, 30}};

    void remove_for_tree_settings();

}; // class TitleDrawSettings

// ----------------------------------------------------------------------

class TitleDraw
{
 public:
    inline TitleDraw(acmacs::surface::Surface& aSurface, const Tree& aTree, const ChartDrawBase* aChart, TitleDrawSettings& aSettings)
        : mSurface(aSurface), mTree(aTree), mChart(aChart), mSettings(aSettings) {}

    void init_settings(const SettingsInitializer& settings_initilizer);
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
