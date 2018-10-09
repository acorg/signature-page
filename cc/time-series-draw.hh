#pragma once

#include <string>

#include "acmacs-draw/surface.hh"
//#include "acmacs-base/date.hh"
#include "rjson-serialize.hh"

// ----------------------------------------------------------------------

class Tree;
class TreeDraw;
class HzSection;
class HzSections;

// ----------------------------------------------------------------------

class TimeSeriesDrawSettings : public acmacs::settings::object
{
 public:
    TimeSeriesDrawSettings(rjson::field_container_parent& aParent, std::string aFieldName);

    acmacs::settings::field<std::string> begin;
    acmacs::settings::field<std::string> end;
    acmacs::settings::field<double> label_size;
    acmacs::settings::field<acmacs::TextStyle> label_style;
    acmacs::settings::field<double> month_year_to_timeseries_gap;
    acmacs::settings::field<Color> month_separator_color;
    acmacs::settings::field<double> month_separator_width;
    acmacs::settings::field<double> dash_width;
    acmacs::settings::field<double> dash_line_width;

}; // class TimeSeriesDrawSettings

// ----------------------------------------------------------------------

class TimeSeriesDraw
{
 public:
    inline TimeSeriesDraw(acmacs::surface::Surface& aSurface, Tree& aTree, const TreeDraw& aTreeDraw, HzSections& aHzSections, TimeSeriesDrawSettings& aSettings)
        : mSurface(aSurface), mTree(aTree), mTreeDraw(aTreeDraw), mSettings(aSettings), mHzSections(aHzSections), mTreeMode(false) {}

    void prepare();
    void draw();

    inline const acmacs::Size& size() const { return mSurface.viewport().size; }
    inline acmacs::Location2D origin_in_parent() const { return mSurface.origin_in_parent(); }

    void init_settings();
    inline acmacs::surface::Surface& surface() { return mSurface; }
    inline void tree_mode(bool aTreeMode) { mTreeMode = aTreeMode; }
    // void hide_hz_section_labels_in_time_series();

 private:
    acmacs::surface::Surface& mSurface;
    Tree& mTree;
    const TreeDraw& mTreeDraw;
    TimeSeriesDrawSettings& mSettings;
    HzSections& mHzSections;
    size_t mNumberOfMonths;
    bool mTreeMode;

    void draw_labels(double month_width);
    void draw_labels_at_side(acmacs::Location2D aOrigin, double month_width, double month_max_height);
    void draw_month_separators(double month_width);
    void draw_dashes(double month_width);
    void draw_hz_section_lines();
    void draw_hz_section_label(size_t aSectionIndex, double aY);

}; // class TimeSeriesDraw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
