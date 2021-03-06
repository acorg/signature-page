#pragma once

#include <string>
#include <limits>
#include <utility>
#include <memory>
#include <algorithm>
#include <optional>
#include <tuple>

#include "acmacs-draw/surface.hh"
#include "legend.hh"
#include "clades-draw.hh"

// ----------------------------------------------------------------------

class Node;
class Tree;
class Coloring;
class SettingsInitializer;
class SignaturePageDraw;

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
    std::optional<double> label_absolute_x;
    double label_connection_line_width;
    Color label_connection_line_color;

}; // class AATransitionIndividualSettingsForLabel

// ----------------------------------------------------------------------

class AATransitionIndividualSettings : public acmacs::settings::v1::object
{
  public:
    using acmacs::settings::v1::object::object;

    void set_label_disabled_offset(std::string_view aLabel, std::string_view aFirstLeafSeqid, acmacs::Offset&& /*aLabelOffset*/)
    {
        label = std::string{aLabel};
        first_leaf_seq_id = std::string{aFirstLeafSeqid};
          // label_offset_commented = std::move(aLabelOffset);
    }

    acmacs::settings::v1::field<std::string> label{this, "label", ""};
    acmacs::settings::v1::field<std::string> first_leaf_seq_id{this, "first_leaf_seq_id", ""};
    acmacs::settings::v1::field<bool> show{this, "show"};
    acmacs::settings::v1::field<double> size{this, "size"};
    acmacs::settings::v1::field<Color> color{this, "color"};
    acmacs::settings::v1::field<acmacs::TextStyle> style{this, "style"};
    acmacs::settings::v1::field<double> interline{this, "interline"};
    acmacs::settings::v1::field<acmacs::Offset> label_offset{this, "label_offset"};
    acmacs::settings::v1::field<double> label_absolute_x{this, "label_absolute_x"}; // if specified, ignore label_offset[0] and use this absolute x position
    acmacs::settings::v1::field<double> label_connection_line_width{this, "label_connection_line_width"};
    acmacs::settings::v1::field<Color> label_connection_line_color{this, "label_connection_line_color"};

    acmacs::settings::v1::field<acmacs::Offset> label_offset_commented{this, "?label_offset"};

    void remove_for_tree_settings();
    void remove_for_signature_page_settings();

}; // class AATransitionIndividualSettings

// ----------------------------------------------------------------------

class AATransitionPerBranchDrawSettings : public acmacs::settings::v1::object
{
 public:
    using acmacs::settings::v1::object::object;

    acmacs::settings::v1::field<bool>                                    show{this, "show", true};
    acmacs::settings::v1::field<double>                                  size{this, "size", 8};
    acmacs::settings::v1::field<Color>                                   color{this, "color", "black"};
    acmacs::settings::v1::field<acmacs::TextStyle>                       style{this, "style", acmacs::TextStyle{"Courier New"}};
    acmacs::settings::v1::field<double>                                  interline{this, "interline", 1.2};
    acmacs::settings::v1::field<acmacs::Offset>                          label_offset{this, "label_offset", {-40, 20}};
    acmacs::settings::v1::field<double>                                  scatter_label_offset{this, "scatter_label_offset", 0.0};
    acmacs::settings::v1::field<std::string>                             scatter_label_offset_help{this, "scatter_label_offset?", "randomization range for label offset in trying to avoid clattering (mostly for figuring out good offsets"};
    acmacs::settings::v1::field<double>                                  label_connection_line_width{this, "label_connection_line_width", 0.1};
    acmacs::settings::v1::field<Color>                                   label_connection_line_color{this, "label_connection_line_color", "black"};
    acmacs::settings::v1::field_array_of<AATransitionIndividualSettings> by_aa_label{this, "by_aa_label"};

    AATransitionIndividualSettingsForLabel settings_for_label(const AA_TransitionLabels& aLabels, std::string_view aFirstLeafSeqid) const;

    void remove_for_tree_settings();
    void remove_for_signature_page_settings();

}; // class AATransitionPerBranchDrawSettings

// ----------------------------------------------------------------------

class AATransitionDrawSettings : public acmacs::settings::v1::object
{
 public:
    using acmacs::settings::v1::object::object;

