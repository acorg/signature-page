#pragma once

#include <iostream>

#include "acmacs-base/settings-v1.hh"
#include "acmacs-draw/surface.hh"

// ----------------------------------------------------------------------

class Tree;
class ChartDrawBase;
class SettingsInitializer;

// ----------------------------------------------------------------------

class TitleDrawSettings : public acmacs::settings::v1::object
{
 public:
    using acmacs::settings::v1::object::object;

    acmacs::settings::v1::field<std::string>       title{this, "title", ""};
    acmacs::settings::v1::field<Color>             color{this, "color", BLACK};
    acmacs::settings::v1::field<double>            size{this, "size", 12};
    acmacs::settings::v1::field<acmacs::TextStyle> style{this, "style", {}};
    acmacs::settings::v1::field<acmacs::Offset>    offset{this, "offset", {10, 30}};

    void remove_for_tree_settings();

}; // class TitleDrawSettings

// ----------------------------------------------------------------------

class TitleDraw
{
  public:
    TitleDraw(acmacs::surface::Surface& aSurface, TitleDrawSettings& aSettings) : mSurface(aSurface), mSettings(aSettings) {}

    void init_settings(const SettingsInitializer& settings_initilizer);
    void prepare();
    void draw();

    acmacs::surface::Surface& surface() { return mSurface; }

  private:
    acmacs::surface::Surface& mSurface;
    TitleDrawSettings& mSettings;

}; // class TitleDraw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
