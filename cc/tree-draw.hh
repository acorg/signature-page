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

class AATransitionIndividualSettings : public rjson::v1::array_field_container_child_element
{
 public:
    AATransitionIndividualSettings(const rjson::v1::value& aData);

    inline void set_label_disabled_offset(std::string aLabel, std::string aFirstLeafSeqid, acmacs::Offset&& aLabelOffset)
        {
            label = aLabel;
            first_leaf_seq_id = aFirstLeafSeqid;
            add("?label_offset", std::move(aLabelOffset));
        }

    rjson::v1::field_get_set<std::string> label;
    rjson::v1::field_get_set<std::string> first_leaf_seq_id;
    rjson::v1::field_get_set<bool> show;
    rjson::v1::field_get_set<double> size;
    rjson::v1::field_get_set<Color> color;
    rjson::v1::field_get_set<acmacs::TextStyle> style;
    rjson::v1::field_get_set<double> interline;
    rjson::v1::field_get_set<acmacs::Offset> label_offset;
    rjson::v1::field_get_set<double> label_connection_line_width;
    rjson::v1::field_get_set<Color> label_connection_line_color;
    rjson::v1::field_get_set<bool> _no_pp; // hidden field to avoid pretty printing this rjson object

}; // class AATransitionIndividualSettings

// ----------------------------------------------------------------------

class AATransitionPerBranchDrawSettings : public rjson::v1::field_container_child
{
 public:
    AATransitionPerBranchDrawSettings(rjson::v1::field_container_parent& aParent, std::string aFieldName);

    rjson::v1::field_get_set<bool> show;
    rjson::v1::field_get_set<double> size;
    rjson::v1::field_get_set<Color> color;
    rjson::v1::field_get_set<acmacs::TextStyle> style;
    rjson::v1::field_get_set<double> interline;
    rjson::v1::field_get_set<acmacs::Offset> label_offset;
    rjson::v1::field_get_set<double> scatter_label_offset;
    rjson::v1::field_get_set<std::string> scatter_label_offset_help;
    rjson::v1::field_get_set<double> label_connection_line_width;
    rjson::v1::field_get_set<Color> label_connection_line_color;
    rjson::v1::array_field_container_child<AATransitionIndividualSettings> by_aa_label;

      // returns ref to static object overriden with each call
    const AATransitionIndividualSettings& settings_for_label(const AA_TransitionLabels& aLabels, std::string aFirstLeafSeqid) const;
    void remove_for_signature_page_settings();

}; // class AATransitionPerBranchDrawSettings

// ----------------------------------------------------------------------

class AATransitionDrawSettings : public rjson::v1::field_container_child
{
 public:
    AATransitionDrawSettings(rjson::v1::field_container_parent& aParent, std::string aFieldName);

    rjson::v1::field_get_set<bool> show;
    rjson::v1::field_get_set<unsigned> number_strains_threshold; // Do not show aa transition label if number_strains (leaf nodes) for the branch is less than this value.
    rjson::v1::field_get_set<std::string> number_strains_threshold_help;
    rjson::v1::field_get_set<bool> show_empty_left;
    AATransitionPerBranchDrawSettings per_branch;
    rjson::v1::field_get_set<bool> show_node_for_left_line;
    rjson::v1::field_get_set<Color> node_for_left_line_color;
    rjson::v1::field_get_set<double> node_for_left_line_width;

    void remove_for_signature_page_settings();

}; // class AATransitionDrawSettings

// ----------------------------------------------------------------------

class TreeDrawVaccineSettings : public rjson::v1::array_field_container_child_element
{
 public:
    TreeDrawVaccineSettings(const rjson::v1::value& aData);

    rjson::v1::field_get_set<std::string> name;           // empty for default settings
    rjson::v1::field_get_set<std::string> name_help;
    rjson::v1::field_get_set<Color> label_color;
    rjson::v1::field_get_set<double> label_size;
    rjson::v1::field_get_set<acmacs::TextStyle> label_style;
    rjson::v1::field_get_set<Color> line_color;
    rjson::v1::field_get_set<double> line_width;

}; // class TreeDrawVaccineSettings

