#pragma once

#include <string>

#include "surface.hh"
#include "acmacs-base/date.hh"

// ----------------------------------------------------------------------

class Tree;
class TreeDraw;
class HzSections;

// ----------------------------------------------------------------------

class TimeSeriesDrawSettings
{
 public:
    inline TimeSeriesDrawSettings()
        : label_size(40), month_year_to_timeseries_gap(10), month_separator_color("black"),
          month_separator_width(1), dash_width(0.5), dash_line_width(5)
        {}
    ~TimeSeriesDrawSettings();

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
    inline TimeSeriesDraw(Surface& aSurface, Tree& aTree, const TreeDraw& aTreeDraw, const HzSections& aHzSections, TimeSeriesDrawSettings& aSettings)
        : mSurface(aSurface), mTree(aTree), mTreeDraw(aTreeDraw), mSettings(aSettings), mHzSections(aHzSections) {}

    void prepare();
    void draw();

    inline const Size& size() const { return mSurface.size(); }
    inline const Size& offset() const { return mSurface.offset(); }

    void init_settings();
    Surface& surface() { return mSurface; }

 private:
    Surface& mSurface;
    Tree& mTree;
    const TreeDraw& mTreeDraw;
    TimeSeriesDrawSettings& mSettings;
    const HzSections& mHzSections;
    Date mBegin, mEnd;
    size_t mNumberOfMonths;

    void draw_labels(double month_width);
    void draw_labels_at_side(const Location& aOrigin, double month_width, double month_max_height);
    void draw_month_separators(double month_width);
    void draw_dashes(double month_width);
    void draw_hz_section_lines();

}; // class TimeSeriesDraw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
