#pragma once

#include <string>
#include <limits>
#include <utility>
#include <memory>
#include <algorithm>

#include "acmacs-draw/surface.hh"
#include "legend.hh"
#include "clades-draw.hh"

// ----------------------------------------------------------------------

class Node;
class Tree;
class Coloring;

// ----------------------------------------------------------------------

class AATransitionPerBranchDrawSettings
{
 public:
    AATransitionPerBranchDrawSettings();

    double size;
    Color color;
    TextStyle style;
    double interline;
    Size label_offset;
    double label_connection_line_width;
    Color label_connection_line_color;

}; // class AATransitionPerBranchDrawSettings

// ----------------------------------------------------------------------

class AATransitionDrawSettings
{
 public:
    AATransitionDrawSettings();

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
    TreeDrawVaccineSettings();

    std::string name;           // empty for default settings
    Color label_color;
    double label_size;
    TextStyle label_style;
    Color line_color;
    double line_width;

}; // class TreeDrawVaccineSettings

// ----------------------------------------------------------------------

class TreeDrawMod
{
 public:
    TreeDrawMod(std::string aMod = std::string{});
    TreeDrawMod(std::string aMod, double aD1);
    TreeDrawMod(std::string aMod, std::string aS1, std::string aS2 = std::string{});

    std::string mod;            // root, hide_isolated_before, hide_if_cumulative_edge_length_bigger_than, hide_between, before2015-58P-or-146I-or-559I
    double d1;                  // depends on mod
    std::string s1;             // depends on mod
    std::string s2;             // depends on mod

}; // class TreeDrawMod

// ----------------------------------------------------------------------

class TreeDrawSettings
{
 public:
    TreeDrawSettings();
    ~TreeDrawSettings();

    inline const TreeDrawVaccineSettings& vaccine(std::string aName) const
        {
            auto p = std::find_if(vaccines.begin(), vaccines.end(), [&aName](const auto& e) { return e.name == aName; });
            if (p == vaccines.end())
                p = std::find_if(vaccines.begin(), vaccines.end(), [](const auto& e) { return e.name.empty(); });
            if (p == vaccines.end())
                throw std::runtime_error("Invalid tree.vaccines settings: neither default entry (with empty name) nor entry with name " + aName);
            return *p;
        }

    Tree::LadderizeMethod ladderize;
    std::vector<TreeDrawMod> mods;
    bool force_line_width;
    double line_width;
    double root_edge;
    Color line_color;
    TextStyle label_style;
    double name_offset;         // offset of the label from the line right end, in W widths
    std::string color_nodes;    // black, continent, position number (e.g. 162)
    AATransitionDrawSettings aa_transition;
    std::vector<TreeDrawVaccineSettings> vaccines;
    LegendSettings legend;

      // obsolete: v2
    std::string _root;           // re-root tree
    std::string _hide_isolated_before; // hide leaves isolated before the date (empty -> do not hide based on date)
    double _hide_if_cumulative_edge_length_bigger_than; // hide long branches
    std::string _hide_if;                               // built-in function to hide stains based on complicated criteria

      // for json importer
    inline std::vector<TreeDrawVaccineSettings>& get_vaccines() { return vaccines; }
    inline std::vector<TreeDrawMod>& get_mods() { return mods; }

    inline std::string ladderize_to_string() const
        {
            switch (ladderize) {
              case Tree::LadderizeMethod::NumberOfLeaves:
                  return "number-of-leaves";
              case Tree::LadderizeMethod::MaxEdgeLength:
                  return "max-edge-length";
            }
            return "number-of-leaves";
        }

    inline void ladderize_from_string(const char* str, size_t length)
        {
            const std::string s(str, length);
            if (s == "number-of-leaves")
                ladderize = Tree::LadderizeMethod::NumberOfLeaves;
            else if (s == "max-edge-length")
                ladderize = Tree::LadderizeMethod::MaxEdgeLength;
            else
                throw std::runtime_error("Unrecognized ladderize method: " + s);
        }

}; // class TreeDrawSettings

// ----------------------------------------------------------------------

class HzSection
{
 public:
    HzSection(std::string aName = std::string{}, bool aShowLine = true);
    HzSection(const Node& aFirst, bool aShow, bool aShowLine, bool aShowMap);
    inline HzSection(const HzSection&) = default;
    // inline HzSection(HzSection&&) = default;
    ~HzSection();
    inline HzSection& operator=(const HzSection&) = default;

    bool show;
    bool show_line;
    bool show_label_in_time_series;
    bool show_map;
    std::string name;           // first seq_id
    std::string label;          // antigenic map label, empty - generate automatically

    // not stored in settings
    const Node* first;
    const Node* last;
    std::string index;
};

class HzSections
{
 public:
    HzSections();
    ~HzSections();

    void sort(const Tree& aTree);
    void auto_detect(Tree& aTree, const Clades* aClades);

    inline size_t shown_maps() const
        {
            return std::accumulate(sections.begin(), sections.end(), 0U, [](size_t a, const HzSection& section) -> size_t { return a + (section.show_map ? 1 : 0); });
        }

    double vertical_gap;
    Color line_color;
    double line_width;
    double ts_label_size;
    TextStyle ts_label_style;
    Color ts_label_color;
    std::vector<HzSection> sections;
    bool show_labels_in_time_series_in_tree_mode;

      // for json importer
    inline std::vector<HzSection>& get_sections() { return sections; }
};

// ----------------------------------------------------------------------

class TreeDraw
{
 public:
    TreeDraw(Surface& aSurface, Tree& aTree, TreeDrawSettings& aSettings, HzSections& aHzSections);
    ~TreeDraw();

    void prepare(const LocDb& aLocDb);
    void draw();

    const Legend* coloring_legend() const;
    const Coloring& coloring() const { return *mColoring; }
    double vertical_step() const { return mVerticalStep; }

    void init_settings(const Clades* aClades);
    void set_line_no();
    inline Surface& surface() { return mSurface; }

    static constexpr const size_t sFirstLineNo = 1; // line of the first node is 1, we have 1 line space at the top and bottom of the tree

 private:
    Surface& mSurface;
    Tree& mTree;
    TreeDrawSettings& mSettings;
    HzSections& mHzSections;
    std::unique_ptr<Coloring> mColoring;
    mutable std::unique_ptr<Legend> mColoringLegend;

    double mHorizontalStep;
    double mVerticalStep;
    Scaled mLineWidth;
    Scaled mFontSize;
    double mNameOffset;

    bool apply_mods();          // returns if nodes were hidden
    void set_vertical_pos();
    size_t prepare_hz_sections();
    void draw_node(const Node& aNode, double aOriginX, double& aVerticalGap, double aEdgeLength = -1);
    void draw_legend();
    void draw_aa_transition(const Node& aNode, const Location& aOrigin, double aRight);
    void fit_labels_into_viewport();
    void calculate_name_offset();

    inline double text_width(std::string text) { return mSurface.text_size(text, mFontSize, mSettings.label_style).width; }
    double max_label_offset();

    void make_coloring();

    void hide_isolated_before(std::string aDate);
    void hide_if_cumulative_edge_length_bigger_than(double aThreshold);
    void hide_before2015_58P_or_146I_or_559I();
    void hide_between(std::string aFirst, std::string aLast);
    void hide_one(std::string aName);
    void mark_with_line(std::string aName, Color aColor, Pixels aLineWidth);
    static void hide_branch(Node& aNode);

}; // class TreeDraw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
