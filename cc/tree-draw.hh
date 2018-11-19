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

class AATransitionIndividualSettings;
class AATransitionPerBranchDrawSettings;

struct AATransitionIndividualSettingsForLabel
{
  public:
    AATransitionIndividualSettingsForLabel(const AATransitionPerBranchDrawSettings& src);
    void update(const AATransitionIndividualSettings& src);

    // std::string label;
    // std::string first_leaf_seq_id;
    bool show;
    double size;
    Color color;
    acmacs::TextStyle style;
    double interline;
    acmacs::Offset label_offset;
    double label_connection_line_width;
    Color label_connection_line_color;

}; // class AATransitionIndividualSettingsForLabel

// ----------------------------------------------------------------------

class AATransitionIndividualSettings : public acmacs::settings::object
{
  public:
    using acmacs::settings::object::object;

    void set_label_disabled_offset(std::string aLabel, std::string aFirstLeafSeqid, acmacs::Offset&& /*aLabelOffset*/)
    {
        label = aLabel;
        first_leaf_seq_id = aFirstLeafSeqid;
          // label_offset_commented = std::move(aLabelOffset);
    }

    acmacs::settings::field<std::string> label{this, "label", ""};
    acmacs::settings::field<std::string> first_leaf_seq_id{this, "first_leaf_seq_id", ""};
    acmacs::settings::field<bool> show{this, "show", true};
    acmacs::settings::field<double> size{this, "size", 8};
    acmacs::settings::field<Color> color{this, "color", "black"};
    acmacs::settings::field<acmacs::TextStyle> style{this, "style", {"Courier New"}};
    acmacs::settings::field<double> interline{this, "interline", 1.2};
    acmacs::settings::field<acmacs::Offset> label_offset{this, "label_offset", {-40, 20}};
    acmacs::settings::field<double> label_connection_line_width{this, "label_connection_line_width", 0.1};
    acmacs::settings::field<Color> label_connection_line_color{this, "label_connection_line_color", "black"};

    acmacs::settings::field<acmacs::Offset> label_offset_commented{this, "?label_offset"};

    void remove_for_tree_settings()
    {
        show.remove();
        size.remove();
        color.remove();
        style.remove();
        interline.remove();
        label_connection_line_width.remove();
        label_connection_line_color.remove();
    }

    void remove_for_signature_page_settings()
    {
        show.remove();
        size.remove();
        color.remove();
        style.remove();
        interline.remove();
        label_connection_line_width.remove();
        label_connection_line_color.remove();
    }

}; // class AATransitionIndividualSettings

// ----------------------------------------------------------------------

class AATransitionPerBranchDrawSettings : public acmacs::settings::object
{
 public:
    using acmacs::settings::object::object;

    acmacs::settings::field<bool>                                    show{this, "show", true};
    acmacs::settings::field<double>                                  size{this, "size", 8};
    acmacs::settings::field<Color>                                   color{this, "color", "black"};
    acmacs::settings::field<acmacs::TextStyle>                       style{this, "style", {"Courier New"}};
    acmacs::settings::field<double>                                  interline{this, "interline", 1.2};
    acmacs::settings::field<acmacs::Offset>                          label_offset{this, "label_offset", {-40, 20}};
    acmacs::settings::field<double>                                  scatter_label_offset{this, "scatter_label_offset", 0};
    acmacs::settings::field<std::string>                             scatter_label_offset_help{this, "scatter_label_offset?", "randomization range for label offset in trying to avoid clattering (mostly for figuring out good offsets"};
    acmacs::settings::field<double>                                  label_connection_line_width{this, "label_connection_line_width", 0.1};
    acmacs::settings::field<Color>                                   label_connection_line_color{this, "label_connection_line_color", "black"};
    acmacs::settings::field_array_of<AATransitionIndividualSettings> by_aa_label{this, "by_aa_label"};

    AATransitionIndividualSettingsForLabel settings_for_label(const AA_TransitionLabels& aLabels, std::string aFirstLeafSeqid) const;

