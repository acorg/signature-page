#pragma once

#include <string>
#include <limits>
#include <utility>
#include <memory>
#include <algorithm>

#include "acmacs-draw/surface.hh"
#include "legend.hh"
#include "clades-draw.hh"
#include "rjson-serialize.hh"

// ----------------------------------------------------------------------

class Node;
class Tree;
class Coloring;

// ----------------------------------------------------------------------

class AATransitionIndividualSettings : public rjson::array_field_container_child_element
{
 public:
    AATransitionIndividualSettings(const rjson::value& aData);

    inline void set_label_disabled_offset(std::string aLabel, std::string aFirstLeafSeqid, acmacs::Size&& aLabelOffset)
        {
            label = aLabel;
            first_leaf_seq_id = aFirstLeafSeqid;
            add("?label_offset", std::move(aLabelOffset));
        }

    rjson::field_get_set<std::string> label;
    rjson::field_get_set<std::string> first_leaf_seq_id;
    rjson::field_get_set<bool> show;
    rjson::field_get_set<double> size;
    rjson::field_get_set<Color> color;
    rjson::field_get_set<acmacs::TextStyle> style;
    rjson::field_get_set<double> interline;
    rjson::field_get_set<acmacs::Size> label_offset;
    rjson::field_get_set<double> label_connection_line_width;
    rjson::field_get_set<Color> label_connection_line_color;
    rjson::field_get_set<bool> _no_pp; // hidden field to avoid pretty printing this rjson object

}; // class AATransitionIndividualSettings

// ----------------------------------------------------------------------

class AATransitionPerBranchDrawSettings : public rjson::field_container_child
{
 public:
    AATransitionPerBranchDrawSettings(rjson::field_container_parent& aParent, std::string aFieldName);

    rjson::field_get_set<bool> show;
    rjson::field_get_set<double> size;
    rjson::field_get_set<Color> color;
    rjson::field_get_set<acmacs::TextStyle> style;
    rjson::field_get_set<double> interline;
    rjson::field_get_set<acmacs::Size> label_offset;
    rjson::field_get_set<double> scatter_label_offset;
    rjson::field_get_set<std::string> scatter_label_offset_help;
    rjson::field_get_set<double> label_connection_line_width;
    rjson::field_get_set<Color> label_connection_line_color;
    rjson::array_field_container_child<AATransitionIndividualSettings> by_aa_label;

      // returns ref to static object overriden with each call
    const AATransitionIndividualSettings& settings_for_label(const AA_TransitionLabels& aLabels, std::string aFirstLeafSeqid) const;
    void remove_for_signature_page_settings();

}; // class AATransitionPerBranchDrawSettings

// ----------------------------------------------------------------------

class AATransitionDrawSettings : public rjson::field_container_child
{
 public:
    AATransitionDrawSettings(rjson::field_container_parent& aParent, std::string aFieldName);

    rjson::field_get_set<bool> show;
    rjson::field_get_set<unsigned> number_strains_threshold; // Do not show aa transition label if number_strains (leaf nodes) for the branch is less than this value.
    rjson::field_get_set<std::string> number_strains_threshold_help;
    rjson::field_get_set<bool> show_empty_left;
    AATransitionPerBranchDrawSettings per_branch;
    rjson::field_get_set<bool> show_node_for_left_line;
    rjson::field_get_set<Color> node_for_left_line_color;
    rjson::field_get_set<double> node_for_left_line_width;

    void remove_for_signature_page_settings();

}; // class AATransitionDrawSettings

// ----------------------------------------------------------------------

class TreeDrawVaccineSettings : public rjson::array_field_container_child_element
{
 public:
    TreeDrawVaccineSettings(const rjson::value& aData);

    rjson::field_get_set<std::string> name;           // empty for default settings
    rjson::field_get_set<std::string> name_help;
    rjson::field_get_set<Color> label_color;
    rjson::field_get_set<double> label_size;
    rjson::field_get_set<acmacs::TextStyle> label_style;
    rjson::field_get_set<Color> line_color;
    rjson::field_get_set<double> line_width;

}; // class TreeDrawVaccineSettings

class TreeDrawVaccines : public rjson::array_field_container_child<TreeDrawVaccineSettings>
{
 public:
    using rjson::array_field_container_child<TreeDrawVaccineSettings>::array_field_container_child;
};

// ----------------------------------------------------------------------