class TreeDrawVaccines : public rjson::v1::array_field_container_child<TreeDrawVaccineSettings>
{
 public:
    using rjson::v1::array_field_container_child<TreeDrawVaccineSettings>::array_field_container_child;
};

// ----------------------------------------------------------------------

class TreeDrawMod : public rjson::v1::array_field_container_child_element
{
 public:
    inline TreeDrawMod(const rjson::v1::value& aData)
        : rjson::v1::array_field_container_child_element(aData),
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

    rjson::v1::field_get_set<std::string> mod;            // root, hide_isolated_before, hide_if_cumulative_edge_length_bigger_than, hide_between, before2015-58P-or-146I-or-559I
    rjson::v1::field_get_set<double> d1;                  // depends on mod
    rjson::v1::field_get_set<std::string> s1;             // depends on mod
    rjson::v1::field_get_set<std::string> s2;             // depends on mod
    rjson::v1::field_get_set<std::string> clade;          // mark-clade-with-line mod
    rjson::v1::field_get_set<std::string> color;          // mark-clade-with-line mod
    rjson::v1::field_get_set<double> line_width;          // in pixels, mark-clade-with-line, mark-with-label

    rjson::v1::field_get_set<std::string> seq_id;      // mark-with-label
    rjson::v1::field_get_set<std::string> label;       // mark-with-label
    rjson::v1::field_get_set<std::string> label_color; // mark-with-label
    rjson::v1::field_get_set<double> label_size;       // mark-with-label
    rjson::v1::field_get_set<std::string> line_color;  // mark-with-label
    rjson::v1::field_get_set<acmacs::TextStyle> label_style;   // mark-with-label
    rjson::v1::field_get_set<acmacs::Offset> label_offset;   // mark-with-label

}; // class TreeDrawMod

class TreeDrawMods : public rjson::v1::array_field_container_child<TreeDrawMod>
{
 public:
    using rjson::v1::array_field_container_child<TreeDrawMod>::array_field_container_child;

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
    namespace v1
    {
        template <> struct content_type<Tree::LadderizeMethod>
        {
            using type = string;
        };

        template <> inline field_get_set<Tree::LadderizeMethod>::operator Tree::LadderizeMethod() const
        {
            try {
                const std::string_view method_s = get_value_ref();
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

    } // namespace v1
} // namespace rjson

class TreeDrawSettings : public rjson::v1::field_container_child
{
 public:
    TreeDrawSettings(rjson::v1::field_container_parent& aParent, std::string aFieldName);

    rjson::v1::field_get_set<Tree::LadderizeMethod> ladderize;
    rjson::v1::field_get_set<std::string> ladderize_help;
    TreeDrawMods mods;
    rjson::v1::field_get_set<std::vector<std::string>> mods_help;
    rjson::v1::field_get_set<bool> force_line_width;
    rjson::v1::field_get_set<double> line_width;
    rjson::v1::field_get_set<double> root_edge;
    rjson::v1::field_get_set<Color> line_color;
    rjson::v1::field_get_set<acmacs::TextStyle> label_style;
    rjson::v1::field_get_set<double> name_offset;         // offset of the label from the line right end, in W widths
    rjson::v1::field_get_set<std::string> color_nodes;    // black, continent, position number (e.g. 162)
    rjson::v1::field_get_set<std::map<std::string, std::string>> color_for_aa;    // for "color_nodes": "<position-number>"
    rjson::v1::field_get_set<double> right_padding;       // padding at the right to add space for the mark_with_line (for BVic del and triple-del mutants)
    AATransitionDrawSettings aa_transition;
    LegendSettings legend;

    void remove_for_signature_page_settings();

}; // class TreeDrawSettings

// ----------------------------------------------------------------------

class HzSection : public rjson::v1::array_field_container_child_element
{
 public:
    HzSection(const rjson::v1::value& aData);
    // HzSection(std::string aName = std::string{}, bool aShowLine = true);
    // HzSection(const Node& aFirst, bool aShow, bool aShowLine, bool aShowMap);
      // inline HzSection(const HzSection&) = default;
    // inline HzSection(HzSection&&) = default;
    // inline HzSection& operator=(const HzSection&) = default;