    void remove_for_tree_settings()
    {
        show.remove();
        size.remove();
        color.remove();
        style.remove();
        interline.remove();
        label_connection_line_width.remove();
        label_connection_line_color.remove();
        for (size_t index = 0; index < by_aa_label.size(); ++index)
            by_aa_label[index]->remove_for_tree_settings();
    }

    void remove_for_signature_page_settings()
    {
        show.remove();
        size.remove();
        color.remove();
        style.remove();
        interline.remove();
        label_offset.remove();
        scatter_label_offset.remove();
        scatter_label_offset_help.remove();
        label_connection_line_width.remove();
        label_connection_line_color.remove();
        for (size_t index = 0; index < by_aa_label.size(); ++index)
            by_aa_label[index]->remove_for_signature_page_settings();
    }

}; // class AATransitionPerBranchDrawSettings

// ----------------------------------------------------------------------

class AATransitionDrawSettings : public acmacs::settings::object
{
 public:
    using acmacs::settings::object::object;

    acmacs::settings::field<bool>                                     show{this, "show", true};
    acmacs::settings::field<unsigned>                                 number_strains_threshold{this, "number_strains_threshold", 20}; // Do not show aa transition label if number_strains (leaf nodes) for the branch is less than this value.
    acmacs::settings::field<std::string>                              number_strains_threshold_help{this, "number_strains_threshold?", "do not show aa transition label if number_strains (leaf nodes) for the branch is less than this value"};
    acmacs::settings::field<bool>                                     show_empty_left{this, "show_empty_left", false};
    acmacs::settings::field_object<AATransitionPerBranchDrawSettings> per_branch{this, "per_branch"};
    acmacs::settings::field<bool>                                     show_node_for_left_line{this, "show_node_for_left_line", false};
    acmacs::settings::field<Color>                                    node_for_left_line_color{this, "node_for_left_line_color", "green"};
    acmacs::settings::field<double>                                   node_for_left_line_width{this, "node_for_left_line_width", 1};

    void remove_for_tree_settings() { per_branch->remove_for_tree_settings(); }

    void remove_for_signature_page_settings()
    {
        show.remove();
        number_strains_threshold.remove();
        number_strains_threshold_help.remove();
        show_empty_left.remove();
        show_node_for_left_line.remove();
        node_for_left_line_color.remove();
        node_for_left_line_width.remove();
        per_branch->remove_for_signature_page_settings();
    }

}; // class AATransitionDrawSettings

// ----------------------------------------------------------------------

class TreeDrawVaccineSettings : public acmacs::settings::object
{
 public:
    using acmacs::settings::object::object;

    acmacs::settings::field<std::string>       name{this, "name", ""};           // empty for default settings
    acmacs::settings::field<std::string>       name_help{this, "name?", "empty for default settings"};
    acmacs::settings::field<Color>             label_color{this, "label_color", "black"};
    acmacs::settings::field<double>            label_size{this, "label_size", 10};
    acmacs::settings::field<acmacs::TextStyle> label_style{this, "label_style", {}};
    acmacs::settings::field<Color>             line_color{this, "line_color", "black"};
    acmacs::settings::field<double>            line_width{this, "line_width", 1};

}; // class TreeDrawVaccineSettings

// class TreeDrawVaccines : public rjson::array_field_container_child<TreeDrawVaccineSettings>
// {
//  public:
//     using rjson::array_field_container_child<TreeDrawVaccineSettings>::array_field_container_child;
// };

// ----------------------------------------------------------------------

class TreeDrawMod : public acmacs::settings::object
{
 public:
    using acmacs::settings::object::object;

    acmacs::settings::field<std::string>       mod{this, "mod", ""};            // root, hide_isolated_before, hide_if_cumulative_edge_length_bigger_than, hide_between, before2015-58P-or-146I-or-559I
    acmacs::settings::field<std::string>       mod_help{this, "?mod"};
    acmacs::settings::field<std::string>       help{this, "?"};
    acmacs::settings::field<double>            d1{this, "d1"};
    acmacs::settings::field<std::string>       s1{this, "s1"};
    acmacs::settings::field<std::string>       s2{this, "s2"};

