#pragma once

#include <string>

#include "acmacs-base/settings.hh"
#include "acmacs-draw/surface.hh"

// ----------------------------------------------------------------------

class Tree;
class TreeDraw;
class HzSection;
class HzSections;

// ----------------------------------------------------------------------

class TimeSeriesDrawSettings : public acmacs::settings::object
{
 public:
    using acmacs::settings::object::object;

    acmacs::settings::field<std::string>       begin{this, "begin", ""};
    acmacs::settings::field<std::string>       end{this, "end", ""};
    acmacs::settings::field<double>            label_size{this, "label_size", 8};
    acmacs::settings::field<acmacs::TextStyle> label_style{this, "label_style", {}};
    acmacs::settings::field<double>            month_year_to_timeseries_gap{this, "month_year_to_timeseries_gap", 2};
    acmacs::settings::field<Color>             month_separator_color{this, "month_separator_color", BLACK};
    acmacs::settings::field<double>            month_separator_width{this, "month_separator_width", 0.5};
    acmacs::settings::field<double>            dash_width{this, "dash_width", 0.5};
    acmacs::settings::field<double>            dash_line_width{this, "dash_line_width", 1};

    void remove_for_tree_settings();
    
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
