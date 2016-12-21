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
class Legend;

// ----------------------------------------------------------------------

class AATransitionPerBranchDrawSettings
{
 public:
    inline AATransitionPerBranchDrawSettings()
        : size(20), color("black"), style("Courier New"), interline(1.2),
          label_offset{-40, 20}, label_connection_line_width(0.1), label_connection_line_color("black") {}

    double size;
    Color color;
    TextStyle style;
    double interline;
      // std::string branch_id;
      // std::vector<std::string> labels;
    Size label_offset;
    double label_connection_line_width;
    Color label_connection_line_color;

}; // class AATransitionPerBranchDrawSettings

// ----------------------------------------------------------------------

class AATransitionDrawSettings
{
 public:
    inline AATransitionDrawSettings()
        : show(true), number_strains_threshold(20), show_empty_left(false), show_node_for_left_line(false),
          node_for_left_line_color("green"), node_for_left_line_width(1)
        {}

    bool show;
    size_t number_strains_threshold; // Do not show aa transition label if number_strains (leaf nodes) for the branch is less than this value.
    bool show_empty_left;
    AATransitionPerBranchDrawSettings per_branch;
    bool show_node_for_left_line;
    Color node_for_left_line_color;
    double node_for_left_line_width;

}; // class AATransitionDrawSettings

// ----------------------------------------------------------------------

class TreeDrawVaccineSettings
{
 public:
    inline TreeDrawVaccineSettings()
        : label_color("black"), label_size(20), line_color("black"), line_width(1) {}

    std::string name;           // empty for default settings
    Color label_color;
    double label_size;
    TextStyle label_style;
    Color line_color;
    double line_width;

}; // class TreeDrawVaccineSettings

// ----------------------------------------------------------------------

class TreeDrawSettings
{
 public:
    inline TreeDrawSettings()
        : hide_if_cumulative_edge_length_bigger_than(0.05), //(std::numeric_limits<double>::max()),
          force_line_width(false),
          line_width(1), root_edge(0), line_color(0), name_offset(0.3), color_nodes("159"),
          vaccines{{TreeDrawVaccineSettings{}}}
        {}

    inline const TreeDrawVaccineSettings& vaccine(std::string aName) const
        {
            auto p = std::find_if(vaccines.begin(), vaccines.end(), [&aName](const auto& e) { return e.name == aName; });
            if (p == vaccines.end())
                p = std::find_if(vaccines.begin(), vaccines.end(), [](const auto& e) { return e.name.empty(); });
            if (p == vaccines.end())
                throw std::runtime_error("Invalid tree.vaccines settings: neither default entry (with empty name) nor entry with name " + aName);
            return *p;
        }

    std::string hide_isolated_before; // hide leaves isolated before the date (empty -> do not hide based on date)
    double hide_if_cumulative_edge_length_bigger_than; // hide long branches
    bool force_line_width;
    double line_width;
    double root_edge;
    Color line_color;
    TextStyle label_style;
    double name_offset;         // offset of the label from the line right end, in W widths
    std::string color_nodes;    // black, continent, position number (e.g. 162)
    AATransitionDrawSettings aa_transition;
    std::vector<TreeDrawVaccineSettings> vaccines;

}; // class TreeDrawSettings

// ----------------------------------------------------------------------

class TreeDraw
{
 public:
    TreeDraw(Surface& aSurface, Tree& aTree, TreeDrawSettings& aSettings);
    ~TreeDraw();

    void prepare();
    void draw();
    Legend* coloring_legend() const; // new instance returned, caller must free it

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
    void draw_aa_transition(const Node& aNode, const Location& aOrigin, double aRight);
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
