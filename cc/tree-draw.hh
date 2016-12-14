#pragma once

#include <string>
#include <limits>

// ----------------------------------------------------------------------

class Tree;
class Surface;

// ----------------------------------------------------------------------

class TreeDrawSettings
{
 public:
    inline TreeDrawSettings()
        : hide_if_cumulative_edge_length_bigger_than(std::numeric_limits<double>::max()) {}

    std::string hide_isolated_before; // hide leaves isolated before the date (empty -> do not hide based on date)
    double hide_if_cumulative_edge_length_bigger_than; // hide long branches

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

    void hide_leaves();
    void set_line_no();
    void set_top_bottom();

}; // class TreeDraw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