    // mark-clade-with-line mod
    acmacs::settings::field<std::string>       clade{this, "clade"};
    acmacs::settings::field<std::string>       color{this, "color"};
    acmacs::settings::field<double> line_width{this, "line_width"}; // -1, in pixels, mark-clade-with-line, mark-with-label

    // mark-with-label
    acmacs::settings::field<std::string>       seq_id{this, "seq_id"};
    acmacs::settings::field<std::string>       label{this, "label"};
    acmacs::settings::field<std::string>       label_color{this, "label_color"}; // "black"
    acmacs::settings::field<double>            label_size{this, "label_size"}; // 10.0
    acmacs::settings::field<std::string>       line_color{this, "line_color"};
    acmacs::settings::field<acmacs::TextStyle> label_style{this, "label_style"};
    acmacs::settings::field<acmacs::Offset>    label_offset{this, "label_offset"}; // {20, 20}

}; // class TreeDrawMod

// ----------------------------------------------------------------------

namespace acmacs::settings
{
    inline namespace v1
    {
        template <> inline void field<Tree::LadderizeMethod>::assign(rjson::value& to, const Tree::LadderizeMethod& from)
        {
            switch (from) {
              case Tree::LadderizeMethod::NumberOfLeaves:
                  to = "number-of-leaves";
                  break;
              case Tree::LadderizeMethod::MaxEdgeLength:
                  to = "max-edge-length";
                  break;
            }
        }

        template <> inline Tree::LadderizeMethod field<Tree::LadderizeMethod>::extract(const rjson::value& from) const
        {
            if (from == "number-of-leaves")
                return Tree::LadderizeMethod::NumberOfLeaves;
            else if (from == "max-edge-length")
                return Tree::LadderizeMethod::MaxEdgeLength;
            else
                throw std::runtime_error("Unrecognized Tree::LadderizeMethod: " + rjson::to_string(from));
        }
    }
}

// ----------------------------------------------------------------------

class TreeDrawSettings : public acmacs::settings::object
{
 public:
    TreeDrawSettings(acmacs::settings::base& parent) : acmacs::settings::object::object(parent)
        {
            auto mod = mods.append();
            mod->mod_help = "hide-if-cumulative-edge-length-bigger-than";
            mod->d1 = 0.04;
        }

    acmacs::settings::field<Tree::LadderizeMethod>              ladderize{this, "ladderize", Tree::LadderizeMethod::NumberOfLeaves};
    acmacs::settings::field<std::string>                        ladderize_help{this, "ladderize?", "number-of-leaves or max-edge-length"};
    acmacs::settings::field_array_of<TreeDrawMod>               mods{this, "mods"};
    acmacs::settings::field_array<std::string>                  mods_help{this, "mods_help",
                                                                          {"mods is a list of objects:", "{mod: root, s1: new-root}",
                                                                           "{mod: hide-isolated-before, s1: date}",
                                                                           "{mod: hide-if-cumulative-edge-length-bigger-than, d1: cumulative-length-threshold}",
                                                                           "{mod: before2015-58P-or-146I-or-559I}",
                                                                           "{mod: hide-between, s1: first-name-to-hide, s2: last-name-to-hide} - after ladderizing",
                                                                           "{mod: hide-one, s1: name-to-hide} - after ladderizing",
                                                                           "{mod: mark-with-line, s1: name-to-mark, s2: color-to-mark, d1: line-width-in-pixels}",
                                                                           "{mod: mark-clade-with-line, clade: , color: , line_width: line-width-in-pixels}",
                                                                           "{mod: mark-with-label, seq_id:, label:, line_color:, line_width:, label_offset:, label_size:, labeL_color:, label_style: }"}};
    acmacs::settings::field<bool>                               force_line_width{this, "force_line_width", false};
    acmacs::settings::field<double>                             line_width{this, "line_width", 1};
    acmacs::settings::field<double>                             root_edge{this, "root_edge", 0};
    acmacs::settings::field<Color>                              line_color{this, "line_color", "black"};
    acmacs::settings::field<acmacs::TextStyle>                  label_style{this, "label_style", {}};
    acmacs::settings::field<double>                             name_offset{this, "name_offset", 0.3};         // offset of the label from the line right end, in W widths
    acmacs::settings::field<std::string>                        color_nodes{this, "color_nodes", "continent"};    // black, continent, position number (e.g. 162)
    acmacs::settings::field<std::map<std::string, std::string>> color_for_aa{this, "color_for_aa"};            // for "color_nodes": "<position-number>"
    acmacs::settings::field<double>                             right_padding{this, "right_padding", 0};       // padding at the right to add space for the mark_with_line (for BVic del and triple-del mutants)
    acmacs::settings::field_object<AATransitionDrawSettings>    aa_transition{this, "aa_transition"};
    acmacs::settings::field_object<LegendSettings>              legend{this, "legend"};

    const acmacs::settings::const_array_element<TreeDrawMod> find_mark_with_label_mod(std::string aSeqId) const
    {
        if (auto found = mods.find_if([&](const auto& val) { return val.mod == "mark-with-label" && val.seq_id == aSeqId; }); found)
            return *found;
        throw std::runtime_error("Invalid tree.mods settings: cannot find mark-with-label for " + aSeqId);
    }

    void remove_for_tree_settings() { aa_transition->remove_for_tree_settings(); }

    void remove_for_signature_page_settings()
    {
        ladderize.remove();
        ladderize_help.remove();
        mods.remove();
        mods_help.remove();
        force_line_width.remove();
        line_width.remove();
        root_edge.remove();
        line_color.remove();
        label_style.remove();
        name_offset.remove();
        color_nodes.remove();
        legend.remove();
        aa_transition->remove_for_signature_page_settings();
    }

}; // class TreeDrawSettings

// ----------------------------------------------------------------------

class HzSection : public acmacs::settings::object
{
 public:
    using acmacs::settings::object::object;
    // HzSection(std::string aName = std::string{}, bool aShowLine = true);
    // HzSection(const Node& aFirst, bool aShow, bool aShowLine, bool aShowMap);

    acmacs::settings::field<bool>              show{this, "show", true};
    acmacs::settings::field<bool>              show_line{this, "show_line", true};
    acmacs::settings::field<bool>              show_label_in_time_series{this, "show_label_in_time_series", true};
    acmacs::settings::field<bool>              show_map{this, "show_map", true};
    acmacs::settings::field<std::string>       name{this, "name", ""};           // first seq_id
    acmacs::settings::field<std::string>       label{this, "label", ""};          // antigenic map label, empty - generate automatically
    acmacs::settings::field_array<std::string> triggering_clades{this, "triggering_clades"}; // clades that trigger this hz line
    acmacs::settings::field_array<size_t>      triggering_aa_pos{this, "triggering_aa_pos"};      // aa pos triggering this line
};

// ----------------------------------------------------------------------

class HzSections : public acmacs::settings::object
{
 public:
    using acmacs::settings::object::object;

    acmacs::settings::field<double>             vertical_gap{this, "vertical_gap", 20};
    acmacs::settings::field<Color>              line_color{this, "line_color", "grey63"};
    acmacs::settings::field<double>             line_width{this, "line_width", 1};
    acmacs::settings::field<double>             ts_label_size{this, "ts_label_size", 10};
    acmacs::settings::field<acmacs::TextStyle>  ts_label_style{this, "ts_label_style", {}};
    acmacs::settings::field<Color>              ts_label_color{this, "ts_label_color", "black"};
    acmacs::settings::field_array_of<HzSection> sections{this, "sections"};
    acmacs::settings::field<bool>               show_labels_in_time_series_in_tree_mode{this, "show_labels_in_time_series_in_tree_mode", false};

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

    size_t shown_maps() const
        {
            size_t result = 0;
            sections.for_each([&result](const HzSection& section) { if (section.show_map) ++result; });
            return result;
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