    acmacs::settings::v1::field<bool>                                     show{this, "show", true};
    acmacs::settings::v1::field<unsigned>                                 number_strains_threshold{this, "number_strains_threshold", 20}; // Do not show aa transition label if number_strains (leaf nodes) for the branch is less than this value.
    acmacs::settings::v1::field<std::string>                              number_strains_threshold_help{this, "number_strains_threshold?", "do not show aa transition label if number_strains (leaf nodes) for the branch is less than this value"};
    acmacs::settings::v1::field<bool>                                     show_empty_left{this, "show_empty_left", false};
    acmacs::settings::v1::field_object<AATransitionPerBranchDrawSettings> per_branch{this, "per_branch"};
    acmacs::settings::v1::field<bool>                                     show_node_for_left_line{this, "show_node_for_left_line", false};
    acmacs::settings::v1::field<Color>                                    node_for_left_line_color{this, "node_for_left_line_color", "green"};
    acmacs::settings::v1::field<double>                                   node_for_left_line_width{this, "node_for_left_line_width", 1};

    void remove_for_tree_settings() { per_branch->remove_for_tree_settings(); }

    void remove_for_signature_page_settings();

}; // class AATransitionDrawSettings

// ----------------------------------------------------------------------

class TreeDrawVaccineSettings : public acmacs::settings::v1::object
{
 public:
    using acmacs::settings::v1::object::object;

    acmacs::settings::v1::field<std::string>       name{this, "name", std::string{}};           // empty for default settings
    acmacs::settings::v1::field<std::string>       name_help{this, "name?", std::string{"empty for default settings"}};
    acmacs::settings::v1::field<Color>             label_color{this, "label_color", "black"};
    acmacs::settings::v1::field<double>            label_size{this, "label_size", 10};
    acmacs::settings::v1::field<acmacs::TextStyle> label_style{this, "label_style", acmacs::TextStyle{}};
    acmacs::settings::v1::field<Color>             line_color{this, "line_color", "black"};
    acmacs::settings::v1::field<double>            line_width{this, "line_width", 1};

}; // class TreeDrawVaccineSettings

// class TreeDrawVaccines : public rjson::array_field_container_child<TreeDrawVaccineSettings>
// {
//  public:
//     using rjson::array_field_container_child<TreeDrawVaccineSettings>::array_field_container_child;
// };

// ----------------------------------------------------------------------

class TreeDrawMod : public acmacs::settings::v1::object
{
 public:
    using acmacs::settings::v1::object::object;

    acmacs::settings::v1::field<std::string>       mod{this, "mod", ""};            // root, hide_isolated_before, hide_if_cumulative_edge_length_bigger_than, hide_between, hide-one, before2015-58P-or-146I-or-559I
    acmacs::settings::v1::field<std::string>       mod_help{this, "?mod"};
    acmacs::settings::v1::field<std::string>       help{this, "?"};
    acmacs::settings::v1::field<double>            d1{this, "d1"};
    acmacs::settings::v1::field<std::string>       s1{this, "s1"};
    acmacs::settings::v1::field<std::string>       s2{this, "s2"};

    // mark-clade-with-line mod
    acmacs::settings::v1::field<std::string>       clade{this, "clade"};
    acmacs::settings::v1::field<std::string>       color{this, "color"};
    acmacs::settings::v1::field<double>            line_width{this, "line_width"}; // -1, in pixels, mark-clade-with-line, mark-with-label
    acmacs::settings::v1::field<bool>              report{this, "report", false};

    // mark-country-with-line mod
    acmacs::settings::v1::field<std::string>       country{this, "country"};

    // mark-location-with-line mod
    acmacs::settings::v1::field<std::string>       location{this, "location"};

    // mark-with-label
    acmacs::settings::v1::field<std::string>       seq_id{this, "seq_id", ""};
    acmacs::settings::v1::field<std::string>       name{this, "name", ""};
    acmacs::settings::v1::field<std::string>       label{this, "label"};
    acmacs::settings::v1::field<std::string>       label_color{this, "label_color"}; // "black"
    acmacs::settings::v1::field<double>            label_size{this, "label_size"}; // 10.0
    acmacs::settings::v1::field<std::string>       line_color{this, "line_color"};
    acmacs::settings::v1::field<acmacs::TextStyle> label_style{this, "label_style"};
    acmacs::settings::v1::field<acmacs::Offset>    label_offset{this, "label_offset"}; // {20, 20}
    acmacs::settings::v1::field<double>            label_absolute_x{this, "label_absolute_x"}; // if specified, ignore label_offset[0] and use this absolute position
    acmacs::settings::v1::field<size_t>            collapse{this, "collapse", 10}; // do not draw the same label if it is too close (less than this number of lines) to the previously drawn label

}; // class TreeDrawMod

