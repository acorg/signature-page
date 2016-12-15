#pragma once

#include <string>
#include <limits>

#include "surface.hh"

// ----------------------------------------------------------------------

class Node;
class Tree;

// ----------------------------------------------------------------------

class TreeDrawSettings
{
 public:
    inline TreeDrawSettings()
        : hide_if_cumulative_edge_length_bigger_than(std::numeric_limits<double>::max()), force_line_width(false),
          line_width(1), root_edge(0), line_color(0), name_offset(0.3) {}

    std::string hide_isolated_before; // hide leaves isolated before the date (empty -> do not hide based on date)
    double hide_if_cumulative_edge_length_bigger_than; // hide long branches
    bool force_line_width;
    double line_width;
    double root_edge;
    Color line_color;
    TextStyle label_style;
    double name_offset;         // offset of the label from the line right end, in W widths

}; // class TreeDrawSettings

// ----------------------------------------------------------------------

class TreeDraw
{
 public:
    inline TreeDraw(Surface& aSurface, Tree& aTree, TreeDrawSettings& aSettings)
        : mSurface(aSurface), mTree(aTree), mSettings(aSettings) {}

    void prepare();
    void draw();

 private:
    Surface& mSurface;
    Tree& mTree;
    TreeDrawSettings& mSettings;

    double mHorizontalStep;
    double mVerticalStep;
    double mLineWidth;
      // double mLabelScale;
    double mLabelHeight;
    double mFontSize;
    double mNameOffset;

    void hide_leaves();
    void set_line_no();
    void set_top_bottom();
    void draw_node(const Node& aNode, const Location& aOrigin, double aEdgeLength = -1);
    void set_label_scale();

}; // class TreeDraw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End: