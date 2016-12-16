#pragma once

#include <string>
#include <limits>
#include <utility>
#include <memory>

#include "surface.hh"

// ----------------------------------------------------------------------

class Node;
class Tree;
class Coloring;

// ----------------------------------------------------------------------

class TreeDrawSettings
{
 public:
    inline TreeDrawSettings()
        : hide_if_cumulative_edge_length_bigger_than(std::numeric_limits<double>::max()), force_line_width(false),
          line_width(1), root_edge(0), line_color(0), name_offset(0.3), color_nodes("172") {}

    std::string hide_isolated_before; // hide leaves isolated before the date (empty -> do not hide based on date)
    double hide_if_cumulative_edge_length_bigger_than; // hide long branches
    bool force_line_width;
    double line_width;
    double root_edge;
    Color line_color;
    TextStyle label_style;
    double name_offset;         // offset of the label from the line right end, in W widths
    std::string color_nodes;    // black, continent, position number (e.g. 162)

}; // class TreeDrawSettings

// ----------------------------------------------------------------------

class TreeDraw
{
 public:
    TreeDraw(Surface& aSurface, Tree& aTree, TreeDrawSettings& aSettings);
    ~TreeDraw();

    void prepare();
    void draw();

 private:
    Surface& mSurface;
    Tree& mTree;
    TreeDrawSettings& mSettings;
    std::unique_ptr<Coloring> mColoring;

    double mHorizontalStep;
    double mVerticalStep;
    double mLineWidth;
    double mFontSize;
      // double mLabelHeight;
    double mNameOffset;

    void hide_leaves();
    void set_line_no();
    void set_top_bottom();
    void draw_node(const Node& aNode, const Location& aOrigin, double aEdgeLength = -1);
    void fit_labels_into_viewport();
    void calculate_name_offset();

    inline double text_width(std::string text) { return mSurface.text_size(text, mFontSize, mSettings.label_style).width; }
    double max_label_offset();

    void make_coloring();

}; // class TreeDraw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