// ----------------------------------------------------------------------

namespace acmacs::settings::v1
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
            case Tree::LadderizeMethod::None:
                to = "none";
                break;
        }
    }

    template <> inline Tree::LadderizeMethod field<Tree::LadderizeMethod>::extract(const rjson::value& from) const
    {
        if (from == "number-of-leaves")
            return Tree::LadderizeMethod::NumberOfLeaves;
        else if (from == "max-edge-length")
            return Tree::LadderizeMethod::MaxEdgeLength;
        else if (from == "none")
            return Tree::LadderizeMethod::None;
        else
            throw std::runtime_error(fmt::format("Unrecognized Tree::LadderizeMethod: {}", from));
    }
} // namespace acmacs::settings::v1

// ----------------------------------------------------------------------

class TreeDrawSettings : public acmacs::settings::v1::object
{
 public:
    TreeDrawSettings(acmacs::settings::v1::base& parent) : acmacs::settings::v1::object::object(parent)
        {
            auto mod = mods.append();
            mod->mod_help = "hide-if-cumulative-edge-length-bigger-than";
            mod->d1 = 0.04;
        }

    acmacs::settings::v1::field<Tree::LadderizeMethod>              ladderize{this, "ladderize", Tree::LadderizeMethod::NumberOfLeaves};
    acmacs::settings::v1::field<std::string>                        ladderize_help{this, "ladderize?", "number-of-leaves or max-edge-length"};
    acmacs::settings::v1::field_array_of<TreeDrawMod>               mods{this, "mods"};
    acmacs::settings::v1::field_array<std::string>                  mods_help{this, "mods_help",
                                                                          {"mods is a list of objects:", "{mod: root, s1: new-root}",
                                                                           "{mod: hide-isolated-before, s1: date}",
                                                                           "{mod: hide-if-cumulative-edge-length-bigger-than, d1: cumulative-length-threshold}",
                                                                           "{mod: before2015-58P-or-146I-or-559I}",
                                                                           "{mod: hide-between, s1: first-name-to-hide, s2: last-name-to-hide} - after ladderizing",
                                                                           "{mod: hide-one, s1: name-to-hide} - after ladderizing",
                                                                           "{mod: hide-not-found-in-chart}",
                                                                           "{mod: mark-with-line, s1: name-to-mark, s2: color-to-mark, d1: line-width-in-pixels}",
                                                                           "{mod: mark-aa-with-line, s1: space-separated-pos-aa 183P 141E, s2: color-to-mark, d1: line-width-in-pixels, report: true}",
                                                                           "{mod: mark-clade-with-line, clade: , color: , line_width: line-width-in-pixels}",
                                                                           "{mod: mark-country-with-line, country: , color: , line_width: line-width-in-pixels}",
                                                                           "{mod: mark-location-with-line, location: , color: , line_width: line-width-in-pixels}",
                                                                           "{mod: mark-having-serum-with-line, color: , line_width: line-width-in-pixels}",
                                                                           "{mod: mark-with-label, seq_id:, name: <substring>, label:, line_color:, line_width:, label_offset:[0.0, 0.0], label_absolute_x:, label_size:, labeL_color:, label_style:, collapse: 10 }"}};
    acmacs::settings::v1::field<bool>                               force_line_width{this, "force_line_width", false};
    acmacs::settings::v1::field<double>                             line_width{this, "line_width", 1.0};
    acmacs::settings::v1::field<double>                             root_edge{this, "root_edge", 0.0};
    acmacs::settings::v1::field<Color>                              line_color{this, "line_color", "black"};
    acmacs::settings::v1::field<acmacs::TextStyle>                  label_style{this, "label_style", acmacs::TextStyle{}};
    acmacs::settings::v1::field<double>                             name_offset{this, "name_offset", 0.3};         // offset of the label from the line right end, in W widths
    acmacs::settings::v1::field<std::string>                        color_nodes{this, "color_nodes", "continent"};    // black, continent, position number (e.g. 162)
    acmacs::settings::v1::field<std::map<std::string, std::string>> color_for_aa{this, "color_for_aa"};            // for "color_nodes": "<position-number>"
    acmacs::settings::v1::field<double>                             right_padding{this, "right_padding", 0.0};       // padding at the right to add space for the mark_with_line (for BVic del and triple-del mutants)
    acmacs::settings::v1::field_object<AATransitionDrawSettings>    aa_transition{this, "aa_transition"};
    acmacs::settings::v1::field_object<LegendSettings>              legend{this, "legend"};