class TreeDrawMod : public rjson::array_field_container_child_element
{
 public:
    inline TreeDrawMod(const rjson::value& aData)
        : rjson::array_field_container_child_element(aData),
          mod(*this, "mod", std::string{}),
          d1(*this,  "d1", -1.0),
          s1(*this,  "s1", std::string{}),
          s2(*this,  "s2", std::string{}),
          clade(*this,  "clade", std::string{}),
          color(*this,  "color", std::string{}),
          line_width(*this,  "line_width", -1.0),
          seq_id(*this, "seq_id", std::string{}),
          label(*this, "label", std::string{}),
          label_color(*this, "label_color", "black"),
          label_size(*this, "label_size", 10.0),
          line_color(*this, "line_color", "black"),
          label_style(*this, "label_style", {}),
          label_offset(*this, "label_offset", {20, 20})
        {}

    rjson::field_get_set<std::string> mod;            // root, hide_isolated_before, hide_if_cumulative_edge_length_bigger_than, hide_between, before2015-58P-or-146I-or-559I
    rjson::field_get_set<double> d1;                  // depends on mod
    rjson::field_get_set<std::string> s1;             // depends on mod
    rjson::field_get_set<std::string> s2;             // depends on mod
    rjson::field_get_set<std::string> clade;          // mark-clade-with-line mod
    rjson::field_get_set<std::string> color;          // mark-clade-with-line mod
    rjson::field_get_set<double> line_width;          // in pixels, mark-clade-with-line, mark-with-label

    rjson::field_get_set<std::string> seq_id;      // mark-with-label
    rjson::field_get_set<std::string> label;       // mark-with-label
    rjson::field_get_set<std::string> label_color; // mark-with-label
    rjson::field_get_set<double> label_size;       // mark-with-label
    rjson::field_get_set<std::string> line_color;  // mark-with-label
    rjson::field_get_set<acmacs::TextStyle> label_style;   // mark-with-label
    rjson::field_get_set<acmacs::Size> label_offset;   // mark-with-label

}; // class TreeDrawMod

class TreeDrawMods : public rjson::array_field_container_child<TreeDrawMod>
{
 public:
    using rjson::array_field_container_child<TreeDrawMod>::array_field_container_child;

    inline const TreeDrawMod find_mark_with_label(std::string aSeqId) const // not reference returned, TreeDrawMod is a proxy
        {
            auto p = std::find_if(begin(), end(), [&](const auto& e) { return e.mod == "mark-with-label" && e.seq_id == aSeqId; });
            if (p == end())
                throw std::runtime_error("Invalid tree.mods settings: cannot find mark-with-label for " + aSeqId);
            return *p;
        }

}; // class TreeDrawMods

// ----------------------------------------------------------------------

// serializing Tree::LadderizeMethod from tree.hh
namespace rjson
{
    template <> struct content_type<Tree::LadderizeMethod> { using type = string; };

    template <> inline field_get_set<Tree::LadderizeMethod>::operator Tree::LadderizeMethod() const
    {
        try {
            auto method_s = static_cast<std::string_view>(get_value_ref());
            if (method_s == "number-of-leaves")
                return Tree::LadderizeMethod::NumberOfLeaves;
            else if (method_s == "max-edge-length")
                return Tree::LadderizeMethod::MaxEdgeLength;
            else
                throw std::exception{}; // std::runtime_error("Unrecognized ladderize method: " + method_s);
        }
        catch (std::exception& /*err*/) {
            std::cerr << "ERROR: cannot convert json to Tree::LadderizeMethod: " << get_value_ref() << '\n';
            return {};
        }
    }

    template <> inline value to_value<Tree::LadderizeMethod>(const Tree::LadderizeMethod& aLadderizeMethod)
    {
            switch (aLadderizeMethod) {
              case Tree::LadderizeMethod::NumberOfLeaves:
                  return string{"number-of-leaves"};
              case Tree::LadderizeMethod::MaxEdgeLength:
                  return string{"max-edge-length"};
            }
            return string{"number-of-leaves"};
    }

    template <> inline value to_value<Tree::LadderizeMethod>(Tree::LadderizeMethod&& aLadderizeMethod)
    {
        return to_value<Tree::LadderizeMethod>(const_cast<const Tree::LadderizeMethod&>(aLadderizeMethod));
    }
}

class TreeDrawSettings : public rjson::field_container_child
{
 public:
    TreeDrawSettings(rjson::field_container_parent& aParent, std::string aFieldName);

