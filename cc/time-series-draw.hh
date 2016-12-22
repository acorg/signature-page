#pragma once

#include <string>
#include <limits>
// #include <utility>
// #include <memory>

#include "surface.hh"
#include "acmacs-base/date.hh"

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
        : label_size(50), month_year_to_timeseries_gap(20), month_separator_color("black"),
          month_separator_width(1), dash_width(0.5), dash_line_width(5)
        {}

    std::string begin, end;
    double label_size;
    TextStyle label_style;
    double month_year_to_timeseries_gap;
    Color month_separator_color;
    double month_separator_width;
    double dash_width;
    double dash_line_width;

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
    Date mBegin, mEnd;
    size_t mNumberOfMonths;

    void draw_labels(double month_width);
    void draw_labels_at_side(const Location& aOrigin, double month_width, double month_max_height);
    void draw_month_separators(double month_width);
    void draw_dashes(double month_width);

}; // class TimeSeriesDraw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