    // const acmacs::settings::v1::const_array_element<TreeDrawMod> find_mark_with_label_mod(std::string_view aSeqId) const
    // {
    //     if (auto found = mods.find_if([&](const auto& val) { return val.mod == "mark-with-label" && val.seq_id == aSeqId; }); found)
    //         return *found;
    //     throw std::runtime_error("Invalid tree.mods settings: cannot find mark-with-label for " + aSeqId);
    // }

    const acmacs::settings::v1::const_array_element<TreeDrawMod> find_mod(size_t mod_no) const
    {
        return mods[mod_no];
    }

    void remove_for_tree_settings() { aa_transition->remove_for_tree_settings(); }

    void remove_for_signature_page_settings();

}; // class TreeDrawSettings

// ----------------------------------------------------------------------

class HzSection : public acmacs::settings::v1::object
{
 public:
    using acmacs::settings::v1::object::object;
    // HzSection(std::string_view aName = std::string{}, bool aShowLine = true);
    // HzSection(const Node& aFirst, bool aShow, bool aShowLine, bool aShowMap);

    acmacs::settings::v1::field<bool>              show{this, "show", true};
    acmacs::settings::v1::field<bool>              show_line{this, "show_line", true};
    acmacs::settings::v1::field<bool>              show_label_in_time_series{this, "show_label_in_time_series", true};
    acmacs::settings::v1::field<bool>              show_map{this, "show_map", true};
    acmacs::settings::v1::field<std::string>       name{this, "name", ""};           // first seq_id
    acmacs::settings::v1::field<std::string>       name_commented{this, "?name", ""}; // first seq_id
    acmacs::settings::v1::field<std::string>       aa_transition{this, "aa_transition", ""};
    acmacs::settings::v1::field<std::string>       label{this, "label", ""};          // antigenic map label, empty - generate automatically
    acmacs::settings::v1::field_array<std::string> triggering_clades{this, "triggering_clades"}; // clades that trigger this hz line
    acmacs::settings::v1::field_array<size_t>      triggering_aa_pos{this, "triggering_aa_pos"};      // aa pos triggering this line

    void comment_out() { name_commented = name; name = ""; show = false; }
};

// ----------------------------------------------------------------------

class HzSections : public acmacs::settings::v1::object
{
 public:
    using acmacs::settings::v1::object::object;

    acmacs::settings::v1::field<double>             vertical_gap{this, "vertical_gap", 20};
    acmacs::settings::v1::field<Color>              line_color{this, "line_color", "grey63"};
    acmacs::settings::v1::field<double>             line_width{this, "line_width", 1};
    acmacs::settings::v1::field<double>             ts_label_size{this, "ts_label_size", 10};
    acmacs::settings::v1::field<acmacs::TextStyle>  ts_label_style{this, "ts_label_style", acmacs::TextStyle{}};
    acmacs::settings::v1::field<Color>              ts_label_color{this, "ts_label_color", "black"};
    acmacs::settings::v1::field_array_of<HzSection> sections{this, "sections"};
    acmacs::settings::v1::field<bool>               show_labels_in_time_series_in_tree_mode{this, "show_labels_in_time_series_in_tree_mode", false};

      // not stored

    struct NodeRef
    {
        NodeRef() = default;
        NodeRef(const Node* aFirst) : first{aFirst} {}
        const Node* first = nullptr;
        const Node* last = nullptr;
        std::string index;
    };
    std::vector<NodeRef> node_refs;
    std::vector<size_t> section_order;
    bool show = true;