    rjson::v1::field_get_set<bool> show;
    rjson::v1::field_get_set<bool> show_line;
    rjson::v1::field_get_set<bool> show_label_in_time_series;
    rjson::v1::field_get_set<bool> show_map;
    rjson::v1::field_get_set<std::string> name;           // first seq_id
    rjson::v1::field_get_set<std::string> label;          // antigenic map label, empty - generate automatically

    rjson::v1::array_field_container_child<std::string> triggering_clades; // clades that trigger this hz line
    rjson::v1::array_field_container_child<size_t> triggering_aa_pos;      // aa pos triggering this line

      // not stored in settings
    // const Node* first;
    // const Node* last;
    // std::string index;
};

class HzSections : public rjson::v1::field_container_child
{
 public:
    HzSections(rjson::v1::field_container_parent& aParent, std::string aFieldName);

    rjson::v1::field_get_set<double> vertical_gap;
    rjson::v1::field_get_set<Color> line_color;
    rjson::v1::field_get_set<double> line_width;
    rjson::v1::field_get_set<double> ts_label_size;
    rjson::v1::field_get_set<acmacs::TextStyle> ts_label_style;
    rjson::v1::field_get_set<Color> ts_label_color;
    rjson::v1::array_field_container_child<HzSection> sections;
    rjson::v1::field_get_set<bool> show_labels_in_time_series_in_tree_mode;

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
    bool show = true;

    void sort(const Tree& aTree);
    void report(std::ostream& out) const;
    void report_html(std::string filename) const;
    // void auto_detect(Tree& aTree, const Clades* aClades);
    void detect_hz_lines_for_clades(Tree& aTree, const Clades* aClades, bool aForce);
    void add(std::string seq_id, bool show_line, std::string clade, size_t aa_pos);
    void add(const Node& node, bool show_line, std::string clade, size_t aa_pos);
    void add(const Tree& tree, const Node& first, const Node& last, bool show_line, std::string clade, size_t aa_pos);

    inline size_t shown_maps() const
        {
            return std::accumulate(sections.begin(), sections.end(), 0U, [](size_t a, const HzSection& section) -> size_t { return a + (section.show_map ? 1 : 0); });
        }

};

// ----------------------------------------------------------------------

class TreeDraw
{
 public:
    TreeDraw(acmacs::surface::Surface& aSurface, Tree& aTree, TreeDrawSettings& aSettings, HzSections& aHzSections);
    ~TreeDraw();

    void prepare();
    void draw();

    const Legend* coloring_legend() const;
    const Coloring& coloring() const { return *mColoring; }
    double vertical_step() const { return mVerticalStep; }

    void init_settings(const Clades* aClades);
    void ladderize();
    void detect_hz_lines_for_clades(const Clades* aClades, bool aForce = true);
    void set_line_no();
    acmacs::surface::Surface& surface() { return mSurface; }
    const HzSections& hz_sections() const { return mHzSections; }

    static constexpr const size_t sFirstLineNo = 1; // line of the first node is 1, we have 1 line space at the top and bottom of the tree

 private:
    struct AA_Transition
    {
        std::string first_leaf;
        std::string transition;
        acmacs::Location2D origin;
        size_t number_strains;
    };

    acmacs::surface::Surface& mSurface;
    Tree& mTree;
    TreeDrawSettings& mSettings;
    HzSections& mHzSections;
    std::unique_ptr<Coloring> mColoring;
    mutable std::unique_ptr<Legend> mColoringLegend;
    std::vector<AA_Transition> aa_transitions_;

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
    void draw_aa_transition(const Node& aNode, acmacs::Location2D aOrigin, double aRight);
    void draw_mark_with_label(const Node& aNode, acmacs::Location2D aTextOrigin);
    void report_aa_transitions();

    void fit_labels_into_viewport();
    void calculate_name_offset();

    double text_width(std::string text) { return mSurface.text_size(text, mFontSize, mSettings.label_style).width; }
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