    rjson::field_get_set<Tree::LadderizeMethod> ladderize;
    rjson::field_get_set<std::string> ladderize_help;
    TreeDrawMods mods;
    rjson::field_get_set<std::vector<std::string>> mods_help;
    rjson::field_get_set<bool> force_line_width;
    rjson::field_get_set<double> line_width;
    rjson::field_get_set<double> root_edge;
    rjson::field_get_set<Color> line_color;
    rjson::field_get_set<acmacs::TextStyle> label_style;
    rjson::field_get_set<double> name_offset;         // offset of the label from the line right end, in W widths
    rjson::field_get_set<std::string> color_nodes;    // black, continent, position number (e.g. 162)
    AATransitionDrawSettings aa_transition;
    LegendSettings legend;

    void remove_for_signature_page_settings();

}; // class TreeDrawSettings

// ----------------------------------------------------------------------

class HzSection : public rjson::array_field_container_child_element
{
 public:
    HzSection(const rjson::value& aData);
    // HzSection(std::string aName = std::string{}, bool aShowLine = true);
    // HzSection(const Node& aFirst, bool aShow, bool aShowLine, bool aShowMap);
      // inline HzSection(const HzSection&) = default;
    // inline HzSection(HzSection&&) = default;
    // inline HzSection& operator=(const HzSection&) = default;

    rjson::field_get_set<bool> show;
    rjson::field_get_set<bool> show_line;
    rjson::field_get_set<bool> show_label_in_time_series;
    rjson::field_get_set<bool> show_map;
    rjson::field_get_set<std::string> name;           // first seq_id
    rjson::field_get_set<std::string> label;          // antigenic map label, empty - generate automatically

    // not stored in settings
    // const Node* first;
    // const Node* last;
    // std::string index;
};

class HzSections : public rjson::field_container_child
{
 public:
    HzSections(rjson::field_container_parent& aParent, std::string aFieldName);

    rjson::field_get_set<double> vertical_gap;
    rjson::field_get_set<Color> line_color;
    rjson::field_get_set<double> line_width;
    rjson::field_get_set<double> ts_label_size;
    rjson::field_get_set<acmacs::TextStyle> ts_label_style;
    rjson::field_get_set<Color> ts_label_color;
    rjson::array_field_container_child<HzSection> sections;
    rjson::field_get_set<bool> show_labels_in_time_series_in_tree_mode;

      // not stored

    struct NodeRef
    {
        inline NodeRef() = default;
        inline NodeRef(const Node* aFirst) : first{aFirst} {}
        const Node* first = nullptr;
        const Node* last = nullptr;
        std::string index;
    };
    std::vector<NodeRef> node_refs;
    std::vector<size_t> section_order;

    void sort(const Tree& aTree);
    // void auto_detect(Tree& aTree, const Clades* aClades);
    void detect_hz_lines_for_clades(Tree& aTree, const Clades* aClades, bool aForce);

    inline size_t shown_maps() const
        {
            return std::accumulate(sections.begin(), sections.end(), 0U, [](size_t a, const HzSection& section) -> size_t { return a + (section.show_map ? 1 : 0); });
        }

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
    void ladderize();
    void detect_hz_lines_for_clades(const Clades* aClades, bool aForce = true);
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
    bool mInitializeSettings = false;

    bool apply_mods();          // returns if nodes were hidden
    void set_vertical_pos();
    size_t prepare_hz_sections();
    void draw_node(const Node& aNode, double aOriginX, double& aVerticalGap, double aEdgeLength = -1);
    void draw_legend();
    void draw_aa_transition(const Node& aNode, const acmacs::Location& aOrigin, double aRight);
    void draw_mark_with_label(const Node& aNode, const acmacs::Location& aTextOrigin);

    void fit_labels_into_viewport();
    void calculate_name_offset();

    inline double text_width(std::string text) { return mSurface.text_size(text, mFontSize, mSettings.label_style).width; }
    double max_label_offset();

    void make_coloring();

    // void unhide();
    void hide_isolated_before(std::string aDate);
    void hide_if_cumulative_edge_length_bigger_than(double aThreshold);
    void hide_before2015_58P_or_146I_or_559I();
    void hide_between(std::string aFirst, std::string aLast);
    void hide_one(std::string aName);
    void mark_with_line(std::string aName, Color aColor, Pixels aLineWidth);
    void mark_clade_with_line(std::string aClade, Color aColor, Pixels aLineWidth);
    void mark_with_label(const TreeDrawMod& aMod);
    static void hide_branch(Node& aNode);

}; // class TreeDraw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