    void sort(const Tree& aTree);
    void report(std::ostream& out) const;
    void report_html(std::string_view filename) const;
    // void auto_detect(Tree& aTree, const Clades* aClades);
    void detect_hz_lines_for_clades(Tree& aTree, const Clades* aClades, bool aForce);
    void convert_aa_transitions(Tree& tree); // to name based hz sections
    acmacs::settings::v1::array_element<HzSection> add(std::string_view seq_id, bool show_line, std::string_view clade, size_t aa_pos, bool first_in_clade);
    void add(const Tree& tree, const Node& first, const Node& last, bool show_line, std::string_view clade, size_t aa_pos);
    acmacs::settings::v1::array_element<HzSection> add(std::string_view aa_transition, bool show_line);
    auto find_section(std::string_view seq_id) const { return sections.find_if([&seq_id](const auto& sect) { return sect.name == std::string{seq_id}; }); }
    auto find_section(std::string_view seq_id)  { return sections.find_if([&seq_id](const auto& sect) { return sect.name == std::string{seq_id}; }); }

    size_t shown_maps() const
        {
            size_t result = 0;
            sections.for_each([&result](const HzSection& section, size_t) { if (section.show_map) ++result; });
            return result;
        }

}; // class HzSections

// ----------------------------------------------------------------------

class TreeDraw
{
 public:
    TreeDraw(SignaturePageDraw& aSignaturePageDraw, acmacs::surface::Surface& aSurface, Tree& aTree, TreeDrawSettings& aSettings, HzSections& aHzSections);
    ~TreeDraw();

    void prepare();
    void draw();

    const Legend* coloring_legend() const;
    const Coloring& coloring() const { return *mColoring; }
    double vertical_step() const { return mVerticalStep; }

    void init_settings(const Clades* aClades, const SettingsInitializer& settings_initilizer, bool whocc_support);
    void ladderize();
    void detect_hz_lines_for_clades(const Clades* aClades, bool aForce = true);
    void set_line_no();
    acmacs::surface::Surface& surface() { return mSurface; }
    const HzSections& hz_sections() const { return mHzSections; }
    HzSections& hz_sections() { return mHzSections; }
    const TreeDrawSettings& settings() const { return mSettings; }
    TreeDrawSettings& settings() { return mSettings; }
    const Tree& tree() const { return mTree; }
    Tree& tree() { return mTree; }

    static constexpr const size_t sFirstLineNo = 1; // line of the first node is 1, we have 1 line space at the top and bottom of the tree

 private:
    struct AA_Transition
    {
        std::string first_leaf;
        std::string transition;
        acmacs::PointCoordinates origin;
        size_t number_strains;
        bool operator<(const AA_Transition& rhs) const { return origin.y() < rhs.origin.y(); }
    };

    SignaturePageDraw& mSignaturePageDraw;
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
    std::optional<std::tuple<size_t, std::string>> last_marked_with_label_;

    bool apply_mods();          // returns if nodes were hidden
    void set_vertical_pos();
    size_t prepare_hz_sections();
    void draw_node(const Node& aNode, double aOriginX, double& aVerticalGap, double aEdgeLength = -1);
    void draw_legend();
    void draw_aa_transition(const Node& aNode, const acmacs::PointCoordinates& aOrigin, double aRight);
    void draw_mark_with_label(const Node& aNode, const acmacs::PointCoordinates& aTextOrigin);
    void report_aa_transitions();

    void fit_labels_into_viewport();
    void calculate_name_offset();

    double text_width(std::string_view text) { return mSurface.text_size(text, mFontSize, mSettings.label_style).width; }
    double max_label_offset();

    void make_coloring();

    // void unhide();
    void hide_isolated_before(std::string_view aDate);
    void hide_if_cumulative_edge_length_bigger_than(double aThreshold);
    void hide_before2015_58P_or_146I_or_559I();
    void hide_between(std::string_view aFirst, std::string_view aLast);
    void hide_one(std::string_view aName);
    void hide_not_found_in_chart();
    void mark_with_line(std::string_view aName, Color aColor, Pixels aLineWidth);
    void mark_aa_with_line(std::string_view aPos1AA, Color aColor, Pixels aLineWidth, bool aReport);
    void mark_clade_with_line(std::string_view aClade, Color aColor, Pixels aLineWidth, bool aReport);
    void mark_country_with_line(std::string_view aCountry, Color aColor, Pixels aLineWidth, bool aReport);
    void mark_location_with_line(std::string_view aLocation, Color aColor, Pixels aLineWidth, bool aReport);
    void mark_having_serum_with_line(Color aColor, Pixels aLineWidth);
    void mark_with_label(const TreeDrawMod& aMod, size_t mod_no);
    static void hide_branch(Node& aNode);

}; // class TreeDraw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
