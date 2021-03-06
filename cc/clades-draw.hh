#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "acmacs-base/settings-v1.hh"
#include "acmacs-draw/surface.hh"
#include "signature-page/tree.hh"

// ----------------------------------------------------------------------

class TreeDraw;
class TimeSeriesDraw;
class SettingsInitializer;

// ----------------------------------------------------------------------

enum class CladeDrawSettingsLabelPosition { bottom, middle, top };

namespace acmacs::settings::v1
{
    template <> inline void field<CladeDrawSettingsLabelPosition>::assign(rjson::value& to, const CladeDrawSettingsLabelPosition& from)
    {
        switch (from) {
            case CladeDrawSettingsLabelPosition::bottom:
                to = "bottom";
                break;
            case CladeDrawSettingsLabelPosition::middle:
                to = "middle";
                break;
            case CladeDrawSettingsLabelPosition::top:
                to = "top";
                break;
        }
    }

    template <> inline CladeDrawSettingsLabelPosition field<CladeDrawSettingsLabelPosition>::extract(const rjson::value& from) const
    {
        if (from == "bottom")
            return CladeDrawSettingsLabelPosition::bottom;
        else if (from == "middle")
            return CladeDrawSettingsLabelPosition::middle;
        else if (from == "top")
            return CladeDrawSettingsLabelPosition::top;
        else
            throw std::runtime_error(fmt::format("Unrecognized CladeDrawSettingsLabelPosition: {}", from));
    }
} // namespace acmacs::settings::v1

// ----------------------------------------------------------------------

class CladeDrawSettings : public acmacs::settings::v1::object
{
 public:
    using acmacs::settings::v1::object::object;

    constexpr static const int NoSlot = -1;

    acmacs::settings::v1::field<std::string>                    name{this, "name", ""};
    acmacs::settings::v1::field<std::string>                    display_name{this, "display_name", ""};
    acmacs::settings::v1::field<bool>                           show{this, "show", true};
    acmacs::settings::v1::field<size_t>                         section_inclusion_tolerance{this, "section_inclusion_tolerance", 10}; // max number of lines (strains) within section from another clade that do not interrupt the secion
    acmacs::settings::v1::field<size_t>                         section_exclusion_tolerance{this, "section_exclusion_tolerance", 5}; // max number of lines (strains) to exclude small sections
    acmacs::settings::v1::field<bool>                           show_section_size_in_label{this, "show_section_size_in_label", true};
    acmacs::settings::v1::field<Color>                          arrow_color{this, "arrow_color", BLACK};
    acmacs::settings::v1::field<double>                         line_width{this, "line_width", 0.8};
    acmacs::settings::v1::field<double>                         arrow_width{this, "arrow_width", 3};
    acmacs::settings::v1::field<Color>                          separator_color{this, "separator_color", "grey63"};
    acmacs::settings::v1::field<double>                         separator_width{this, "separator_width", 0.5};
    acmacs::settings::v1::field<CladeDrawSettingsLabelPosition> label_position{this, "label_position", CladeDrawSettingsLabelPosition::middle};
    acmacs::settings::v1::field<acmacs::Offset>                 label_offset{this, "label_offset", {5, 0}};
    acmacs::settings::v1::field<Color>                          label_color{this, "label_color", BLACK};
    acmacs::settings::v1::field<double>                         label_size{this, "label_size", 11};
    acmacs::settings::v1::field<acmacs::TextStyle>              label_style{this, "label_style", acmacs::TextStyle{}};
    acmacs::settings::v1::field<double>                         label_rotation{this, "label_rotation", 0.0};
    acmacs::settings::v1::field<int>                            slot{this, "slot", NoSlot};
    acmacs::settings::v1::field<std::string>                    last_node{this, "last_node", ""}; // seq_id of the last node, to enforce the last node

}; // class CladeDrawSettings

// ----------------------------------------------------------------------

class CladesDrawSettings : public acmacs::settings::v1::object
{
 public:
    CladesDrawSettings(acmacs::settings::v1::base& parent) : acmacs::settings::v1::object::object(parent) { clades.append(); }

    acmacs::settings::v1::const_array_element<CladeDrawSettings> for_clade(std::string_view name) const;
    void hide_default_clade();

    acmacs::settings::v1::field_array_of<CladeDrawSettings> clades{this, "clades"};
    acmacs::settings::v1::field<double>                     slot_width{this, "slot_width", 10};

}; // class CladesDrawSettings

// ----------------------------------------------------------------------

class CladeSection
{
 public:
    CladeSection(const Node* node) : first(node), last(node) {}
    const Node* first;
    const Node* last;
};

class CladeData
{
  public:
    using slot_type = int;
    using seq_ids_entry_t = std::pair<std::string, std::string>;
    using seq_ids_t = std::vector<seq_ids_entry_t>;

    CladeData() : slot{CladeDrawSettings::NoSlot} {}
    CladeData(const Node& node) : sections{{&node}}, slot{CladeDrawSettings::NoSlot} {}

    void extend(const Node& node, size_t section_inclusion_tolerance);
    void remove_small_sections(size_t section_exclusion_tolerance);
    void set_last_node(const Node& node);

    const Node* first() const { return sections.front().first; }
    size_t first_line() const { return first()->draw.line_no; }
    const Node* last() const { return sections.back().last; }
    size_t last_line() const { return last()->draw.line_no; }

    bool shown() const { return slot != CladeDrawSettings::NoSlot; }

    seq_ids_t seq_ids() const
    {
        seq_ids_t result;
        std::transform(std::begin(sections), std::end(sections), std::back_inserter(result), [](const auto& sec) -> seq_ids_entry_t { return {sec.first->seq_id, sec.last->seq_id}; });
        return result;
    }

    std::vector<CladeSection> sections;
    slot_type slot;
};

using Clades = std::map<std::string, CladeData>; // clade name to data

std::ostream& operator << (std::ostream& out, const CladeSection& section);
std::ostream& operator << (std::ostream& out, const CladeData& clade);

// ----------------------------------------------------------------------

class CladesDraw
{
 public:
    CladesDraw(acmacs::surface::Surface& aSurface, Tree& aTree, const TreeDraw& aTreeDraw, const TimeSeriesDraw& aTimeSeriesDraw, CladesDrawSettings& aSettings)
        : mSurface(aSurface), mTree(aTree), mTreeDraw(aTreeDraw), mTimeSeriesDraw(aTimeSeriesDraw), mSettings(aSettings) {}

    void prepare();
    void draw();

    void init_settings(const SettingsInitializer& settings_initilizer, bool whocc_support);
    acmacs::surface::Surface& surface() { return mSurface; }
    const Clades* clades() const { return &mClades; }

 private:
    acmacs::surface::Surface& mSurface;
    Tree& mTree;
    const TreeDraw& mTreeDraw;
    const TimeSeriesDraw& mTimeSeriesDraw;
    CladesDrawSettings& mSettings;
    Clades mClades;

    void collect();
    void assign_slots();
    void draw_right(CladeData::slot_type aSlot, std::string aCladeName, double top, double bottom, double label_vpos, const CladeDrawSettings& for_clade);
    void draw_left(CladeData::slot_type aSlot, std::string aCladeName, double top, double bottom, double label_vpos, const CladeDrawSettings& for_clade);

}; // class CladesDraw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
